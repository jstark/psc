#include "be/interpreter.h"
#include "im/icode.h"
#include "im/symbol_table.h"
#include "msg/message.h"
#include <chrono>

using namespace psc::im;
using namespace psc::be;
using namespace psc::msg;
using std::unique_ptr;

void Interpreter::process(unique_ptr<ICode> iCode, unique_ptr<SymbolTableStack> symtab)
{
	int exec_count = 0;
	int errors = 0;
	auto start = std::chrono::system_clock::now();

	// TODO:

	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed = end - start;

	Message msg(MessageType::InterpreterSummary, { exec_count, errors, elapsed.count() });
	_mp.send_msg(msg);
}
