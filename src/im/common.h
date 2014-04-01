#ifndef _PSC_IM_COMMON_H_
#define _PSC_IM_COMMON_H_

#include <boost/variant.hpp>

namespace psc { namespace im {
 
class SymbolTableEntry;
    
using ICodeNodeAttrValue = boost::variant<int, const SymbolTableEntry *>;
    
}
}

#endif // _PSC_IM_COMMON_H_