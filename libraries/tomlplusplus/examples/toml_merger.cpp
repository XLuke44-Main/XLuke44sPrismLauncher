// This file is a part of toml++ and is subject to the the terms of the MIT license.
// Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
// See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

#include "examples.h"
#include <toml++/toml.h>

using namespace std::string_view_literals;

namespace
{

	static void merge_left(toml::table& lhs, toml::table&& rhs);

	static void merge_left(toml::array& lhs, toml::array&& rhs)
	{
		rhs.for_each(
			[&](std::size_t index, auto&& rhs_val)
			{

				if (lhs.size() <= index)
				{
					lhs.push_back(std::move(rhs_val));
					return;
				}

				if constexpr (toml::is_container<decltype(rhs_val)>)
				{
					using rhs_type = std::remove_cv_t<std::remove_reference_t<decltype(rhs_val)>>;
					if (auto lhs_child = lhs[index].as<rhs_type>())
					{
						merge_left(*lhs_child, std::move(rhs_val));
						return;
					}
				}

				lhs.replace(lhs.cbegin() + static_cast<std::ptrdiff_t>(index), std::move(rhs_val));
			});
	}

	static void merge_left(toml::table& lhs, toml::table&& rhs)
	{
		rhs.for_each(
			[&](const toml::key& rhs_key, auto&& rhs_val)
			{
				auto lhs_it = lhs.lower_bound(rhs_key);

				if (lhs_it == lhs.cend() || lhs_it->first != rhs_key)
				{
					using rhs_type = std::remove_cv_t<std::remove_reference_t<decltype(rhs_val)>>;
					lhs.emplace_hint<rhs_type>(lhs_it, rhs_key, std::move(rhs_val));
					return;
				}

				if constexpr (toml::is_container<decltype(rhs_val)>)
				{
					using rhs_type = std::remove_cv_t<std::remove_reference_t<decltype(rhs_val)>>;
					if (auto lhs_child = lhs_it->second.as<rhs_type>())
					{
						merge_left(*lhs_child, std::move(rhs_val));
						return;
					}
				}

				lhs.insert_or_assign(rhs_key, std::move(rhs_val));
			});
	}
}

int main(int argc, char** argv)
{
	const auto base_path	  = argc > 1 ? std::string_view{ argv[1] } : "merge_base.toml"sv;
	const auto overrides_path = argc > 2 ? std::string_view{ argv[2] } : "merge_overrides.toml"sv;

	toml::table tbl;
	try
	{
		tbl = toml::parse_file(base_path);
		merge_left(tbl, toml::parse_file(overrides_path));
	}
	catch (const toml::parse_error& err)
	{
		std::cerr << err << "\n";
		return 1;
	}

	std::cout << tbl << "\n";
	return 0;
}
