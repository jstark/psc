#ifndef _PSC_FE_LISTENERS_H_
#define _PSC_FE_LISTENERS_H_

#include "msg/message.h"

namespace psc { namespace fe {

class SourceMessageListener : public msg::MessageListener
{
public:
    void receive_msg(const msg::Message &message) override;
};

class ParserMessageListener : public msg::MessageListener
{
public:
    void receive_msg(const msg::Message &message) override;
};

}}

#endif // _PSC_FE_LISTENERS_H_ 
