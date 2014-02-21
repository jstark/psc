#include <boost/program_options.hpp>
#include <string>
#include <vector>
#include <iostream>

#include "msg/message.h"
#include "fe/source.h"
#include "pascal/scanner.h"
#include "pascal/parser.h"
#include "fe/listeners.h"

using std::string;
using std::vector;
using std::shared_ptr;
using std::ifstream;

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
            auto src_l = std::make_shared<fe::SourceMessageListener>();
            src.add(src_l);
            pascal::Parser parser(std::move(pascal::Scanner(std::move(src))));
            auto parser_l = std::make_shared<fe::ParserMessageListener>();
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

