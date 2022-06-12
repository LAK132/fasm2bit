#include "fasm.hpp"

#include "lak/string_literals.hpp"

fasm_parser::result<lak::astring_view>
fasm_parser::parse_non_newline_whitespace()
{
	const char *begin = input.begin();

	while (pop_char({' ', '\t'}).is_ok())
		;

	return lak::ok_t{lak::astring_view(begin, input.begin())};
}

fasm_parser::result<lak::astring_view> fasm_parser::parse_identifier()
{
	const char *begin = input.begin();

	RES_TRY_ASSIGN(char c =, peek());
	if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
	    (c >= '0' && c <= '9'))
	{
		for (pop().unwrap();; pop().unwrap())
		{
			if_let_ok (c = char, peek())
			{
				if (!(c >= 'a' && c <= 'z') && !(c >= 'A' && c <= 'Z') &&
				    !(c >= '0' && c <= '9') && c != '_')
					break;
			}
			else
				break;
		}
	}
	else
		return lak::err_t{error_type::unexpected_character};

	return lak::ok_t{lak::astring_view(begin, input.begin())};
}

fasm_parser::result<std::vector<lak::astring_view>>
fasm_parser::parse_feature()
{
	std::vector<lak::astring_view> result;

	do
	{
		RES_TRY_ASSIGN(const lak::astring_view ident =, parse_identifier());
		result.push_back(ident);
	} while (pop_char({'.'}).is_ok());

	return lak::ok_t{lak::move(result)};
}

fasm_parser::result<lak::bigint> fasm_parser::parse_dec_value()
{
	lak::bigint result = 0;

	RES_TRY_ASSIGN(char c =, peek());
	if ((c >= '0' && c <= '9') || c == '_')
	{
		if (c != '_')
		{
			result *= 10;
			result += c - '0';
		}
		for (pop().unwrap();; pop().unwrap())
		{
			if_let_ok (c = char, peek())
			{
				if (!(c >= '0' && c <= '9') && c != '_') break;
				if (c != '_')
				{
					result *= 10;
					result += (c - '0');
				}
			}
			else
				break;
		}
	}
	else
		return lak::err_t{error_type::unexpected_character};

	return lak::ok_t{result};
}

fasm_parser::result<lak::bigint> fasm_parser::parse_hex_value()
{
	lak::bigint result = 0;

	RES_TRY_ASSIGN(char c =, peek());
	if ((c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F') ||
	    (c >= '0' && c <= '9') || c == '_')
	{
		if (c >= 'a' && c <= 'f')
		{
			result <<= 4U;
			result += uintmax_t(0xA + (c - 'a'));
		}
		else if (c >= 'A' && c <= 'F')
		{
			result <<= 4U;
			result += uintmax_t(0xA + (c - 'A'));
		}
		else if (c != '_')
		{
			result <<= 4U;
			result += uintmax_t(c - '0');
		}

		for (pop().unwrap();; pop().unwrap())
		{
			if_let_ok (c = char, peek())
			{
				if (!(c >= 'a' && c <= 'f') && !(c >= 'A' && c <= 'F') &&
				    !(c >= '0' && c <= '9') && c != '_')
					break;
				if (c >= 'a' && c <= 'f')
				{
					result <<= 4U;
					result += uintmax_t(0xA + (c - 'a'));
				}
				else if (c >= 'A' && c <= 'F')
				{
					result <<= 4U;
					result += uintmax_t(0xA + (c - 'A'));
				}
				else if (c != '_')
				{
					result <<= 4U;
					result += uintmax_t(c - '0');
				}
			}
			else
				break;
		}
	}
	else
		return lak::err_t{error_type::unexpected_character};

	return lak::ok_t{result};
}

fasm_parser::result<lak::bigint> fasm_parser::parse_oct_value()
{
	lak::bigint result = 0;

	RES_TRY_ASSIGN(char c =, peek());
	if ((c >= '0' && c <= '7') || c == '_')
	{
		if (c != '_')
		{
			result <<= 3U;
			result += uintmax_t(c - '0');
		}
		for (pop().unwrap();; pop().unwrap())
		{
			if_let_ok (c = char, peek())
			{
				if (!(c >= '0' && c <= '7') && c != '_') break;
				if (c != '_')
				{
					result <<= 3U;
					result += uintmax_t(c - '0');
				}
			}
			else
				break;
		}
	}
	else
		return lak::err_t{error_type::unexpected_character};

	return lak::ok_t{result};
}

fasm_parser::result<lak::bigint> fasm_parser::parse_bin_value()
{
	lak::bigint result = 0;

	RES_TRY_ASSIGN(char c =, peek());
	if ((c >= '0' && c <= '1') || c == '_')
	{
		if (c != '_')
		{
			result <<= 1U;
			result += uintmax_t(c - '0');
		}
		for (pop().unwrap();; pop().unwrap())
		{
			if_let_ok (c = char, peek())
			{
				if (!(c >= '0' && c <= '1') && c != '_') break;
				if (c != '_')
				{
					result <<= 1U;
					result += uintmax_t(c - '0');
				}
			}
			else
				break;
		}
	}
	else
		return lak::err_t{error_type::unexpected_character};

	return lak::ok_t{result};
}

fasm_parser::result<uint8_t> fasm_parser::parse_value_base()
{
	if (input.size() < 2) return lak::err_t{error_type::end_of_file};
	if (input[0] != '0')
	{
		if (input[0] >= '0' && input[0] <= '9')
			return lak::ok_t<uint8_t>{10};
		else
			return lak::err_t{error_type::unexpected_character};
	}
	uint8_t result = 0;
	if (input[1] == 'b' || input[1] == 'B')
		result = 2;
	else if (input[1] == 'o' || input[1] == 'O')
		result = 8;
	else if (input[1] == 'x' || input[1] == 'X')
		result = 16;
	else
		return lak::err_t{error_type::unexpected_character};
	input = input.substr(2);
	return lak::ok_t{result};
}

fasm_parser::result<uintmax_t> fasm_parser::parse_verilog_value_width()
{
	const char *begin = input.begin();
	DEFER({
		if (begin) input = lak::astring_view(begin, input.end());
	});

	uintmax_t result;

	if_let_ok (const uint8_t base, parse_value_base())
	{
		lak::result<uintmax_t> value = lak::err_t{};
		switch (base)
		{
			case 2:
			{
				RES_TRY_ASSIGN(value =, parse_bin_value()).to_uintmax();
			}
			break;
			case 8:
			{
				RES_TRY_ASSIGN(value =, parse_oct_value()).to_uintmax();
			}
			break;
			case 10:
			{
				RES_TRY_ASSIGN(value =, parse_dec_value()).to_uintmax();
			}
			break;
			case 16:
			{
				RES_TRY_ASSIGN(value =, parse_hex_value()).to_uintmax();
			}
			break;
			default: ASSERT_UNREACHABLE();
		}
		RES_TRY_ASSIGN(
		  result =,
		  value.map_err([](auto &&) { return error_type::integer_overflow; }));
	}

	RES_TRY(parse_non_newline_whitespace());

	RES_TRY(peek_char({'\''}));

	begin = nullptr;
	return lak::ok_t{result};
}

fasm_parser::result<fasm_parser::verilog_value>
fasm_parser::parse_verilog_value()
{
	verilog_value result;

	if_let_ok (const uintmax_t width, parse_verilog_value_width())
	{
		result.width = width;
	}
	else
		RES_TRY(parse_non_newline_whitespace());

	if (pop_char({'\''}).is_ok())
	{
		RES_TRY_ASSIGN(const char base =, pop_char({'b', 'o', 'd', 'h'}));
		RES_TRY(parse_non_newline_whitespace());
		switch (base)
		{
			case 'b':
			{
				RES_TRY_ASSIGN(result.value =, parse_bin_value());
			}
			break;
			case 'o':
			{
				RES_TRY_ASSIGN(result.value =, parse_oct_value());
			}
			break;
			case 'd':
			{
				RES_TRY_ASSIGN(result.value =, parse_dec_value());
			}
			break;
			case 'h':
			{
				RES_TRY_ASSIGN(result.value =, parse_hex_value());
			}
			break;
		}
	}
	else
	{
		RES_TRY_ASSIGN(result.value =, parse_dec_value());
	}

	return lak::ok_t{lak::move(result)};
}

fasm_parser::result<fasm_parser::feature_address>
fasm_parser::parse_feature_address()
{
	feature_address result;

	RES_TRY(pop_char({'['}));

	RES_TRY_ASSIGN(result.address1 =,
	               parse_dec_value().and_then(
	                 [](const lak::bigint &i)
	                 {
		                 return i.to_uintmax().map_err(
		                   [](auto &&) { return error_type::integer_overflow; });
	                 }));

	if (pop_char({':'}).is_ok())
	{
		RES_TRY_ASSIGN(result.address2 =,
		               parse_dec_value().and_then(
		                 [](const lak::bigint &i)
		                 {
			                 return i.to_uintmax().map_err(
			                   [](auto &&) { return error_type::integer_overflow; });
		                 }));
	}

	RES_TRY(pop_char({']'}));

	return lak::ok_t{result};
}

fasm_parser::result<lak::astring_view> fasm_parser::parse_comment()
{
	const char *begin = input.begin();

	RES_TRY(pop_char({'#'}));

	while (pop_not_char({'\n', '\r'}).is_ok())
		;

	return lak::ok_t{lak::astring_view(begin, input.begin())};
}

fasm_parser::result<lak::astring_view> fasm_parser::parse_annotation_name()
{
	const char *begin = input.begin();

	RES_TRY_ASSIGN(char c =, peek());
	if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
	    (c >= '0' && c <= '9') || c == '.')
	{
		for (pop().unwrap();; pop().unwrap())
		{
			if_let_ok (c = char, peek())
			{
				if (!(c >= 'a' && c <= 'z') && !(c >= 'A' && c <= 'Z') &&
				    !(c >= '0' && c <= '9') && c != '_')
					break;
			}
			else
				break;
		}
	}
	else
		return lak::err_t{error_type::unexpected_character};

	return lak::ok_t{lak::astring_view(begin, input.begin())};
}

fasm_parser::result<lak::astring_view> fasm_parser::parse_annotation_value()
{
	const char *begin = input.begin();

	while (pop_not_char({'\\', '"'}).is_ok() || pop_string("\\\""_view).is_ok())
		;

	return lak::ok_t{lak::astring_view(begin, input.begin())};
}

fasm_parser::result<fasm_parser::annotation> fasm_parser::parse_annotation()
{
	annotation result;

	RES_TRY_ASSIGN(result.name =, parse_annotation_name());

	RES_TRY(parse_non_newline_whitespace());

	RES_TRY(pop_char({'='}));

	RES_TRY(parse_non_newline_whitespace());

	RES_TRY(pop_char({'"'}));

	RES_TRY_ASSIGN(result.value =, parse_annotation_value());

	RES_TRY(pop_char({'"'}));

	return lak::ok_t{result};
}

fasm_parser::result<std::vector<fasm_parser::annotation>>
fasm_parser::parse_annotations()
{
	std::vector<annotation> result;

	RES_TRY(pop_char({'{'}));

	do
	{
		RES_TRY(parse_non_newline_whitespace());

		result.emplace_back();
		RES_TRY_ASSIGN(result.back() =, parse_annotation());
	} while (pop_char({','}).is_ok());

	RES_TRY(parse_non_newline_whitespace());

	RES_TRY(pop_char({'}'}));

	return lak::ok_t{lak::move(result)};
}

fasm_parser::result<fasm_parser::fasm_feature> fasm_parser::parse_set_feature()
{
	fasm_feature result;

	RES_TRY_ASSIGN(result.feature =, parse_feature());

	if (peek_char({'['}).is_ok())
	{
		RES_TRY_ASSIGN(result.address =, parse_feature_address());
	}

	RES_TRY(parse_non_newline_whitespace());

	if (pop_char({'='}).is_ok())
	{
		RES_TRY(parse_non_newline_whitespace());

		RES_TRY_ASSIGN(result.value =, parse_verilog_value());
	}

	return lak::ok_t{lak::move(result)};
}

fasm_parser::result<fasm_parser::line> fasm_parser::parse_line()
{
	line result;

	RES_TRY(parse_non_newline_whitespace());

	if (peek_not_char({'{', '#', '\n', '\r'}).is_ok())
	{
		RES_TRY_ASSIGN(result.feature =, parse_set_feature());
		RES_TRY(parse_non_newline_whitespace());
	}

	if (peek_char({'{'}).is_ok())
	{
		RES_TRY_ASSIGN(result.annotations =, parse_annotations());
		RES_TRY(parse_non_newline_whitespace());
	}

	if (peek_char({'#'}).is_ok())
	{
		RES_TRY_ASSIGN(result.comment =, parse_comment());
		RES_TRY(parse_non_newline_whitespace());
	}

	return lak::ok_t{lak::move(result)};
}

fasm_parser::result<std::vector<fasm_parser::line>> fasm_parser::parse()
{
	std::vector<line> result;

	while (!input.empty())
	{
		RES_TRY(lak::move(parse_line())
		          .map(
		            [&](auto &&line)
		            {
			            result.push_back(lak::move(line));
			            return lak::monostate{};
		            }));

		if_let_ok (const char c, peek_not_char({'\n', '\r'}))
			return lak::err_t{error_type::unexpected_character};

		while (pop_char({'\n', '\r'}).is_ok())
			;
	}

	return lak::ok_t{lak::move(result)};
}

std::ostream &operator<<(std::ostream &strm,
                         const fasm_parser::verilog_value &value)
{
	if (value.width) strm << std::dec << *value.width << "'";
	return strm << value.value;
}

std::ostream &operator<<(std::ostream &strm,
                         const fasm_parser::feature_address &value)
{
	strm << std::dec << "[" << value.address1;
	if (value.address2) strm << ":" << *value.address2;
	return strm << "]";
}

std::ostream &operator<<(std::ostream &strm,
                         const fasm_parser::annotation &value)
{
	return strm << value.name << " = \"" << value.value << "\"";
}

std::ostream &operator<<(std::ostream &strm,
                         const fasm_parser::fasm_feature &value)
{
	if (!value.feature.empty())
	{
		strm << value.feature[0];
		for (const auto &feature : lak::span(value.feature).subspan(1))
		{
			strm << "." << feature;
		}
	}
	if (value.address) strm << *value.address;
	if (value.value) strm << " = " << *value.value;
	return strm;
}

std::ostream &operator<<(std::ostream &strm, const fasm_parser::line &value)
{
	if (value.feature)
	{
		strm << *value.feature;
	}
	if (!value.annotations.empty())
	{
		if (value.feature) strm << " ";
		strm << "{ " << value.annotations[0];
		for (const auto &annotation : lak::span(value.annotations).subspan(1))
		{
			strm << ", " << annotation;
		}
		strm << " }";
	}
	if (!value.comment.empty() && (value.feature || !value.annotations.empty()))
		strm << " ";
	return strm << value.comment;
}

void fasm_test()
{
	DEBUG(LAK_GREEN "FASM tests complete" LAK_SGR_RESET);
}
