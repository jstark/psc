#ifndef _PSC_IM_ICODE_H_
#define _PSC_IM_ICODE_H_

#include <memory>
#include <vector>
#include <functional>
#include <boost/optional.hpp>
#include "im/common.h"

namespace psc {
namespace im {

#define ICODENODETYPES	\
	X(PROGRAM)			\
	X(PROCEDURE)		\
	X(FUNCTION)			\
	X(COMPOUND)			\
	X(ASSIGN)			\
	X(LOOP)				\
	X(TEST)				\
	X(CALL)				\
	X(PARAMETERS)		\
	X(IF)				\
	X(SELECT)			\
	X(SELECT_BRANCH)	\
	X(SELECT_CONSTANTS)	\
	X(NO_OP)			\
	X(EQ)				\
	X(NE)				\
	X(LT)				\
	X(LE)				\
	X(GT)				\
	X(GE)				\
	X(NOT)				\
	X(ADD)				\
	X(SUBTRACT)			\
	X(OR)				\
	X(NEGATE)			\
	X(MULTIPLY)			\
	X(INTEGER_DIVIDE)	\
	X(FLOAT_DIVIDE)		\
	X(MOD)				\
	X(AND)				\
	X(VARIABLE)			\
	X(SUBSCRIPTS)		\
	X(FIELD)			\
	X(INTEGER_CONSTANT)	\
	X(REAL_CONSTANT)	\
	X(STRING_CONSTANT)	\
	X(BOOLEAN_CONSTANT)

#define X(a) \
	a,
    
enum class ICodeNodeType
{
    ICODENODETYPES
};

#undef X

extern const char *ICodeNodeTypeNames[];
    
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
    ICodeNode* root() const;
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
    ICodeNode* parent() const;
    
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
    void foreach_attribute(std::function<void(ICodeKey, ICodeNodeAttrValue)> f) const;

	/*
	 * Copy a node;
	 */
	std::unique_ptr<ICodeNode> copy() const;
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
