#include "csv.hpp"

#include "lak/string_literals.hpp"

csv_parser::result<lak::astring_view> csv_parser::parse_value()
{
	const char *begin = input.begin();

	while (pop_not_char({',', '\n', '\r'}).is_ok())
		;

	return lak::ok_t{lak::astring_view(begin, input.begin())};
}

csv_parser::result<lak::astring_view> csv_parser::parse_newline()
{
	const char *begin = input.begin();

	while (pop_char({'\r'}).is_ok())
		;
	RES_TRY(pop_char({'\n'}));
	while (pop_char({'\r'}).is_ok())
		;

	return lak::ok_t{lak::astring_view(begin, input.begin())};
}

csv_parser::result<csv_parser::line> csv_parser::parse_line()
{
	line result;

	while (peek_char({'\n', '\r'}).is_err())
	{
		RES_TRY_ASSIGN(const lak::astring_view value =, parse_value());
		result.values.push_back(value);
		if (pop_char({','}).is_err()) break;
	}

	return lak::ok_t{lak::move(result)};
}

csv_parser::result<std::vector<csv_parser::line>> csv_parser::parse()
{
	std::vector<line> result;

	while (!input.empty())
	{
		RES_TRY_ASSIGN(line l =, parse_line());
		result.push_back(lak::move(l));
		if (input.empty()) break;
		RES_TRY(parse_newline());
	}

	return lak::ok_t{lak::move(result)};
}

std::ostream &operator<<(std::ostream &strm, const csv_parser::line &line)
{
	if (!line.values.empty())
	{
		strm << line.values[0];
		for (const auto &value : lak::span(line.values).subspan(1))
		{
			strm << "," << value;
		}
	}
	return strm;
}

void csv_test()
{
	SCOPED_CHECKPOINT("CSV tests");

	DEBUG(LAK_GREEN "CSV tests complete" LAK_SGR_RESET);
}
