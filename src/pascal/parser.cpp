#include "fe/token.h"
#include "im/symbol_table.h"
#include "im/icode.h"
#include "msg/message.h"
#include "pascal/parser.h"
#include "pascal/error.h"
#include "pascal/token_type.h"
#include "pascal/stmnt_parser.h"
#include <chrono>
#include <boost/variant.hpp>
#include <boost/optional.hpp>

using namespace psc;
using namespace psc::im;
using namespace psc::msg;
using namespace psc::pascal;

Parser::Parser(pascal::Scanner &&scanner, msg::MessageProducer &mp)
    : fe::Parser<pascal::Scanner>(std::move(scanner), mp) {}

std::tuple<std::unique_ptr<im::ICode>, std::unique_ptr<im::SymbolTableStack>> Parser::parse()
{
    fe::Token token;
    auto start = std::chrono::system_clock::now();
	auto symtabstack = im::SymbolTableFactory::make_stack();
	auto icode = ICodeFactory::create_icode();
    try
    {
		std::unique_ptr<ICodeNode> root = nullptr; 
		token = next_token();
		auto type = (const pascal::TokenType *)token.type();
		if (type == &BEGIN)
		{
			StatementParser stmnt_parser{ _scanner, *symtabstack, _mp };
			root = stmnt_parser.parse(token);
			token = _scanner.current();
		}
		else
		{
			ErrorHandler::flag(token, &UNEXPECTED_TOKEN, _mp);
		}
        
		// look for the final period.
		if (token.type() != &DOT)
		{
			ErrorHandler::flag(token, &MISSING_PERIOD, _mp);
		}

		token = _scanner.current();
		if (root)
		{
			icode->setRoot(std::move(root));
		}

        auto end = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed = end - start;
        
        Message msg(MessageType::ParserSummary, {token.line_number(), error_count(), elapsed.count()});
        _mp.send_msg(msg);
    } catch (...)
    {
        ErrorHandler::abort_translation(&IO_ERROR, _mp);
    }
	return std::make_tuple(std::move(icode), std::move(symtabstack));
}

int Parser::error_count() const
{
    return ErrorHandler::error_count();
}

