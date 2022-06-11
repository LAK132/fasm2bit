#ifndef DATABASE_HPP
#define DATABASE_HPP

#include "lak/result.hpp"
#include "lak/string_view.hpp"

#include <filesystem>
#include <unordered_map>
#include <vector>

namespace fs = std::filesystem;

struct database
{
	static lak::result<database> open(const fs::path &path,
	                                  lak::astring_view family,
	                                  lak::astring_view fabric,
	                                  lak::astring_view package);
};

#endif
