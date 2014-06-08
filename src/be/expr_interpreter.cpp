#include "be/expr_interpreter.h"
#include "im/icode.h"
#include "be/rt_error.h"
#include "be/rt_error_handler.h"
#include <set>

using namespace psc::im;
using namespace psc::be;

namespace
{
    std::set<ICodeNodeType> ARITH_OPS = {
        ICodeNodeType::ADD,
        ICodeNodeType::SUBTRACT,
        ICodeNodeType::MULTIPLY,
        ICodeNodeType::FLOAT_DIVIDE,
        ICodeNodeType::INTEGER_DIVIDE
    };


}

ExprVal ExprInterpreter::execute(const ICodeNode &node, int *exec_count)
{
    auto type = node.type();
    if (type == ICodeNodeType::VARIABLE)
    {
        // get the variable's symbol table entry and return its value.
        auto opt_entry = node.attribute(ICodeKey::ID);
        // var_entry is a ICodeNodeAttrValue
        // and we must take the SymbolTableEntry from it.
        auto var_entry = opt_entry.get();
        auto entry = boost::get<SymbolTableEntry *>(var_entry);
        auto entry_val = entry->attribute(SymbolTableKey::DataValue);
        // we must convert var_entry to ExprVal
        return entry_val;
    } else if (type == ICodeNodeType::INTEGER_CONSTANT)
    {
		auto val_attr = node.attribute(ICodeKey::VALUE);
		auto variant = val_attr.get();
        // return integer value;
		return boost::get<int>(variant);
    } else if (type == ICodeNodeType::REAL_CONSTANT)
    {
		auto val_attr = node.attribute(ICodeKey::VALUE);
		auto variant = val_attr.get();
        // return the real value:
        return boost::get<double>(variant);
    } else if (type == ICodeNodeType::STRING_CONSTANT)
    {
		auto val_attr = node.attribute(ICodeKey::VALUE);
		auto variant = val_attr.get();
        // return the string value:
		return boost::get<std::string>(variant);
    } else if (type == ICodeNodeType::NEGATE)
    {
        // get the NEGATE node's expression node child
        auto children = node.children();
        auto expression = children.at(0);

        // execute the expression and return the negative of its value
        auto value = execute(*expression, exec_count);
        if (value.which() == 1)
        {
            return -boost::get<int>(value);
        } else if (value.which() == 2)
        {
            return -boost::get<double>(value);
        } else
        {
            return nullptr;
        }
    } else if (type == ICodeNodeType::NOT)
    {
        // get the NOT node's expression node child
        auto children = node.children();
        auto expression = children.at(0);
        auto val = execute(*expression, exec_count);

        // execute the expression and return the "NOT" of its value.
        bool value = boost::get<bool>(val);
        return !value;

    } else
    {
		return execute_binop(node, exec_count);
    }
    return 0;
}

ExprVal ExprInterpreter::execute_binop(const ICodeNode &node, int *exec_count)
{
    // get the two operand children of the operator node.
    auto children = node.children();
    auto operand_node1 = children.at(0);
    auto operand_node2 = children.at(1);

    // operands
    auto operand_val1 = execute(*operand_node1, exec_count);
    auto operand_val2 = execute(*operand_node2, exec_count);

    bool integer_mode = (operand_val1.which() == 1) && (operand_val2.which() == 1);

    // ====================
    // Arithmetic operators
    // ====================
    auto type = node.type();
	if (ARITH_OPS.count(type))
	{
		if (integer_mode)
		{
			int value1 = boost::get<int>(operand_val1);
			int value2 = boost::get<int>(operand_val2);

			// integer operations
			// SHOULD RETURN AT EVERY IF
			if (type == ICodeNodeType::ADD)      return value1 + value2;
			if (type == ICodeNodeType::SUBTRACT) return value1 - value2;
			if (type == ICodeNodeType::MULTIPLY) return value1 * value2;
			if (type == ICodeNodeType::FLOAT_DIVIDE)
			{
				// check for division by zero:
				if (value2 != 0)
				{
					return (double) value1 / (double) value2;
				}
				else
				{
					RuntimeErrorHandler::flag(node, DIVISION_BY_ZERO, _mp);
					return 0;
				}
			}
			if (type == ICodeNodeType::INTEGER_DIVIDE)
			{
				// check for division by zero:
				if (value2 != 0)
				{
					return value1 / value2;
				}
				else
				{
					RuntimeErrorHandler::flag(node, DIVISION_BY_ZERO, _mp);
					return 0;
				}
			}
			if (type == ICodeNodeType::MOD)
			{
				// check for division by zero
				if (value2 != 0)
				{
					return value1 % value2;
				}
				else
				{
					RuntimeErrorHandler::flag(node, DIVISION_BY_ZERO, _mp);
					return 0;
				}
			}
		}
		else // double mode
		{
			double value1 = operand_val1.which() == 1 ? boost::get<int>(operand_val1) : boost::get<double>(operand_val1);
			double value2 = operand_val2.which() == 1 ? boost::get<int>(operand_val2) : boost::get<double>(operand_val2);

			// double operations
			// SHOULD RETURN AT EVERY IF
			switch (type)
			{
			case ICodeNodeType::ADD: return value1 + value2;
			case ICodeNodeType::SUBTRACT: return value1 - value2;
			case ICodeNodeType::MULTIPLY: return value1 * value2;
			case ICodeNodeType::FLOAT_DIVIDE:
			{
				// check for division by zero:
				if (value2 != 0.0)
				{
					return value1 / value2;
				}
				else
				{
					RuntimeErrorHandler::flag(node, DIVISION_BY_ZERO, _mp);
					return 0.0;
				}
			}
                break;
            default:
                std::abort();
			}
		}
	} else if (node.type() == ICodeNodeType::AND || node.type() == ICodeNodeType::OR)
	{
		bool value1 = boost::get<bool>(operand_val1);
		bool value2 = boost::get<bool>(operand_val2);

		switch (node.type())
		{
		case ICodeNodeType::AND:
			return value1 && value2;
		case ICodeNodeType::OR:
			return value1 || value2;
        default:
            std::abort();
		}
    } else if (integer_mode)
    {
        int value1 = boost::get<int>(operand_val1);
        int value2 = boost::get<int>(operand_val2);

        switch (node.type())
        {
        case ICodeNodeType::EQ: return value1 == value2;
        case ICodeNodeType::NE: return value1 != value2;
        case ICodeNodeType::LT: return value1 <  value2;
        case ICodeNodeType::LE: return value1 <= value2;
        case ICodeNodeType::GT: return value1 >  value2;
        case ICodeNodeType::GE: return value1 >= value2;
        default: std::abort();
        }
    } else
    {
		double value1 = operand_val1.which() == 1 ? boost::get<int>(operand_val1) : boost::get<double>(operand_val1);
		double value2 = operand_val2.which() == 1 ? boost::get<int>(operand_val2) : boost::get<double>(operand_val2);

        switch (type)
        {
        case ICodeNodeType::EQ: return value1 == value2;
        case ICodeNodeType::NE: return value1 != value2;
        case ICodeNodeType::LT: return value1 <  value2;
        case ICodeNodeType::LE: return value1 <= value2;
        case ICodeNodeType::GT: return value1 >  value2;
        case ICodeNodeType::GE: return value1 >= value2;
        default:
            std::abort();
        }
    }
    return 0; // should never get here.
}
