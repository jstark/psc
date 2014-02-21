#ifndef _PSC_PASCAL_TOKEN_TYPE_H_
#define _PSC_PASCAL_TOKEN_TYPE_H_

#include "fe/token.h"
#include <unordered_map>
#include <string>

namespace psc { namespace pascal { 

enum class TokenSubtype
{
    Reserved, 
    Special,
    Other
};

class TokenType : public fe::TokenType
{
public:
    TokenType(const std::string& name, TokenSubtype subtype, int id = -1);
    std::string text() const { return name; }

    static const TokenType* reserved_word_type(const std::string& key);
    static const TokenType* special_symbol_type(const std::string& key);
    static bool is_special_symbol(const std::string& key);
private:
    std::string name;
    int type_id;

    static std::unordered_map<std::string, const TokenType *> reserved_words;
    static std::unordered_map<std::string, const TokenType *> special_symbols;
};

#define RESERVED    \
        X(AND)          \
        X(ARRAY)        \
        X(BEGIN)        \
        X(CASE)         \
        X(CONST)        \
        X(DIV)          \
        X(DO)           \
        X(DOWNTO)       \
        X(ELSE)         \
        X(END)          \
        X(FILe)         \
        X(FOR)          \
        X(FUNCTION)     \
        X(GOTO)         \
        X(IF)           \
        X(IN)           \
        X(LABEL)        \
        X(MOD)          \
        X(NIL)          \
        X(NOT)          \
        X(OF)           \
        X(OR)           \
        X(PACKED)       \
        X(PROCEDURE)    \
        X(PROGRAM)      \
        X(RECORD)       \
        X(REPEAT)       \
        X(SET)          \
        X(THEN)         \
        X(TO)           \
        X(TYPE)         \
        X(UNTIL)        \
        X(VAR)          \
        X(WHILE)        \
        X(WITH)


#define SPECIAL     \
        X(PLUS  , "+")  \
        X(MINUS , "-")  \
        X(STAR  , "*")  \
        X(SLASH , "/")  \
        X(COLON_EQUALS  , ":=") \
        X(DOT   , ".")  \
        X(COMMA , ",")  \
        X(SEMICOLON     , ";" ) \
        X(COLON , ":")  \
        X(QUOTE , "'")  \
        X(EQUALS, "=")  \
        X(NOT_EQUALS    , "<>") \
        X(LESS_THAN     , "<" ) \
        X(LESS_EQUALS   , "<=") \
        X(GREATER_EQUALS, ">=") \
        X(GREATER_THAN  , ">" ) \
        X(LEFT_PAREN    , "(" ) \
        X(RIGHT_PAREN   , ")" ) \
        X(LEFT_BRACKET  , "[" ) \
        X(RIGHT_BRACKET , "]" ) \
        X(LEFT_BRACE    , "{" ) \
        X(RIGHT_BRACE   , "}" ) \
        X(UP_ARROW      , "^" ) \
        X(DOT_DOT       , "..")


#define OTHER       \
        X(IDENTIFIER)   \
        X(INTEGER)      \
        X(REAL)         \
        X(STRING)       \
        X(ERROR)        \
        X(END_OF_FILE)  
          

#define DECLARE_TOKENTYPE(name)\
        extern const TokenType name;

#define X DECLARE_TOKENTYPE
    RESERVED
#undef X

#define DECLARE_TOKENTYPE_2(name, sym) \
            DECLARE_TOKENTYPE(name)

#define X DECLARE_TOKENTYPE_2
        SPECIAL
#undef X

#define X DECLARE_TOKENTYPE
        OTHER
#undef X

}}

#endif // _PSC_PASCAL_TOKEN_TYPE_H_

