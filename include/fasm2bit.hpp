#ifndef FASM2BIT_HPP
#define FASM2BIT_HPP

#include "lak/debug.hpp"
#include "lak/errno_result.hpp"
#include "lak/string_literals.hpp"

#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

extern const lak::astring_view help_string;

lak::errno_result<std::vector<char>> read_file(const fs::path &path);

inline int user_error(const auto &...ars)
{
	lak::debugger.std_err(u8"" LAK_RED "ERROR: " LAK_SGR_RESET ""_str,
	                      lak::streamify(ars..., "\n"));
	return EXIT_FAILURE;
}

inline int user_error_cont(const auto &...ars)
{
	lak::debugger.std_err_cont(lak::streamify(ars..., "\n"));
	return EXIT_FAILURE;
}

#endif
