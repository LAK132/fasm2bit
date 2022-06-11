#include "database.hpp"

#include "fasm2bit.hpp"

lak::result<database> database::open(const fs::path &path,
                                     lak::astring_view family,
                                     lak::astring_view fabric,
                                     lak::astring_view package)
{
	database result;

	const auto family_path{path / family.to_string()};
	const auto fabric_path{family_path / fabric.to_string()};
	const auto package_path{family_path / package.to_string()};

	auto open_file = [](const std::filesystem::path &path)
	{
		return read_file(path).map_err(
		  [&](const auto &err) -> lak::monostate
		  {
			  user_error("Failed to open ", path, ": ", err);
			  return {};
		  });
	};

	return lak::ok_t{lak::move(result)};
}
