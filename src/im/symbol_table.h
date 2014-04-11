#ifndef _PSC_IM_SYMBOL_TABLE_H_
#define _PSC_IM_SYMBOL_TABLE_H_

#include <string>
#include <memory>
#include <vector>
#include <boost/optional.hpp>
#include <boost/variant.hpp>

namespace psc { namespace im {

enum class SymbolTableKey
{
	// Constant.
	ConstantValue,
	// Procedure or function.
	RoutineCode, RoutineSymbolTable, RoutineICode,
	RoutineParams, RoutineRoutines,
	// Variable or record field value.
	DataValue
};

using SymbolTableKeyValue = boost::variant<bool, int, double, std::string, void *>;

class SymbolTable;
class SymbolTableEntry;

class SymbolTableStack final
{
public:
    SymbolTableStack();
	~SymbolTableStack();

	/* the current nesting level */
	int current_nesting_level() const;

	/* 
	 * Get the local symbol table that is on
	 * top of the stack.
	 */
	SymbolTable* local_table() const;

	/*
	 * Create a new entry in the local symbol table.
	 */
	SymbolTableEntry* enter_local(const std::string &name);

	/*
	 * Lookup an entry by name, in the local symbol table.
	 */
	boost::optional<SymbolTableEntry*> lookup_local(const std::string &name) const;

	/*
	 * Lookup an entry by nane, in all symbol tables, starting with the local.
	 */
	boost::optional<SymbolTableEntry*> lookup(const std::string &name) const;

private:
	struct SymbolTableStackImpl;
	std::unique_ptr<SymbolTableStackImpl> pimpl;
};

class SymbolTable final
{
public:
	explicit SymbolTable(int level);
	~SymbolTable();

	/*
	 * Get the nesting level.
	 */
	int nesting_level() const;

	/*
	 * Create a new entry.
	 */
	SymbolTableEntry* enter(const std::string &name);

	/*
	 * Lookup an entry by name.
	 */
	boost::optional<SymbolTableEntry*> lookup(const std::string &name);

	/*
	 * All entries sorted by name.
	 */
	std::vector<SymbolTableEntry*> sorted_entries() const;
private:
	struct SymbolTableImpl;
	std::unique_ptr<SymbolTableImpl> pimpl;
};

class SymbolTableEntry final
{
public:
	SymbolTableEntry(const std::string& name, SymbolTable *table);
	~SymbolTableEntry();

	// Get the name of the entry.
	std::string name() const;

	// Get the parent symbol table.
	SymbolTable* symbol_table() const;

	// Append a source line number for this entry.
	void append_line(int number);

	// Get all line numbers (sorted)
	std::vector<int> line_numbers() const;

	// Set key-value attribute
	void set_attribute(SymbolTableKey key, SymbolTableKeyValue val);

	// Get an attribute for a given key
    SymbolTableKeyValue attribute(SymbolTableKey key) const;
private:
	struct SymbolTableEntryImpl;
	std::unique_ptr<SymbolTableEntryImpl> pimpl;
};

class SymbolTableFactory final
{
public:
	SymbolTableFactory() = delete;

	static std::unique_ptr<SymbolTableStack> make_stack();
	static SymbolTable* make_table(int level);
	static SymbolTableEntry* make_entry(const std::string& name, SymbolTable *table);
};

}}

#endif // _PSC_IM_SYMBOL_TABLE_H_
