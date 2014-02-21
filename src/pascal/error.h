#ifndef _PSC_PASCAL_ERRORS_H_
#define _PSC_PASCAL_ERRORS_H_

#include <string>

namespace psc { namespace pascal {

class ErrorCode
{
public:
    explicit ErrorCode(const std::string &msg);
    ErrorCode(int code_id, const std::string &msg);
    
    std::string text() const { return _message; }
private:
    int _id;
    std::string _message;
};

#define CODES \
    X(ALREADY_FORWARDED, "Already specified in FORWARD")    \
    X(IDENTIFIER_REDEFINED, "Redefined identifier")         \
    X(IDENTIFIER_UNDEFINED, "Undefined identifier")         \
    X(INCOMPATIBLE_ASSIGNMENT, "Incompatible assignment")   \
    X(INCOMPATIBLE_TYPES, "Incompatible types")             \
    X(INVALID_ASSIGNMENT, "Invalid assignment statement")   \
    X(INVALID_CHARACTER, "Invalid character")               \
    X(INVALID_CONSTANT, "Invalid constant")                 \
    X(INVALID_EXPONENT, "Invalid exponent")                 \
    X(INVALID_EXPRESSION, "Invalid expression")             \
    X(INVALID_FIELD, "Invalid field")                       \
    X(INVALID_FRACTION, "Invalid fraction")                 \
    X(INVALID_IDENTIFIER_USAGE, "Invalid index type")       \
    X(INVALID_NUMBER, "Invalid number")                     \
    X(INVALID_STATEMENT, "Invalid statement")               \
    X(INVALID_SUBRANGE_TYPE, "Invalid subrange type")       \
    X(INVALID_TARGET, "Invalid assignment target")          \
    X(INVALID_TYPE, "Invalid type")                         \
    X(INVALID_VAR_PARM, "Invalid VAR parameter")            \
    X(MIN_GT_MAX, "Min limit greater than max limit")       \
    X(MISSING_BEGIN, "Missing BEGIN")                       \
    X(MISSING_COLON, "Missing :")                           \
    X(MISSING_COLON_EQUALS, "Missing :=")                   \
    X(MISSING_COMMA, "Missing ,")                           \
    X(MISSING_CONSTANT, "Missing constant")                 \
    X(MISSING_DO, "Missing DO")                             \
    X(MISSING_DOT_DOT, "Missing ..")                        \
    X(MISSING_END, "Missing END")                           \
    X(MISSING_EQUALS, "Missing =")                          \
    X(MISSING_FOR_CONTROL, "Invalid FOR control variable")  \
    X(MISSING_IDENTIFIER, "Missing identifier")             \
    X(MISSING_LEFT_BRACKET, "Missing [")                    \
    X(MISSING_OF, "Missing OF")                             \
    X(MISSING_PERIOD, "Missing .")                          \
    X(MISSING_PROGRAM, "Missing PROGRAM")                   \
    X(MISSING_RIGHT_BRACKET, "Missing ]")                   \
    X(MISSING_RIGHT_PAREN, "Missing )")                     \
    X(MISSING_SEMICOLON, "Missing ;")                       \
    X(MISSING_THEN, "Missing THEN")                         \
    X(MISSING_TO_DOWN_TO, "Missing TO or DOWNTO")           \
    X(MISSING_UNTIL, "Missing UNTIL")                       \
    X(MISSING_VARIABLE, "Missing variable")                 \
    X(CASE_CONSTANT_REUSED, "CASE constant reused")         \
    X(NOT_CONSTANT_IDENTIFIER, "Not a constant identifier") \
    X(NOT_RECORD_VARIABLE, "Not a record variable")         \
    X(NOT_TYPE_IDENTIFIER, "Not a type identifier")         \
    X(RANGE_INTEGER, "Integer literal out of range")        \
    X(RANGE_REAL, "Real literal out of range")              \
    X(STACK_OVERFLOW, "Stack overflow")                     \
    X(TOO_MANY_LEVELS, "Nesting level too deep")            \
    X(TOO_MANY_SUBSCRIPTS, "Too many subscripts")           \
    X(UNEXPECTED_EOF, "Unexpected end of file")             \
    X(UNEXPECTED_TOKEN, "Unexpected token")                 \
    X(UNIMPLEMENTED, "Unimplemented feature")               \
    X(UNRECOGNIZABLE, "Unrecognizable input")               \
    X(WRONG_NUMBER_OF_PARMS, "Wrong number of actual parameters")

#define DECLARE_ERROR_CODE(var, msg) \
    extern const ErrorCode var;

#define X DECLARE_ERROR_CODE
CODES
#undef X

#define FATAL_CODES \
    X(IO_ERROR, -101, "Object I/O error") \
    X(TOO_MANY_ERRORS, -102, "Too many errors")

#define DECLARE_FATAL_ERROR_CODE(var, code, msg) \
    DECLARE_ERROR_CODE(var, msg)

#define X DECLARE_FATAL_ERROR_CODE
FATAL_CODES
#undef X

}}

#endif // _PSC_PASCAL_ERRORS_H_

