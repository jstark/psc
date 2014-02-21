#ifndef _PSC_MSG_MESSAGE_H_
#define _PSC_MSG_MESSAGE_H_

#include <boost/any.hpp>
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
    explicit Message(MessageType t, std::vector<boost::any> args)
        : _type(t), _args(args) {}

    MessageType type() const { return _type; }
    std::vector<boost::any> args() const { return _args; }
private:
    MessageType _type;
    std::vector<boost::any> _args;
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
    void add(shared_ptr<MessageListener> listener)
    {
        _listeners.emplace_back(listener);
    }

    void send_msg(const Message& msg) const
    {
        for (const auto &ml : _listeners)
        {
            auto locked_ml = ml.lock();
            if (locked_ml)
            {
                locked_ml->receive_msg(msg);
            }
        }
    }

private:
    vector<weak_ptr<MessageListener>> _listeners;
};

}}

#endif // _PSC_MSG_MESSAGE_H_
