#ifndef _PSC_FE_SOURCE_H_
#define _PSC_FE_SOURCE_H_

#include <fstream>
#include <memory>
#include "utils/fwd.h"

FWD_DECL_NS_NS_CL(psc, msg, MessageProducer)

namespace psc { namespace fe {

class Source final
{
public:
    explicit Source(std::ifstream &&reader, const psc::msg::MessageProducer &mp);
    ~Source();
    Source(Source &&rhs);

    char current();
    char next();
    char peek();

    int current_line() const;
    int current_pos() const;

    static constexpr char END_OF_FILE = '\0';
    static constexpr char END_OF_LINE = '\n';

private:
    struct SourceImpl;
    std::unique_ptr<SourceImpl> _pimpl;
};

}}

#endif // _PSC_FE_SOURCE_H_ 
