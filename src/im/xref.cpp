#include "im/xref.h"
#include "im/symbol_table.h"
#include <string>
#include <vector>
#include <cstdio>

using namespace psc::im;

namespace
{
	const int NAME_WIDTH = 16;
	const std::string NAME_FORMAT		= "%-" + std::to_string(NAME_WIDTH) + "s";
	const std::string NUMBERS_LABEL		= " Line numbers    ";
	const std::string NUMBERS_UNDERLINE = " ------------	";
	const std::string NUMBER_FORMAT = " %03d";
	const int LABEL_WIDTH = NUMBERS_LABEL.size();
	const int INDENT_WIDTH = NAME_WIDTH + LABEL_WIDTH;
	const std::string INDENT(INDENT_WIDTH, ' ');
}

void CrossReferencer::print(const SymbolTableStack *symtabstack) const
{
	printf("\n===== CROSS-REFERENCE TABLE =====");
	print_column_headings();
	print_symbol_table(symtabstack->local_table());
}

void CrossReferencer::print_column_headings() const
{
	static char buffer[1024] = { 0 };

	printf("\n");
	sprintf(buffer, NAME_FORMAT.c_str(), "Identifier");
	printf("%s%s\n", buffer, NUMBERS_LABEL.c_str());
	sprintf(buffer, NAME_FORMAT.c_str(), "----------");
	printf("%s%s\n", buffer, NUMBERS_UNDERLINE.c_str());
}

void CrossReferencer::print_symbol_table(const SymbolTable *table) const
{
	std::vector<SymbolTableEntry *> entries =
		table->sorted_entries();

	for (auto entry : entries)
	{
		std::vector<int> line_nums = entry->line_numbers();
		// for each entry, print the identifier name
		// followed by the line number
		printf(NAME_FORMAT.c_str(), entry->name().c_str());
		for (auto num : line_nums)
		{
			printf(NUMBER_FORMAT.c_str(), num);
		}
		printf("\n");
	}
}