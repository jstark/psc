#include <cctype>
#include <locale>
#include <algorithm>
#include <tuple>
#include "fe/token.h"
#include "pascal/error.h"
#include "pascal/scanner.h"
#include "pascal/token_type.h"
#include "utils/var.h"

using namespace psc;
using namespace psc::pascal;

namespace
{
    
using std::string;
using std::tuple;

fe::Token word_token(fe::Source &source)
{
    string lexeme;
    char current = source.current();
    
    int line = source.current_line();
    int pos = source.current_pos();
    
    // Get the word characters (letter or digit). The scanner has already
    // determined that the first character is a letter.
    while (std::isalnum(current) || current == '_')
    {
        lexeme.append(1, current);
        current = source.next();
    }
    
    // is it a reserved word or an identifier ?
    string l_text(lexeme);
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
    
using unsigned_digits_t = tuple<const fe::TokenType *, string, utils::var, string>;
    
unsigned_digits_t unsigned_digits(fe::Source &source)
{
    char current = source.current();
    
    string text;
    utils::var val;
    string digits;
    const fe::TokenType *type = nullptr;
    // must have at least one digit
    if (!std::isdigit(current))
    {
        type = &ERROR;
        val  = &INVALID_NUMBER;
    } else
    {
        while (std::isdigit(current))
        {
            text.append(1, current);
            digits.append(1, current);
            current = source.next(); // consume digit
        }
    }
    
    return std::make_tuple(type, text, val, digits);
}
    
using int_value_t = tuple<const fe::TokenType *, int, utils::var>;
    
int_value_t compute_int_val(const string& digits)
{
    int val = 0;
    if (!digits.empty())
    {
        int prev = -1;
        int index= 0;
        
        // loop over the digits to compute the integer value
        // as long as there is no overflow.
        while (index < digits.size() && val >= prev)
        {
            prev = val;
            val = 10 * val + (digits[index++] - '0');
        }
        
        // no overflow: return the int value
        if (val >= prev)
        {
            return std::make_tuple(&INTEGER, val, 0);
        } else
        {
            return std::make_tuple(&ERROR, 0, &RANGE_INTEGER);
        }
    }
    return std::make_tuple(&INTEGER, 0, 0);
}
    
using real_value_t = tuple<const fe::TokenType *, double, utils::var>;
    
real_value_t compute_real_val(const string& whole, const string& frac,
                              const string& expd , char sign)
{
    const fe::TokenType *err = nullptr;
    utils::var _;
    
    int exp_val = 0;
    std::tie(err, exp_val, _) = compute_int_val(expd);
    if (err == &ERROR)
    {
        return std::make_tuple(err, 0, &INVALID_NUMBER);
    }
    if (sign == '-')
    {
        exp_val = -exp_val;
    }
    
    string digits = whole;
    if (!frac.empty())
    {
        exp_val -= frac.size();
        digits.append(frac);
    }
    
    // out of range check
    int x = std::abs(exp_val + (int)whole.size());
    if (x > std::numeric_limits<double>::max_exponent)
    {
        return std::make_tuple(&ERROR, 0, &RANGE_REAL);
    }
    
    // loop over the digits to compute the double value
    int index = 0;
    double dval = 0;
    while (index < digits.size())
    {
        dval = 10 * dval + (digits[index++] - '0');
    }
    
    // adjust due to exponent
    if (exp_val != 0)
    {
        dval *= std::pow(10.0, exp_val);
    }
    
    return std::make_tuple(&REAL, dval, 0);
}
    
fe::Token number_token(fe::Source &source)
{
    int line = source.current_line();
    int pos = source.current_pos();
    std::string lexeme, whole_digits;
    utils::var value;
    
    const fe::TokenType *err = nullptr;
    std::tie(err, lexeme, value, whole_digits) = unsigned_digits(source);
    
    if (err == &ERROR)
    {
        fe::TokenBuilder builder;
        return builder.with_type(err)
                      .with_lexeme(lexeme)
                      .at_pos(pos)
                      .at_line(line)
                      .with_value(value)
                      .build();
    }
    
    // assume int
    const fe::TokenType *type = &INTEGER;
    
    // Is there a . ?
    // It could be a decimal point or the start of a .. token
    char current = source.current();
    bool saw_dot_dot = false;
    string frac_digits;
    if (current == '.')
    {
        if (source.peek() == '.')
        {
            saw_dot_dot = true; // it's a ".." token, so don't consume it
        } else
        {
            type = &REAL; // decimal point, so token type is REAL.
            lexeme.append(1, current);
            current = source.next(); // consume decimal point
            
            // collect fraction part of the number
            err = nullptr;
            string frac_lexeme;
            std::tie(err, frac_lexeme, value, frac_digits) = unsigned_digits(source);
            lexeme.append(frac_lexeme);
            if (err == &ERROR)
            {
                fe::TokenBuilder builder;
                return builder.with_type(err)
                              .with_lexeme(lexeme)
                              .at_pos(pos)
                              .at_line(line)
                              .with_value(value)
                              .build();
            }
        }
    }
    
    // is there an exponent part ?
    // There cannot be an exponent if we already saw a ".." token.
    current = source.current();
    char exponent_sign = '+';
    string exp_digits;
    if (!saw_dot_dot && (current == 'E' || current == 'e'))
    {
        type = &REAL;
        lexeme.append(1, current);
        current = source.next(); // consume 'E' or 'e'
        
        // exponent sign ?
        if (current == '+' || current == '-')
        {
            lexeme.append(1, current);
            exponent_sign = current;
            current = source.next();
            
        }
        
        // extract the digits of the exponent.
        err = nullptr;
        std::tie(err, exp_digits, value, exp_digits) = unsigned_digits(source);
        lexeme.append(exp_digits);
        if (err == &ERROR)
        {
            fe::TokenBuilder builder;
            return builder.with_type(err)
                          .with_lexeme(lexeme)
                          .at_pos(pos)
                          .at_line(line)
                          .with_value(value)
                          .build();
        }
    }
    
    // compute the value of an integer number token
    if (type == &INTEGER)
    {
        int val = 0;
        std::tie(err, val, value) = compute_int_val(whole_digits);
        if (err == &ERROR)
        {
            fe::TokenBuilder builder;
            return builder.with_type(err)
                          .with_lexeme(lexeme)
                          .at_pos(pos)
                          .at_line(line)
                          .with_value(value)
                          .build();
        } else
        {
            value = val;
        }
    } else if (type == &REAL)
    {
        double val = 0;
        std::tie(err, val, value) = compute_real_val(whole_digits, frac_digits,
                                        exp_digits, exponent_sign);
        if (err == &ERROR)
        {
            fe::TokenBuilder builder;
            return builder.with_type(err)
                          .with_lexeme(lexeme)
                          .at_pos(pos)
                          .at_line(line)
                          .with_value(value)
                          .build();
        } else
        {
            value = val;
        }
    }
    
    fe::TokenBuilder builder;
    return builder.with_type(type)
                  .with_lexeme(lexeme)
                  .at_pos(pos)
                  .at_line(line)
                  .with_value(value)
                  .build();
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
                    .with_lexeme(string(1, current))
                    .with_type(&ERROR)
                    .with_value(&INVALID_CHARACTER)
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

