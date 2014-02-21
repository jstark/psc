#include "fe/source.h"
#include "msg/message.h"
#include <boost/any.hpp>
#include <vector>
#include <string>

using namespace psc::fe;

struct Source::SourceImpl final
{
    SourceImpl(std::ifstream &&reader, const Source &back)
        : _reader(std::move(reader)), _back(back) {}

    ~SourceImpl() {}

    std::ifstream _reader;
    std::string _line;
    int _lnum{0};
    int _cpos{-2};

    void read_line();
    char peek_char();
    char next_char();
    char current_char();

    const Source &_back;
};

void Source::SourceImpl::read_line()
{
    using namespace msg;

    std::getline(_reader, _line);
    _cpos = -1;
    if (!_reader.eof())
    {
        ++_lnum;
        Message msg(MessageType::SourceLine, {_lnum, _line});
        _back.send_msg(msg);
    }
}

char Source::SourceImpl::next_char()
{
    ++_cpos;
    return current_char();
}

char Source::SourceImpl::current_char()
{
    if (_cpos == -2 || _cpos > static_cast<int>(_line.size()))
    {
        read_line();
        return next_char();
    } else if (_reader.eof())
    {
        return END_OF_FILE;
    } else if (_cpos == -1 || _cpos == _line.size())
    {
        return END_OF_LINE;
    } else 
    {
        return _line[_cpos];
    }
}

char Source::SourceImpl::peek_char()
{
    current_char(); 
    if (_reader.eof())
    {
        return END_OF_FILE;
    }
    int npos = _cpos + 1;
    return npos < _line.size() ? _line[npos] : END_OF_LINE;
}

// public interface

Source::Source(std::ifstream && reader)
    : _pimpl(new SourceImpl(std::move(reader), *this)) {}

Source::~Source() {}

Source::Source(Source &&source)
    : _pimpl(std::move(source._pimpl)) {}

int Source::current_line() const
{
    return _pimpl->_lnum;
}

int Source::current_pos() const
{
    return _pimpl->_cpos;
}

char Source::peek()
{
    return _pimpl->peek_char();
}

char Source::next()
{
    return _pimpl->next_char();
}

char Source::current()
{
    return _pimpl->current_char();
}

