#ifndef FASM_HPP
#define FASM_HPP

#include "bigint.hpp"
#include "parser.hpp"

#include "lak/optional.hpp"
#include "lak/result.hpp"
#include "lak/stdint.hpp"
#include "lak/string_view.hpp"
#include "lak/tuple.hpp"

#include <ostream>

struct fasm_parser : public basic_parser
{
	result<lak::astring_view> parse_non_newline_whitespace();

	result<lak::astring_view> parse_identifier();
	result<std::vector<lak::astring_view>> parse_feature();

	result<lak::bigint> parse_dec_value();
	result<lak::bigint> parse_hex_value();
	result<lak::bigint> parse_oct_value();
	result<lak::bigint> parse_bin_value();

	result<uint8_t> parse_value_base();
	result<uintmax_t> parse_verilog_value_width();

	struct verilog_value
	{
		lak::optional<uintmax_t> width;
		lak::bigint value;
	};
	result<verilog_value> parse_verilog_value();

	struct feature_address
	{
		uintmax_t address1;
		lak::optional<uintmax_t> address2;
	};
	result<feature_address> parse_feature_address();

	result<lak::astring_view> parse_comment();

	struct annotation
	{
		lak::astring_view name;
		lak::astring_view value;
	};
	result<lak::astring_view> parse_annotation_name();
	result<lak::astring_view> parse_annotation_value();
	result<annotation> parse_annotation();
	result<std::vector<annotation>> parse_annotations();

	struct fasm_feature
	{
		std::vector<lak::astring_view> feature;
		lak::optional<feature_address> address;
		lak::optional<verilog_value> value;
	};
	result<fasm_feature> parse_set_feature();

	struct line
	{
		lak::optional<fasm_feature> feature;
		std::vector<annotation> annotations;
		lak::astring_view comment;
	};
	result<line> parse_line();
	result<std::vector<line>> parse();
};

std::ostream &operator<<(std::ostream &strm,
                         const fasm_parser::verilog_value &value);

std::ostream &operator<<(std::ostream &strm,
                         const fasm_parser::feature_address &value);

std::ostream &operator<<(std::ostream &strm,
                         const fasm_parser::annotation &value);

std::ostream &operator<<(std::ostream &strm,
                         const fasm_parser::fasm_feature &value);

std::ostream &operator<<(std::ostream &strm, const fasm_parser::line &value);

void fasm_test();

#endif
