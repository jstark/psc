#ifndef _PSC_BE_RT_ERROR_H_INCLUDED_
#define _PSC_BE_RT_ERROR_H_INCLUDED_

#include <string>

namespace psc {
namespace be {
	
class RuntimeErrorCode
{
public:
	explicit RuntimeErrorCode(const std::string &msg);

	std::string text() const { return _message; }
private:
	std::string _message;
};

#define RTCODES \
	X(UNITIALIZED_VALUE, "Uninitialized value")									\
	X(VALUE_RANGE, "Value out of range")										\
	X(INVALID_CASE_EXPRESSION_VALUE, "Invalid CASE expression value")			\
	X(DIVISION_BY_ZERO, "Division by zero")										\
	X(INVALID_STANDARD_FUNCTION_ARGUMENT, "Invalid standard function argument") \
	X(INVALID_INPUT, "Invalid input")											\
	X(STACK_OVERFLOW, "Runtime stack overflow")									\
	X(UNIMPLEMENTED_FEATURE, "Unimplemented runtime feature") 

#define DECLARE_ERROR_CODE(var, msg) \
	extern const RuntimeErrorCode var;

#define X DECLARE_ERROR_CODE
	RTCODES
#undef X

}
}

#endif _PSC_BE_RT_ERROR_H_INCLUDED_