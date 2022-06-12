#ifndef JSON_HPP
#define JSON_HPP

#include "parser.hpp"

#include "lak/result.hpp"
#include "lak/string_view.hpp"

struct json_parser : public basic_parser
{
	struct value_type;
	struct key_value;
	struct array
	{
		std::vector<value_type> values;
	};
	struct object
	{
		std::vector<key_value> key_values;
	};
	struct string
	{
		lak::astring_view value;
	};
	struct value_type
	{
		lak::variant<lak::astring_view, string, array, object> value;

		inline const lak::astring_view *lit() const
		{
			return value.template get<lak::astring_view>();
		}
		inline const lak::astring_view *str() const
		{
			if (const auto *p = value.template get<string>(); p)
				return &p->value;
			else
				return nullptr;
		}
		inline const array *arr() const { return value.template get<array>(); }
		inline const object *obj() const { return value.template get<object>(); }
	};
	struct key_value
	{
		string key;
		value_type value;
	};

	lak::astring_view parse_whitespace();

	result<string> parse_string();
	result<lak::astring_view> parse_number();
	result<lak::astring_view> parse_literal();

	result<array> parse_array();

	result<key_value> parse_key_value();
	result<object> parse_object();

	result<value_type> parse_value();

	result<value_type> parse();
};

std::ostream &operator<<(std::ostream &strm,
                         const json_parser::value_type &value);

std::ostream &operator<<(std::ostream &strm,
                         const json_parser::key_value &value);

std::ostream &operator<<(std::ostream &strm, const json_parser::array &value);

std::ostream &operator<<(std::ostream &strm, const json_parser::object &value);

std::ostream &operator<<(std::ostream &strm, const json_parser::string &value);

void json_test();

#endif
