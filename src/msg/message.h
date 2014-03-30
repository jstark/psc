#ifndef _PSC_MSG_MESSAGE_H_
#define _PSC_MSG_MESSAGE_H_

#include "utils/var.h"
#include <vector>
#include <memory>

namespace psc { namespace msg {

enum class MessageType
{
    SourceLine, SyntaxError,
    ParserSummary, InterpreterSummary, CompilerSummary,
    Miscellaneous, Token,
    Assign, Fetch, Breakpoint, RuntimeError,
    Call, Return
};

class Message final
{
public:
    explicit Message(MessageType t, std::vector<utils::var> args)
        : _type(t), _args(args) {}

    MessageType type() const { return _type; }
    std::vector<utils::var> args() const { return _args; }
private:
    MessageType _type;
    std::vector<utils::var> _args;
};

class MessageListener
{
public:
    virtual ~MessageListener() {}
    virtual void receive_msg(const Message& msg) = 0;
};

using std::shared_ptr;
using std::weak_ptr;
using std::vector;

class MessageProducer
{
public:
    void add(MessageListener* listener)
    {
        _listeners.push_back(listener);
    }

    void send_msg(const Message& msg) const
    {
        for (auto ml : _listeners)
        {
            ml->receive_msg(msg);
        }
    }

private:
    vector<MessageListener*> _listeners;
};

}}

#endif // _PSC_MSG_MESSAGE_H_
