#include <boost/program_options.hpp>
#include <boost/any.hpp>
#include <string>
#include <vector>
#include <iostream>

#include "msg/message.h"
#include "fe/source.h"
#include "pascal/scanner.h"
#include "pascal/parser.h"

using std::string;
using std::vector;
using std::shared_ptr;
using std::ifstream;
using boost::any;

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
    vector<any> args = message.args();
    
    if (type == msg::MessageType::SourceLine)
    {
        int lnum = boost::any_cast<int>(args.at(0));
        string text = boost::any_cast<string>(args.at(1));
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
">>> %-15s line=%03d, pos=%2d, text=\"%s\"";

static const char *VALUE =
">>>                  value=%s";

void ParserMessageListener::receive_msg(const msg::Message &message)
{
    msg::MessageType type = message.type();
    vector<any> args = message.args();
    
    switch(type)
    {
        case msg::MessageType::ParserSummary:
        {
            int stmnt_count = boost::any_cast<int>(args.at(0));
            int syntax_errors = boost::any_cast<int>(args.at(1));
            double elapsed_time = boost::any_cast<double>(args.at(2));
            printf(PARSER_SUMMARY_FORMAT, stmnt_count, syntax_errors, elapsed_time);
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

    // command-line only options
    po::options_description generic("Generic options");
    generic.add_options()
        ("version,v", "prints the compiler's version")
        ("help,h", "produces this help message")
        ("config,c", po::value<string>(&config_file)->default_value("psc.cfg"),
         "name of the compiler's config file");

    // options both in the command-line AND in the config file
    po::options_description config("Configuration");
    config.add_options()
        ("crossref,x", po::bool_switch(&xref)->default_value(false), "performs cross-reference for identifiers")
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
            fe::Source src(std::move(stream));
            auto src_l = std::make_shared<SourceMessageListener>();
            src.add(src_l);
            pascal::Parser parser(std::move(pascal::Scanner(std::move(src))));
            auto parser_l = std::make_shared<ParserMessageListener>();
            parser.add(parser_l);
            parser.parse();
        }
    } 
    catch(...)
    {
        std::cout << visible << std::endl;
    }
    return 0;
}

