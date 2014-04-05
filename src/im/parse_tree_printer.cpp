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
    std::string indent = std::string(INDENT_WIDTH, ' ');         // indent spaces
    std::string indentation;    // indentation of a line
    std::string line;           // output line
    
    void print_node(const ICodeNode *node)
    {
		static const std::string lt("<");
		static const std::string gt(">");
		static const std::string close("</");

		const char *node_name = ICodeNodeTypeNames[static_cast<int>(node->type())];
        // opening tag
		append(indentation); append(lt + node_name);
        
        print_attributes(node);
        print_type_spec(node);

		auto children = node->children();
		if (!children.empty())
		{
			append(gt);
			print_line();
			print_child_nodes(children);
			append(indentation);
			append(close + node_name + gt);
		}
		else
		{
			append(" "); append("/>");
		}
		print_line();
	}

	std::string key_to_str(ICodeKey key) const
	{
		if (key == ICodeKey::ID)
		{
			return "ID";
		}
		else if (key == ICodeKey::LINE)
		{
			return "LINE";
		}
		else if (key == ICodeKey::VALUE)
		{
			return "VALUE";
		}
		return "";
	}

	void print_child_nodes(const std::vector<ICodeNode *> &children)
	{
		std::string save = indentation;
		indentation += indent;

		for (const auto & ch : children)
		{
			print_node(ch);
		}

		indentation = save;
	}
    
    void print_attributes(const ICodeNode *node)
    {
        using std::placeholders::_1;
        using std::placeholders::_2;
        
        std::string save_indendation = indentation;
        
		auto f = [=](ICodeKey key, ICodeNodeAttrValue val){ print_attribute(key_to_str(key), val); };
		node->foreach_attribute(f);
        
        indentation = save_indendation;
    }
    
    void print_attribute(const std::string &key, ICodeNodeAttrValue value)
    {
        // if the value is a symbol table entry, use the identifier's name.
		// else just use the value string
		std::string val = boost::apply_visitor(psc::im::to_str_visitor(), value);
		std::string key_lower = key;
		std::transform(key.begin(), key.end(), key_lower.begin(), ::tolower);
		std::string text = key_lower + "=\"" + val + "\"";
        append(" "); append(text);
        
		if (value.which() == 3)
		{
			auto entry = boost::get<const SymbolTableEntry *>(value);
			print_attribute("LEVEL", entry->symbol_table()->nesting_level());
		}
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
        if (!(line_break && text == " "))
        {
            line.append(text);
            length += text_length;
        }
    }
    
    void print_type_spec(const ICodeNode *node)
    {
        
    }
};

////////////////////////////////////////////////////////////////////////////////
ParseTreePrinter::ParseTreePrinter(std::ostream &stream)
: pimpl(new ParseTreePrinter::ParseTreePrinterImpl(stream)) {}

ParseTreePrinter::~ParseTreePrinter() {}

void ParseTreePrinter::print(const ICode *icode)
{
    pimpl->stream << "\n===== INTERMEDIATE CODE =====\n" << std::endl;
    pimpl->print_node(icode->root());
    pimpl->print_line();
}

