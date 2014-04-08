#ifndef _PSC_IM_COMMON_H_
#define _PSC_IM_COMMON_H_

#include <boost/variant.hpp>
#include <string>
#include <sstream>
#include "im/symbol_table.h"

namespace psc { namespace im {
    
using ICodeNodeAttrValue = boost::variant<int, double, std::string, const SymbolTableEntry *>;
   
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

	std::string operator()(const SymbolTableEntry *p) const
	{
		return p->name();
	}
};

}
}

#endif // _PSC_IM_COMMON_H_
