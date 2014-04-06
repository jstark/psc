#include "be/backend.h"
#include "be/interpreter.h"

using namespace psc::im;
using namespace psc::be;
using std::unique_ptr;
using std::string;

unique_ptr<Backend> Backend::create(const string &type, psc::msg::MessageProducer &mp)
{
	unique_ptr<Interpreter> interp(new Interpreter(mp));
	return std::move(interp);

