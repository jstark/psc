#include "fe/listeners.h"
#include <vector>
#include <string>
#include <boost/any.hpp>

using namespace psc::fe;
using namespace psc::msg;
using std::vector;
using std::string;
using boost::any;

static const char *SRC_LINE_FORMAT = "%03d %s\n";

void SourceMessageListener::receive_msg(const Message &message)
{
    MessageType type = message.type();
    vector<any> args = message.args();

    if (type == MessageType::SourceLine)
    {
        int lnum = boost::any_cast<int>(args.at(0));
        string text = boost::any_cast<string>(args.at(1));
        printf(SRC_LINE_FORMAT, lnum, text.c_str());
    }
}

static const char *PARSER_SUMMARY_FORMAT =
"\n%20d source lines "
"\n%20d syntax errors"
"\n%20.2f seconds total parsing time.\n";

static const char *PARSER_TOKEN_FORMAT =
">>> %-15s line=%03d, pos=%2d, text=\"%s\"";

static const char *VALUE = 
">>>                  value=%s";

void ParserMessageListener::receive_msg(const Message &message)
{
    MessageType type = message.type();
    vector<any> args = message.args();

    switch(type)
    {
    case MessageType::ParserSummary:
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
