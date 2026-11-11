// This file is a part of toml++ and is subject to the the terms of the MIT license.
// Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
// See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

#include "examples.h"
#include <toml++/toml.h>

using namespace std::string_view_literals;

int main(int argc, char** argv)
{
	const auto path = argc > 1 ? std::string_view{ argv[1] } : "example.toml"sv;

	toml::table tbl;
	try
	{

		if (path == "-"sv || path.empty())
			tbl = toml::parse(std::cin, "stdin"sv);

		else
			tbl = toml::parse_file(path);
	}
	catch (const toml::parse_error& err)
	{
		std::cerr << err << "\n";
		return 1;
	}

	std::cout << tbl << "\n";
	return 0;
}
