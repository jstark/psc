#include "im/parse_tree_printer.h"
#include "im/icode.h"
#include <string>
#include <functional>

using namespace psc::im;

namespace
{
    constexpr int INDENT_WIDTH = 4;
    constexpr int LINE_WIDTH = 80;
}

////////////////////////////////////////////////////////////////////////////////
struct ParseTreePrinter::ParseTreePrinterImpl
{
    ParseTreePrinterImpl(std::ostream &str)
    : stream(str) {}
    
    std::ostream &stream;       // output print stream
    int length{0};              // output line length
    std::string indent{INDENT_WIDTH, ' '};         // indent spaces
    std::string indentation;    // indentation of a line
    std::string line;           // output line
    
    void print_node(ICodeNode *node)
    {
        // opening tag
        append(indentation);
        
        print_attributes(node);
        print_type_spec(node);
    }
    
    void print_attributes(ICodeNode *node)
    {
        using std::placeholders::_1;
        using std::placeholders::_2;
        
        std::string save_indendation = indentation;
        
        node->foreach_attribute(
            std::bind(&ParseTreePrinterImpl::print_attribute, this, _1, _2));
        
        indentation = save_indendation;
    }
    
    void print_attribute(ICodeKey key, ICodeNodeAttrValue value)
    {
        // TODO
    }
    
    void print_line()
    {
        if (length > 0)
        {
            stream << line << std::endl;
            line.clear();
            length = 0;
        }
    }
    
    void append(const std::string &text)
    {
        std::string::size_type text_length = text.size();
        bool line_break = false;
        
        // Wrap lines
        if (length + text_length > LINE_WIDTH)
        {
            print_line();
            line.append(indentation);
            length = indentation.length();
            line_break = true;
        }
        
        // append
        if (!line_break && text == " ")
        {
            line.append(text);
            length += text_length;
        }
    }
    
    void print_type_spec(ICodeNode *node)
    {
        
    }
};

////////////////////////////////////////////////////////////////////////////////
ParseTreePrinter::ParseTreePrinter(std::ostream &stream)
: pimpl(new ParseTreePrinter::ParseTreePrinterImpl(stream)) {}

ParseTreePrinter::~ParseTreePrinter() {}

void ParseTreePrinter::print(ICode *icode)
{
    pimpl->stream << "\n===== INTERMEDIATE CODE =====\n" << std::endl;
    pimpl->print_node(icode->root());
    pimpl->print_line();
}

