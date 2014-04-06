#include "be/backend.h"

using namespace psc::im;
using namespace psc::be;
using std::unique_ptr;
using std::string;

std::unique_ptr<Backend> Backend::create(const string &type)
{
	return nullptr;
}