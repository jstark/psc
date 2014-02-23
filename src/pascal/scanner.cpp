#include "fe/token.h"
#include "pascal/scanner.h"

using namespace psc;
using namespace psc::pascal;

Scanner::Scanner(fe::Source &&source)
    : fe::Scanner(std::move(source)) {}

fe::Token Scanner::extract_token() 
{
    skipws();
    
    fe::Token token;
    char current = current_char();

    // construct the next token. the current character 
    // determines the token type.
    return token;
}

void Scanner::skipws()
{
    char current = current_char();
    while (std::isspace(current) || current == '{')
    {
        if (current == '{')
        {
            do
            {
                current = next_char(); // consume comment characters
            } while (current != '}' && current != fe::Source::END_OF_FILE);
            
            // found closing '}' ?
            if (current == '}')
            {
                current = next_char();
            }
        } else
        {
            current = next_char(); // consume whitespace characters
        }
    }
}

