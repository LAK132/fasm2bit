#include "fasm2bit.hpp"

#include <fstream>

const lak::astring_view help_string =
  "Usage: fasm2bit "
  "--[un]compressed "
  "--fasm <path to fasm> "
  "--out <path to output bitstream>"_view;

lak::errno_result<std::vector<char>> read_file(const fs::path &path)
{
	std::ifstream file(path, std::ios::binary | std::ios::ate);
	if (!file.is_open()) return lak::err_t{lak::errno_error::last_error()};

	std::streampos file_size = file.tellg();
	if (file_size == std::streampos(-1))
		return lak::err_t{lak::errno_error::last_error()};

	file.seekg(0);
	if (file.fail()) return lak::err_t{lak::errno_error::last_error()};

	auto result = std::vector<char>(static_cast<size_t>(file_size), char{0});

	file.read(reinterpret_cast<char *>(result.data()), result.size());
	if (file.fail()) return lak::err_t{lak::errno_error::last_error()};

	return lak::ok_t{lak::move(result)};
}
