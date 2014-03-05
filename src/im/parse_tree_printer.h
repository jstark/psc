#ifndef PSC_IM_PARSETREEPRINTER_H_
#define PSC_IM_PARSETREEPRINTER_H_

#include <ostream>

namespace psc { namespace im {
 
class ICode;

class ParseTreePrinter final
{
public:
    explicit ParseTreePrinter(std::ostream &stream);
    ~ParseTreePrinter();
    void print(ICode *icode);
    
private:
    struct ParseTreePrinterImpl;
    std::unique_ptr<ParseTreePrinterImpl> pimpl;
};
    
}
}

#endif // PSC_IM_PARSETREEPRINTER_H_