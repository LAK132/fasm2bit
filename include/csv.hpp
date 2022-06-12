#ifndef CSV_HPP
#define CSV_HPP

#include "parser.hpp"

#include "lak/result.hpp"
#include "lak/string_view.hpp"

struct csv_parser : public basic_parser
{
	result<lak::astring_view> parse_value();
	result<lak::astring_view> parse_newline();

	struct line
	{
		std::vector<lak::astring_view> values;
	};
	result<line> parse_line();

	result<std::vector<line>> parse();
};

std::ostream &operator<<(std::ostream &strm, const csv_parser::line &line);

void csv_test();

#endif
