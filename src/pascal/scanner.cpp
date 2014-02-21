#include "fe/token.h"
#include "pascal/scanner.h"

using namespace psc;
using namespace psc::pascal;

Scanner::Scanner(fe::Source &&source)
    : fe::Scanner(std::move(source)) {}

fe::Token Scanner::extract_token() 
{
    fe::Token token;
    char current = current_char();

    // construct the next token. the current character 
    // determines the token type.
    fe::TokenExtractor extractor;
    token = extractor.extract(the_source());
    return token;
}

