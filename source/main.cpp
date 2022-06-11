#include "bit.hpp"
#include "database.hpp"
#include "fasm.hpp"
#include "fasm2bit.hpp"

#include "lak/result.hpp"
#include "lak/stdint.hpp"

#include <vector>

struct argument_iterator
{
	int argc;
	const char **argv;

	lak::result<lak::astring_view> pop()
	{
		if (argc > 0)
		{
			lak::astring_view result = lak::astring_view::from_c_str(*argv);
			--argc;
			++argv;
			return lak::ok_t{result};
		}
		else
			return lak::err_t{};
	}

	lak::astring_view pop(lak::astring_view abort_message)
	{
		if (argc > 0)
		{
			lak::astring_view result = lak::astring_view::from_c_str(*argv);
			--argc;
			++argv;
			return result;
		}
		else
		{
			user_error(abort_message);
			user_error_cont(help_string);
			ABORT();
		}
	}

	bool empty() { return argc <= 0; }
};

void fasm_test();

lak::result<lak::monostate> res_main(int argc, const char **argv)
{
	argument_iterator arg_iter{.argc = argc - 1, .argv = argv + 1};

	lak::astring family_name;
	lak::astring fabric_name;
	lak::astring package_name;
	fs::path database_path;
	fs::path fasm_path;
	fs::path out_path;
	bool compressed = false;

	do
	{
		if (const auto command{arg_iter.pop("Missing arguments"_view)};
		    command == "--help"_view || command == "-h"_view)
		{
			std::cout << help_string << "\n";
			return lak::ok_t{};
		}
		else if (command == "--test")
		{
			fasm_test();
		}
		else if (command == "--compressed")
		{
			compressed = true;
		}
		else if (command == "--uncompressed")
		{
			compressed = false;
		}
		else if (command == "--family"_view)
		{
			family_name = arg_iter.pop("Expected family name, got nothing"_view);
		}
		else if (command == "--package"_view)
		{
			package_name = arg_iter.pop("Expected package name, got nothing"_view);
		}
		else if (command == "--fabric"_view)
		{
			fabric_name = arg_iter.pop("Expected fabric name, got nothing"_view);
		}
		else if (command == "--db-root"_view)
		{
			database_path =
			  arg_iter.pop("Expected database root path, got nothing"_view);
		}
		else if (command == "--fasm"_view)
		{
			fasm_path = arg_iter.pop("Expected fasm path, got nothing"_view);
		}
		else if (command == "--out"_view)
		{
			const auto out_path_res{arg_iter.pop()};
		}
		else
		{
			user_error("Unknown argument '"_view, command.to_string(), "'"_view);
			user_error_cont(help_string);
			return lak::err_t{};
		}
	} while (!arg_iter.empty());

	RES_TRY_ASSIGN(std::vector<byte_t> fasm_file =,
	               read_file(fasm_path).map_err(
	                 [&](const auto &err) -> lak::monostate
	                 {
		                 user_error(
		                   "Failed to open fasm file ", fasm_path, ": ", err);
		                 return {};
	                 }));

	RES_TRY_ASSIGN(std::vector<fasm_parser::line> fasm_lines =,
	               fasm_parser{.input = lak::astring_view(
	                             lak::span<const char>(lak::span(fasm_file)))}
	                 .parse()
	                 .map_err(
	                   [&](const auto &err) -> lak::monostate
	                   {
		                   user_error("Failed to read fasm file: ", err);
		                   return {};
	                   }));

	for (const auto &line : fasm_lines) DEBUG(line);

	RES_TRY_ASSIGN(
	  database db =,
	  database::open(database_path, family_name, fabric_name, package_name));

	return lak::ok_t{};
}

int main(int argc, const char **argv)
{
	return res_main(argc, argv).is_ok() ? EXIT_SUCCESS : EXIT_FAILURE;
}
