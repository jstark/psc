#ifndef _PSC_IM_ICODE_H_
#define _PSC_IM_ICODE_H_

#include <memory>
#include <vector>
#include <functional>
#include <boost/optional.hpp>
#include "im/common.h"

namespace psc {
namespace im {
    
enum class ICodeNodeType
{
    // Program structure
    PROGRAM, PROCEDURE, FUNCTION,
    // Statements
    COMPOUND, ASSIGN, LOOP, TEST, CALL, PARAMETERS,
    IF, SELECT, SELECT_BRANCH, SELECT_CONSTANTS, NO_OP,
    // Relational operators
    EQ, NE, LT, LE, GT, GE, NOT,
    // Additive operators
    ADD, SUBTRACT, OR, NEGATE,
    // Multiplicative operators
    MULTIPLY, INTEGER_DIVIDE, FLOAT_DIVIDE, MOD, AND,
    // Operands
    VARIABLE, SUBSCRIPTS, FIELD,
    INTEGER_CONSTANT, REAL_CONSTANT,
    STRING_CONSTANT, BOOLEAN_CONSTANT,
};
    
enum class ICodeKey
{
    LINE, ID, VALUE
};

class ICodeNode;

class ICode final
{
public:
    ICode();
    ~ICode();
    ICodeNode* setRoot(std::unique_ptr<ICodeNode> node);
    ICodeNode* root();
private:
    struct ICodeImpl;
    std::unique_ptr<ICodeImpl> pimpl;
};
    
class ICodeNode final
{
public:
    explicit ICodeNode(ICodeNodeType type);
    ~ICodeNode();
    
    /*
     * Get the node's type.
     */
    ICodeNodeType type() const;
    
    /*
     * Get the node's parent. Do not delete this
     * pointer.
     */
    ICodeNode* parent();
    
    /*
     * Add a child node and return that node.
     */
    ICodeNode* add_child(std::unique_ptr<ICodeNode> node);
    
    /*
     * Get the node's children.
     */
    std::vector<ICodeNode *> children() const;
    
    /*
     * Set an attribute for this node.
     */
    void set_attribute(ICodeKey key, ICodeNodeAttrValue value);
    
    /*
     * Get an attribute for a given key.
     */
    boost::optional<ICodeNodeAttrValue> attribute(ICodeKey key) const;
    
    /*
     * Run a function f for each attribute in this node.
     */
    void foreach_attribute(std::function<void(ICodeKey, ICodeNodeAttrValue)> f);
private:
    struct ICodeNodeImpl;
    std::unique_ptr<ICodeNodeImpl> pimpl;
};
    
class ICodeFactory final
{
public:
    ICodeFactory() = delete;
    
    static std::unique_ptr<ICode> create_icode();
    static std::unique_ptr<ICodeNode> create_node(ICodeNodeType type);
};

}
}

#endif // _PSC_IM_ICODE_H_
