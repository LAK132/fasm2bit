#include "json.hpp"

#include "lak/string_literals.hpp"

lak::astring_view json_parser::parse_whitespace()
{
	const char *begin = input.begin();

	while (pop_char({' ', '\t', '\n', '\r'}).is_ok())
		;

	return lak::astring_view(begin, input.begin());
}

json_parser::result<json_parser::string> json_parser::parse_string()
{
	RES_TRY(pop_char({'"'}));

	string result;
	const char *begin = input.begin();

	while (pop_not_char({'\\', '"'}).is_ok() || pop_string("\\\""_view).is_ok())
		;

	result.value = lak::astring_view(begin, input.begin());

	RES_TRY(pop_char({'"'}));

	return lak::ok_t{result};
}

json_parser::result<lak::astring_view> json_parser::parse_number()
{
	const char *begin = input.begin();

	auto digits = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

	pop_char({'-'}).discard();

	if (pop_char({'0'}).is_err())
	{
		RES_TRY(peek_char({'1', '2', '3', '4', '5', '6', '7', '8', '9'}));
		do
		{
			RES_TRY(pop());
		} while (peek_char(digits).is_ok());
	}

	if (pop_char({'.'}).is_ok())
	{
		RES_TRY(pop_char(digits));
		while (pop_char(digits).is_ok())
			;
	}

	if (pop_char({'e', 'E'}).is_ok())
	{
		pop_char({'-', '+'}).discard();
		RES_TRY(pop_char(digits));
		while (pop_char(digits).is_ok())
			;
	}

	return lak::ok_t{lak::astring_view(begin, input.begin())};
}

json_parser::result<lak::astring_view> json_parser::parse_literal()
{
	if_let_ok (const lak::astring_view true_value, pop_string("true"_view))
		return lak::ok_t{true_value};
	else if_let_ok (const lak::astring_view false_value,
	                pop_string("false"_view))
		return lak::ok_t{false_value};
	else if_let_ok (const lak::astring_view null_value, pop_string("null"_view))
		return lak::ok_t{null_value};
	else
		return parse_number();
}

json_parser::result<json_parser::array> json_parser::parse_array()
{
	array result;

	RES_TRY(pop_char({'['}));

	parse_whitespace();

	while (peek_char({']'}).is_err())
	{
		RES_TRY_ASSIGN(value_type v =, parse_value());
		result.values.push_back(lak::move(v));
		parse_whitespace();
		if (pop_char({','}).is_err()) break;
		parse_whitespace();
	}

	RES_TRY(pop_char({']'}));

	return lak::ok_t{lak::move(result)};
}

json_parser::result<json_parser::key_value> json_parser::parse_key_value()
{
	key_value result;

	RES_TRY_ASSIGN(result.key =, parse_string());

	parse_whitespace();

	pop_char({':'}).discard();

	parse_whitespace();

	RES_TRY_ASSIGN(result.value =, parse_value());

	return lak::ok_t{lak::move(result)};
}

json_parser::result<json_parser::object> json_parser::parse_object()
{
	object result;

	RES_TRY(pop_char({'{'}));

	parse_whitespace();

	while (peek_char({'}'}).is_err())
	{
		RES_TRY_ASSIGN(key_value kv =, parse_key_value());
		result.key_values.push_back(lak::move(kv));
		parse_whitespace();
		if (pop_char({','}).is_err()) break;
		parse_whitespace();
	}

	RES_TRY(pop_char({'}'}));

	return lak::ok_t{lak::move(result)};
}

json_parser::result<json_parser::value_type> json_parser::parse_value()
{
	value_type result;

	if (peek_char({'"'}).is_ok())
	{
		RES_TRY_ASSIGN(result.value =, parse_string());
	}
	else if (peek_char({'{'}).is_ok())
	{
		RES_TRY_ASSIGN(result.value =, parse_object());
	}
	else if (peek_char({'['}).is_ok())
	{
		RES_TRY_ASSIGN(result.value =, parse_array());
	}
	else
	{
		RES_TRY_ASSIGN(result.value =, parse_literal());
	}

	return lak::ok_t{lak::move(result)};
}

json_parser::result<json_parser::value_type> json_parser::parse()
{
	parse_whitespace();
	return parse_value();
}

std::ostream &operator<<(std::ostream &strm,
                         const json_parser::value_type &value)
{
	lak::visit(value.value, [&](const auto &v) { strm << v; });
	return strm;
}

std::ostream &operator<<(std::ostream &strm,
                         const json_parser::key_value &value)
{
	return strm << value.key << " : " << value.value;
}

std::ostream &operator<<(std::ostream &strm, const json_parser::array &value)
{
	strm << "[";
	if (!value.values.empty())
	{
		strm << value.values[0];
		for (const auto &v : lak::span(value.values).subspan(1))
		{
			strm << ", " << v;
		}
	}
	return strm << "]";
}

std::ostream &operator<<(std::ostream &strm, const json_parser::object &value)
{
	strm << "{";
	if (!value.key_values.empty())
	{
		strm << value.key_values[0];
		for (const auto &kv : lak::span(value.key_values).subspan(1))
		{
			strm << ", " << kv;
		}
	}
	return strm << "}";
}

std::ostream &operator<<(std::ostream &strm, const json_parser::string &value)
{
	return strm << "\"" << value.value << "\"";
}

void json_test()
{
	SCOPED_CHECKPOINT("JSON tests");

	DEBUG(LAK_GREEN "JSON tests complete" LAK_SGR_RESET);
}
