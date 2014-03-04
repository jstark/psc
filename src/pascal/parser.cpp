#include "fe/token.h"
#include "im/symbol_table.h"
#include "msg/message.h"
#include "pascal/parser.h"
#include "pascal/error.h"
#include "pascal/token_type.h"
#include <chrono>
#include <boost/variant.hpp>
#include <boost/optional.hpp>

using namespace psc;
using namespace psc::msg;
using namespace psc::pascal;

Parser::Parser(pascal::Scanner &&scanner)
    : fe::Parser<pascal::Scanner>(std::move(scanner)) {}

std::tuple<im::ICode*, std::unique_ptr<im::SymbolTableStack>> Parser::parse()
{
    fe::Token token;
    auto start = std::chrono::system_clock::now();
	auto symtabStack = im::SymbolTableFactory::make_stack();
    try
    {
        do
        {
            token = next_token();
            auto type = (const pascal::TokenType *)token.type();
            if (type == &IDENTIFIER)
            {
				std::string name = token.lexeme();
				std::transform(name.begin(), name.end(), name.begin(), ::tolower);

				// If it's not already in the symbol table,
				// create and anter a new entry for the identifier.
				auto opt_entry = symtabStack->lookup(name);
				im::SymbolTableEntry *entry = opt_entry.get_value_or(nullptr);
				if (!entry)
				{
					entry = symtabStack->enter_local(name);
				}
				entry->append_line(token.line_number());
            } else if (type == &ERROR)
            {
                _errorHandler.flag(token, static_cast<const ErrorCode *>(boost::get<const void *>(token.value())), this);
            }
        } while (!token.is_eof());
        
        auto end = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed = end - start;
        
        Message msg(MessageType::ParserSummary, {token.line_number(), error_count(), elapsed.count()});
        send_msg(msg);
    } catch (...)
    {
        _errorHandler.abort_translation(&IO_ERROR, this);
    }
	return std::make_tuple(nullptr, std::move(symtabStack));
}

int Parser::error_count() const
{
    return _errorHandler.error_count();
}

