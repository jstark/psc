#include "pascal/token_type.h"
#include <algorithm>

using namespace psc::pascal;

static int tokentype_id = 0;
std::unordered_map<std::string, const TokenType *> TokenType::reserved_words;
std::unordered_map<std::string, const TokenType *> TokenType::special_symbols;

TokenType::TokenType(const std::string& n, TokenSubtype s, int id)
    : name(n)
{
    if (id == -1)
    {
        type_id = tokentype_id++;
    } else 
    {
        type_id = id;
    }

    std::string lname = n;
    std::transform(lname.begin(), lname.end(), lname.begin(), ::tolower);
    if (s == TokenSubtype::Reserved)
    {
        reserved_words.insert( { lname, this } );
    } else if (s == TokenSubtype::Special)
    {
        special_symbols.insert( { lname, this } );
    }
}

bool TokenType::is_special_symbol(const std::string& key)
{
    return special_symbols.find(key) != special_symbols.end();
}

const TokenType* TokenType::reserved_word_type(const std::string& key)
{
    std::string s(key);
    std::transform(key.begin(), key.end(), s.begin(), ::tolower);
    auto it = reserved_words.find(s);
    if (it == reserved_words.end())
    {
        return nullptr;
    }
    return it->second; 
}

const TokenType* TokenType::special_symbol_type(const std::string& key)
{
    std::string s(key);
    std::transform(key.begin(), key.end(), s.begin(), ::tolower);
    auto it = special_symbols.find(s);
    if (it == special_symbols.end())
    {
        return nullptr;
    }
    return it->second;
}

#define DEFINE_RESERVED_TOKENTYPE(name) \
    const TokenType name { #name, TokenSubtype::Reserved };

namespace psc { namespace pascal { 
#define X DEFINE_RESERVED_TOKENTYPE
    RESERVED
#undef X
}}

#define DEFINE_SPECIAL_TOKENTYPE(name, sym) \
    const TokenType name { sym, TokenSubtype::Special };

namespace psc { namespace pascal { 
#define X DEFINE_SPECIAL_TOKENTYPE
    SPECIAL
#undef X
}}

#define DEFINE_OTHER_TOKENTYPE(name) \
    const TokenType name { #name, TokenSubtype::Other };

namespace psc { namespace pascal { 
#define X DEFINE_OTHER_TOKENTYPE
    OTHER
#undef X
}}

