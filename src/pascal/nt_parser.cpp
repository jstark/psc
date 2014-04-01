#include "pascal/nt_parser.h"

#include "fe/token.h"
#include "msg/message.h"
#include "pascal/scanner.h"
#include "pascal/error_handler.h"
#include "pascal/error.h"

using namespace psc::fe;
using namespace psc::pascal;

Token NonTerminalParser::synchronize(const SynchronizationSet &s)
{
	auto tok = _scanner.current();

	// if the current token is not in the synchronization set,
	// then it is unexpected and the parser must recover.
	if (!s.count(tok.type()))
	{
		// flag the unexpected token
		ErrorHandler::flag(tok, &UNEXPECTED_TOKEN, _mp);

		// recover by skipping tokens that are not in 
		// the synchronization set.
		do
		{
			tok = _scanner.next();
		} while (!tok.is_eof() && !s.count(tok.type()));
	}
	return tok;
}