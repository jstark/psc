#ifndef _PSC_UTIL_VAR_H_
#define _PSC_UTIL_VAR_H_

#include <boost/variant.hpp>
#include <boost/variant/static_visitor.hpp>
#include <string>
#include <sstream>

namespace psc { namespace utils {

using var = boost::variant<int, double, std::string, const void *>;
    
class to_str_visitor : public boost::static_visitor<std::string>
{
public:
    std::string operator()(int i) const
    {
        return std::to_string(i);
    }
    
    std::string operator()(double d) const 
    {
        std::ostringstream out;
        out << d;
        return out.str();
    }
    
    std::string operator()(const std::string& s) const 
    {
        return s;
    }
    
    std::string operator()(const void *p) const
    {
        return "__ptr__";
    }
};

}}

#endif // _PSC_UTIL_VAR_H_ 

