#ifndef _PSC_IM_XREF_H_
#define _PSC_IM_XREF_H_

namespace psc {
namespace im {

class SymbolTableStack;
class SymbolTable;
class SymbolTableEntry;

class CrossReferencer final
{
public:
	void print(const SymbolTableStack *symtabstack) const;
private:
	void print_column_headings() const;
	void print_symbol_table(const SymbolTable *table) const;
};

}
}

#endif // _PSC_IM_XREF_H_