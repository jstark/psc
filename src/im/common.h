#ifndef _PSC_IM_COMMON_H_
#define _PSC_IM_COMMON_H_

#include <boost/variant.hpp>
#include <string>

namespace psc { namespace im {
 
class SymbolTableEntry;
    
using ICodeNodeAttrValue = boost::variant<int, double, std::string, const SymbolTableEntry *>;
    
}
}

#endif // _PSC_IM_COMMON_H_
