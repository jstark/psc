#ifndef _PSC_FE_SOURCE_H_
#define _PSC_FE_SOURCE_H_

#include <fstream>
#include <memory>

#include "msg/message.h"

namespace psc { namespace fe {

class Source final : private msg::MessageProducer
{
public:
    explicit Source(std::ifstream &&reader);
    ~Source();
    Source(Source &&rhs);

    char current();
    char next();
    char peek();

    int current_line() const;
    int current_pos() const;

    static constexpr char END_OF_FILE = '\0';
    static constexpr char END_OF_LINE = '\n';

    // message interface
    using msg::MessageProducer::add;
    using msg::MessageProducer::send_msg;
private:
    struct SourceImpl;
    std::unique_ptr<SourceImpl> _pimpl;
};

}}

#endif // _PSC_FE_SOURCE_H_ 
