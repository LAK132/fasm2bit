#include "parser.hpp"

basic_parser::result<char> basic_parser::peek() const
{
	if (input.empty()) return lak::err_t{error_type::end_of_file};

	return lak::ok_t{input[0]};
}

basic_parser::result<char> basic_parser::pop()
{
	if (input.empty()) return lak::err_t{error_type::end_of_file};

	char result = input[0];
	input       = input.substr(1);
	return lak::ok_t{result};
}

basic_parser::result<char> basic_parser::peek_char(
  std::initializer_list<char> c)
{
	RES_TRY_ASSIGN(const char v =, peek());
	for (const char i : c)
		if (i == v) return lak::ok_t{v};
	return lak::err_t{error_type::unexpected_character};
}

basic_parser::result<char> basic_parser::pop_char(
  std::initializer_list<char> c)
{
	RES_TRY_ASSIGN(const char v =, peek());
	for (const char i : c)
	{
		if (i == v)
		{
			pop().unwrap();
			return lak::ok_t{v};
		}
	}
	return lak::err_t{error_type::unexpected_character};
}

basic_parser::result<char> basic_parser::peek_not_char(
  std::initializer_list<char> c)
{
	RES_TRY_ASSIGN(const char v =, peek());
	for (const char i : c)
		if (i == v) return lak::err_t{error_type::unexpected_character};
	return lak::ok_t{v};
}

basic_parser::result<char> basic_parser::pop_not_char(
  std::initializer_list<char> c)
{
	RES_TRY_ASSIGN(const char v =, peek());
	for (const char i : c)
		if (i == v) return lak::err_t{error_type::unexpected_character};
	pop().unwrap();
	return lak::ok_t{v};
}

basic_parser::result<lak::astring_view> basic_parser::peek_string(
  lak::astring_view str)
{
	if (input.size() < str.size())
		return lak::err_t{error_type::end_of_file};
	else if (const auto substr = input.first(str.size()); substr == str)
		return lak::ok_t{substr};
	else
		return lak::err_t{error_type::unexpected_character};
}

basic_parser::result<lak::astring_view> basic_parser::pop_string(
  lak::astring_view str)
{
	if (input.size() < str.size())
		return lak::err_t{error_type::end_of_file};
	else if (const auto substr = input.first(str.size()); substr == str)
	{
		input = input.substr(str.size());
		return lak::ok_t{substr};
	}
	else
		return lak::err_t{error_type::unexpected_character};
}

std::ostream &operator<<(std::ostream &strm,
                         const basic_parser::error_type &err)
{
	switch (err)
	{
		case basic_parser::error_type::end_of_file: return strm << "end of file";
		case basic_parser::error_type::unexpected_character:
			return strm << "unexpected character";
		case basic_parser::error_type::integer_overflow:
			return strm << "integer overflow";
		default: ASSERT_UNREACHABLE();
	}
}
