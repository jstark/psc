#include <cctype>
#include <locale>
#include <algorithm>
#include "fe/token.h"
#include "pascal/error.h"
#include "pascal/scanner.h"
#include "pascal/token_type.h"

using namespace psc;
using namespace psc::pascal;

namespace
{

fe::Token word_token(fe::Source &source)
{
    std::string lexeme;
    char current = source.current();
    
    int line = source.current_line();
    int pos = source.current_pos();
    
    // Get the word characters (letter or digit). The scanner has already
    // determined that the first character is a letter.
    while (std::isalnum(current))
    {
        lexeme.append(1, current);
        current = source.current();
    }
    
    // is it a reserved word or an identifier ?
    std::string l_text(lexeme);
    std::transform(
        l_text.begin(),
        l_text.end(),
        l_text.begin(),
        ::tolower);
    const fe::TokenType *type = TokenType::reserved_word_type(l_text);
    if (!type)
    {
        type = &IDENTIFIER;
    }

    return fe::TokenBuilder()
                .at_line(line)
                .at_pos(pos)
                .with_type(type)
                .with_lexeme(lexeme)
                .build();
}
    
fe::Token number_token(fe::Source &source)
{

}
    
fe::Token string_token(fe::Source &source)
{
    int line = source.current_line();
    int pos = source.current_pos();
    
    std::string lexeme;
    std::string value;
    
    char current = source.next(); // consume initial quote
    lexeme.append(1, '\'');
    
    // deal with string characters
    do
    {
        // replace any whitespace character with a blank
        if (std::isspace(current))
        {
            current = ' ';
        }
        
        if (current != '\'' && current != fe::Source::END_OF_FILE)
        {
            lexeme.append(1, current);
            value.append(1, current);
            current = source.next(); // again consume
        }
        
        // quote ? each pair of adjacent quotes represent a single quote.
        if (current == '\'')
        {
            while (current == '\'' && source.peek() == '\'')
            {
                lexeme.append("''");
                value.append(1, '\'');
                current = source.next();
                current = source.next(); // consume both quotes
            }
        }
    } while(current != '\'' && current != fe::Source::END_OF_FILE);
    
    fe::TokenBuilder builder;
    if (current == '\'')
    {
        source.next(); // consume final quote
        lexeme.append(1, '\'');
        builder.with_type(&STRING).with_value(value).with_lexeme(lexeme);
    } else
    {
        builder.with_type(&ERROR).with_value(&UNEXPECTED_EOF);
    }
    return builder.at_pos(pos).at_line(line).build();
}
    
fe::Token special_token(fe::Source &source)
{
    char current = source.current();
    std::string lexeme(1, current);
    std::string value;
    int pos = source.current_pos();
    int line= source.current_line();
    
    const fe::TokenType *type = nullptr;
    
    switch (current) {
        case '+': case '-':  case '*': case '/': case ',':
        case ';': case '\'': case '=': case '(': case ')':
        case '[': case ']':  case '{': case '}': case '^':
            source.next();
            break;
        case ':':
        {
            current = source.next(); // consume ':'
            if (current == '=')
            {
                lexeme.append(1, current);
                source.next(); // consume '='
            }
            
        }
            break;
        case '<':
        {
            current = source.next();
            if (current == '=')
            {
                lexeme.append(1, current);
                source.next();
            } else if (current == '>')
            {
                lexeme.append(1, current);
                source.next();
            }
        }
            break;
        case '>':
        {
            current = source.next();
            if (current == '=')
            {
                lexeme.append(1, current);
                source.next();
            }
        }
            break;
        case '.':
        {
            current = source.next();
            if (current == '.')
            {
                lexeme.append(1, current);
                source.next();
            }
        }
            break;
        default:
            source.next();
            type = &ERROR;
            value = INVALID_CHARACTER.text();
            break;
    }
    
    if (type == nullptr)
    {
        type = TokenType::special_symbol_type(lexeme);
    }
    
    fe::TokenBuilder builder;
    return builder.with_lexeme(lexeme)
                  .with_value(value)
                  .at_pos(pos)
                  .at_line(line)
                  .with_type(type).build();
    
}
    
}

Scanner::Scanner(fe::Source &&source)
    : fe::Scanner(std::move(source)) {}

fe::Token Scanner::extract_token() 
{
    skipws();
    
    char current = current_char();
    
    fe::Source &source = the_source();
    if (current == fe::Source::END_OF_FILE)
    {
        fe::TokenBuilder builder;
        return builder
            .with_type(&END_OF_FILE)
            .at_line(source.current_line())
            .at_pos(source.current_pos())
            .build();
    }
    
    fe::Token token;
    if (std::isalpha(current))
    {
        token = word_token(source);
    } else if (std::isdigit(current))
    {
        token = number_token(source);
    } else if (current == '\'')
    {
        token = string_token(source);
    } else if (TokenType::is_special_symbol(std::string(1, current)))
    {
        token = special_token(source);
    } else
    {
        fe::TokenBuilder builder;
        token = builder
                    .with_type(&ERROR)
                    .with_value(INVALID_CHARACTER.text())
                    .at_line(source.current_line())
                    .at_pos(source.current_pos())
                    .build();
        source.next();
    }

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

