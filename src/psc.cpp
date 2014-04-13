#include <boost/program_options.hpp>
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>

#include "msg/message.h"
#include "fe/source.h"
#include "im/symbol_table.h"
#include "im/icode.h"
#include "im/xref.h"
#include "im/parse_tree_printer.h"
#include "utils/var.h"
#include "pascal/scanner.h"
#include "pascal/parser.h"
#include "pascal/token_type.h"
#include "be/backend.h"

using std::string;
using std::vector;
using std::shared_ptr;
using std::unique_ptr;
using std::ifstream;

using namespace psc;

class SourceMessageListener : public msg::MessageListener
{
public:
    void receive_msg(const msg::Message &message) override;
};

static const char *SRC_LINE_FORMAT = "%03d %s\n";

void SourceMessageListener::receive_msg(const msg::Message &message)
{
    msg::MessageType type = message.type();
    vector<utils::var> args = message.args();
    
    if (type == msg::MessageType::SourceLine)
    {
        int lnum = boost::get<int>(args.at(0));
        string text = boost::get<string>(args.at(1));
        printf(SRC_LINE_FORMAT, lnum, text.c_str());
    }
}

class ParserMessageListener final: public msg::MessageListener
{
public:
    void receive_msg(const msg::Message &message) override;
};

static const char *PARSER_SUMMARY_FORMAT =
"\n%20d source lines "
"\n%20d syntax errors"
"\n%20.2f seconds total parsing time.\n";

static const char *PARSER_TOKEN_FORMAT =
">>> %-15s line=%03d, pos=%2d, text=\"%s\"\n";

static const char *VALUE =
">>>                  value=\"%s\"\n";

static const int PREFIX_WIDTH = 5;

void ParserMessageListener::receive_msg(const msg::Message &message)
{
    msg::MessageType type = message.type();
    vector<utils::var> args = message.args();
    
    switch(type)
    {
        case msg::MessageType::ParserSummary:
        {
            int stmnt_count = boost::get<int>(args.at(0));
            int syntax_errors = boost::get<int>(args.at(1));
            double elapsed_time = boost::get<double>(args.at(2));
            printf(PARSER_SUMMARY_FORMAT, stmnt_count, syntax_errors, elapsed_time);
        }
            break;
        case msg::MessageType::Token:
        {
            int line = boost::get<int>(args.at(0));
            int pos = boost::get<int>(args.at(1));
            auto type = (const pascal::TokenType *)boost::get<const void *>(args.at(2));
            string text = boost::get<string>(args.at(3));
            utils::var value = args.at(4);
            printf(PARSER_TOKEN_FORMAT, type->text().c_str(), line, pos, text.c_str());
            if (value.which() != 3 || boost::get<void *>(value))
            {
                string s = boost::apply_visitor(utils::to_str_visitor(), value);
                printf(VALUE, s.c_str());
            }
        }
            break;
        case msg::MessageType::SyntaxError:
        {
            int line = boost::get<int>(args.at(0));
            int pos = boost::get<int>(args.at(1));
            string text = boost::get<string>(args.at(2));
            string errmsg = boost::get<string>(args.at(3));
            
            int space = PREFIX_WIDTH + pos;
            string flagBuffer(space-1, ' ');
            flagBuffer.append("^\n*** ").append(errmsg);
            
            // text if any of the bad token
            if (!text.empty())
            {
                flagBuffer.append(" [at \"").append(text).append("\"]");
            }
            printf("%s\n", flagBuffer.c_str());
        }
            break;
        default:
            break;
    }
}

//static const char* INTERPRETER_SUMMARY_FORMAT = "\n%d %20s \n%d %20s \n%8.2f %20s";
static const char* ASSIGN_FORMAT = " >>> LINE %*d: %s = %s \n";

class BackendMessageListener final: public msg::MessageListener
{
public:
    void receive_msg(const msg::Message &message) override;

private:
    bool first_output{true};
};

void BackendMessageListener::receive_msg(const msg::Message &message)
{
    msg::MessageType type = message.type();
    vector<utils::var> args = message.args();

    switch(type)
    {
    case msg::MessageType::InterpreterSummary:
    {
        auto item1 = boost::get<int>(args.at(0));
        auto item2 = boost::get<int>(args.at(1));
        auto item3 = boost::get<double>(args.at(2));
		std::cout << std::endl << std::setw(20) << item1 << " statements executed"
			      << std::endl << std::setw(20) << item2 << " runtime errors"
			      << std::endl << std::setw(20) << std::fixed << std::setprecision(2) << item3 << " seconds total execution time.";
    }
        break;
    case msg::MessageType::Assign:
    {
        if (first_output)
        {
            printf("\n===== OUTPUT =====\n");
            first_output = false;
        }

        auto line_number = boost::get<int>(args.at(0));
        auto variable = boost::get<std::string>(args.at(1));
        auto value = boost::get<std::string>(args.at(2));
        printf(ASSIGN_FORMAT, 3, line_number, variable.c_str(), value.c_str());
    }
        break;
    case msg::MessageType::RuntimeError:
    {
        auto error_msg = boost::get<std::string>(args.at(0));
        auto line_num = boost::get<int>(args.at(1));

        printf("*** RUNTIME ERROR");
        printf(" AT LINE %d", line_num);
        printf(" : %s\n", error_msg.c_str());
    }
        break;
    default:
        break;
    }
}

namespace po = boost::program_options;

int main(int argc, char *argv[])
{
    string config_file;
    bool xref = false;
    bool syntax_only = false;
    bool ast = false;

    // command-line only options
    po::options_description generic("Generic options");
    generic.add_options()
        ("version,v", "prints the compiler's version")
        ("help,h", "produces this help message")
        ("config,c", po::value<string>(&config_file)->default_value("psc.cfg"),
         "name of the compiler's config file")
        ("syntax-only,s", po::bool_switch(&syntax_only)->default_value(false), "perform only a syntax check");

    // options both in the command-line AND in the config file
    po::options_description config("Configuration");
    config.add_options()
        ("crossref,x", po::bool_switch(&xref)->default_value(false), "performs cross-reference for identifiers")
        ("ast,i", po::bool_switch(&ast)->default_value(false), "print the AST of the source")
        ("action,a", "either compile or interpret");

    // hidden options are allowed both in the command-line arguments, and 
    // in the config file, but will not be shown to the user.
    po::options_description hidden("Hidden options");
    hidden.add_options()
        ("sources", po::value<vector<string>>(), "sources");

    // create groups
    po::options_description cmd_opts;
    cmd_opts.add(generic).add(config).add(hidden);

    po::options_description config_opts;
    config_opts.add(config).add(hidden);

    po::options_description visible("Allowed options");
    visible.add(generic).add(config);

    po::positional_options_description pos;
    pos.add("sources", -1);

    try
    {
        using namespace psc;

        po::variables_map vm;
        auto cmd_parser = po::command_line_parser(argc, argv).options(cmd_opts).positional(pos).run();
        po::store(cmd_parser, vm);
        po::notify(vm);

        if (vm.count("help"))
        {
            std::cout << visible << std::endl;
            return 0;
        }

        vector<string> sources;
        if (vm.count("sources"))
        {
            sources = vm["sources"].as<vector<string>>();
        }

        if (sources.empty())
        {
            std::cout << "No files given!" << std::endl;
            return 0;
        }

        ifstream stream(sources[0]);
        if (!stream)
        {
            std::cout << "Invalid file!" << std::endl;
        } else 
        {
			unique_ptr<im::ICode> icode = nullptr;
			unique_ptr<im::SymbolTableStack> symtabstack;

            // ready source
            SourceMessageListener sml;
            psc::msg::MessageProducer smp;
            smp.add(&sml);
            fe::Source src(std::move(stream), smp);

            // ready scanner and parser
            ParserMessageListener pml;
            psc::msg::MessageProducer pmp;
            pmp.add(&pml);
            pascal::Parser parser(std::move(pascal::Scanner(std::move(src))), pmp);

            // do work
            std::tie(icode, symtabstack) = parser.parse();
			if (parser.error_count() == 0)
			{
				if (xref)
				{
					im::CrossReferencer xr;
					xr.print(symtabstack.get());
				}

				if (ast)
				{
					im::ParseTreePrinter printer(std::cout);
					printer.print(icode.get());
				}

				BackendMessageListener bml;
				psc::msg::MessageProducer bmp;
				bmp.add(&bml);
				auto backend = be::Backend::create("intepret", bmp);
				backend->process(std::move(icode), std::move(symtabstack));
			}
        }
    } 
    catch(...)
    {
        std::cout << visible << std::endl;
    }
    return 0;
}

