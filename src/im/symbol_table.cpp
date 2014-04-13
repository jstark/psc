#include "im/symbol_table.h"
#include <vector>
#include <map>

using namespace psc::im;

///////////////////////////////////////////////////////////////////////////////
struct SymbolTableStack::SymbolTableStackImpl
{
    SymbolTableStackImpl()
    {
        tables.push_back(SymbolTableFactory::make_table(0));
    }
	boost::optional<SymbolTableEntry *> lookup_local(const std::string &name) const;
	boost::optional<SymbolTableEntry *> lookup(const std::string &name) const;

	int nesting_level{ 0 };
	std::vector<SymbolTable *> tables; // TODO add a local symbol table here
};

///////////////////////////////////////////////////////////////////////////////
SymbolTableStack::SymbolTableStack()
: pimpl(new SymbolTableStack::SymbolTableStackImpl()) {}
    
SymbolTableStack::~SymbolTableStack()
{
    std::for_each(
        pimpl->tables.begin(),
        pimpl->tables.end(),
        [](SymbolTable *p) { delete p;});
}

int SymbolTableStack::current_nesting_level() const
{
	return pimpl->nesting_level;
}

SymbolTable* SymbolTableStack::local_table() const
{
	return pimpl->tables.back();
}

SymbolTableEntry* SymbolTableStack::enter_local(const std::string &name)
{
	SymbolTable *tbl = pimpl->tables.at(current_nesting_level());
	return tbl->enter(name);
}

boost::optional<SymbolTableEntry*> SymbolTableStack::lookup_local(const std::string &name) const
{
	SymbolTable *table = pimpl->tables.at(current_nesting_level());
	return table->lookup(name);
}

boost::optional<SymbolTableEntry*> SymbolTableStack::lookup(const std::string &name) const
{
	return lookup_local(name); // TODO
}

///////////////////////////////////////////////////////////////////////////////
struct SymbolTable::SymbolTableImpl
{
	explicit SymbolTableImpl(int level)
	: nest_level(level) {}
	int nest_level{ 0 };
	std::map<std::string, SymbolTableEntry *> entries;
};

///////////////////////////////////////////////////////////////////////////////
SymbolTable::SymbolTable(int level)
: pimpl(new SymbolTable::SymbolTableImpl(level)) {}

SymbolTable::~SymbolTable()
{
    using entry_t = std::pair<const std::string, SymbolTableEntry *>;
    std::for_each(
        pimpl->entries.begin(),
        pimpl->entries.end(),
        [](entry_t p) { delete p.second; });
}

int SymbolTable::nesting_level() const
{
	return pimpl->nest_level;
}

SymbolTableEntry* SymbolTable::enter(const std::string &name)
{
	SymbolTableEntry *new_entry = SymbolTableFactory::make_entry(name, this);
	pimpl->entries[name] = new_entry;
	return new_entry;
}

boost::optional<SymbolTableEntry*> SymbolTable::lookup(const std::string &name)
{
	boost::optional<SymbolTableEntry *> lookup_result;
	auto search = pimpl->entries.find(name);
	if (search != pimpl->entries.end())
	{
		lookup_result = search->second;
	}
	return lookup_result;
}

std::vector<SymbolTableEntry*> SymbolTable::sorted_entries() const
{
	std::vector<SymbolTableEntry *> sorted;
	for (const auto &p : pimpl->entries)
	{
		sorted.push_back(p.second);
	}
	return sorted;
}

///////////////////////////////////////////////////////////////////////////////
struct SymbolTableEntry::SymbolTableEntryImpl
{
	SymbolTableEntryImpl(const std::string& n, SymbolTable *tb)
	: table(tb), name(n) {}
	SymbolTable *table{ nullptr };
	std::string name;
	std::vector<int> line_numbers;
	std::map<SymbolTableKey, SymbolTableKeyValue> attributes;
};

///////////////////////////////////////////////////////////////////////////////
SymbolTableEntry::SymbolTableEntry(const std::string& name, SymbolTable *table)
: pimpl(new SymbolTableEntry::SymbolTableEntryImpl(name, table)) {}

SymbolTableEntry::~SymbolTableEntry() {}

std::string SymbolTableEntry::name() const
{
	return pimpl->name;
}

SymbolTable* SymbolTableEntry::symbol_table() const
{
	return pimpl->table;
}

void SymbolTableEntry::append_line(int number)
{
	pimpl->line_numbers.push_back(number);
}

std::vector<int> SymbolTableEntry::line_numbers() const
{
	return pimpl->line_numbers;
}

void SymbolTableEntry::set_attribute(SymbolTableKey key, SymbolTableKeyValue val)
{
	pimpl->attributes[key] = val;
}

SymbolTableKeyValue SymbolTableEntry::attribute(SymbolTableKey key) const
{
	auto attr = pimpl->attributes.find(key);
	if (attr != pimpl->attributes.end())
	{
		return attr->second;
	}
	return nullptr;
}

///////////////////////////////////////////////////////////////////////////////

std::unique_ptr<SymbolTableStack> SymbolTableFactory::make_stack()
{
    std::unique_ptr<SymbolTableStack> us(new SymbolTableStack);
    return us;
}

SymbolTable* SymbolTableFactory::make_table(int level)
{
	return new SymbolTable(level);
}

SymbolTableEntry* SymbolTableFactory::make_entry(const std::string& name, SymbolTable *table)
{
	return new SymbolTableEntry(name, table);
}
