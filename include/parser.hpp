#ifndef PARSER_HPP
#define PARSER_HPP

#include "lak/result.hpp"
#include "lak/string_view.hpp"

#include <ostream>

struct basic_parser
{
	lak::astring_view input;

	enum struct error_type
	{
		end_of_file          = 0,
		unexpected_character = 1,
		integer_overflow     = 2,
	};

	template<typename T = lak::monostate>
	using result = lak::result<T, error_type>;

	result<char> peek() const;
	result<char> pop();

	result<char> peek_char(std::initializer_list<char> c);
	result<char> pop_char(std::initializer_list<char> c);

	result<char> peek_not_char(std::initializer_list<char> c);
	result<char> pop_not_char(std::initializer_list<char> c);

	result<lak::astring_view> peek_string(lak::astring_view str);
	result<lak::astring_view> pop_string(lak::astring_view str);
};

std::ostream &operator<<(std::ostream &strm,
                         const basic_parser::error_type &err);

#endif
