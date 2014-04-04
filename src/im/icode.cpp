#include "im/icode.h"
#include <map>

using namespace psc::im;

#define X(a) \
#a,

const char *psc::im::ICodeNodeTypeNames[] = {
	ICODENODETYPES
};

#undef X

////////////////////////////////////////////////////////////////////////////////
struct ICode::ICodeImpl
{
    std::unique_ptr<ICodeNode> root;
};

////////////////////////////////////////////////////////////////////////////////
ICode::ICode()
: pimpl(new ICode::ICodeImpl) {}

ICode::~ICode() {}

ICodeNode* ICode::setRoot(std::unique_ptr<ICodeNode> node)
{
    pimpl->root = std::move(node);
    return node.get();
}

ICodeNode* ICode::root() const
{
    return pimpl->root.get();
}

////////////////////////////////////////////////////////////////////////////////
struct ICodeNode::ICodeNodeImpl
{
    explicit ICodeNodeImpl(ICodeNodeType t)
    : type(t) {}
    
    ICodeNodeType type;
    ICodeNode *parent {nullptr};
    std::vector<std::unique_ptr<ICodeNode>> children;
    std::map<ICodeKey, ICodeNodeAttrValue> attributes;
};

////////////////////////////////////////////////////////////////////////////////
ICodeNode::ICodeNode(ICodeNodeType type)
: pimpl(new ICodeNode::ICodeNodeImpl(type)) {}

ICodeNode::~ICodeNode() {}

ICodeNodeType ICodeNode::type() const
{
    return pimpl->type;
}

ICodeNode* ICodeNode::parent()
{
    return pimpl->parent;
}

ICodeNode* ICodeNode::add_child(std::unique_ptr<ICodeNode> node)
{
    if (node)
    {
        node->pimpl->parent = this;
        pimpl->children.push_back(std::move(node));
    }
    return node.get();
}

std::vector<ICodeNode *> ICodeNode::children() const
{
    std::vector<ICodeNode *> vec;
    std::for_each(pimpl->children.begin(),
                  pimpl->children.end(),
                  [&](std::unique_ptr<ICodeNode> &node) {
                      vec.push_back(node.get());
                  });
    return vec;
}

void ICodeNode::set_attribute(ICodeKey key, ICodeNodeAttrValue value)
{
    pimpl->attributes[key] = value;
}

boost::optional<ICodeNodeAttrValue> ICodeNode::attribute(ICodeKey key) const
{
    boost::optional<ICodeNodeAttrValue> result;
    auto search = pimpl->attributes.find(key);
    if (search != pimpl->attributes.end())
    {
        result = search->second;
    }
    return result;
}

void ICodeNode::foreach_attribute(std::function<void (ICodeKey, ICodeNodeAttrValue)> f) const
{
    for(const auto& p : pimpl->attributes)
    {
        f(p.first, p.second);
    }
}

////////////////////////////////////////////////////////////////////////////////
std::unique_ptr<ICode> ICodeFactory::create_icode()
{
    std::unique_ptr<ICode> icode(new ICode);
    return icode;
}

std::unique_ptr<ICodeNode> ICodeFactory::create_node(ICodeNodeType type)
{
    std::unique_ptr<ICodeNode> icode(new ICodeNode(type));
    return icode;
}

