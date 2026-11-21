//# This file is a part of toml++ and is subject to the the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT
#pragma once

#include "forward_declarations.h"

TOML_IMPL_NAMESPACE_START
{
	template <typename T>
	using parse_path_callback = bool(TOML_CALLCONV*)(void*, T);

	TOML_NODISCARD
	bool TOML_CALLCONV parse_path(std::string_view,
								  void*,
								  parse_path_callback<std::string_view>,
								  parse_path_callback<size_t>);
}
TOML_IMPL_NAMESPACE_END;

TOML_NAMESPACE_START
{

	TOML_NODISCARD
	TOML_EXPORTED_FREE_FUNCTION
	node_view<node> TOML_CALLCONV at_path(node & root, std::string_view path) noexcept;

	TOML_NODISCARD
	TOML_EXPORTED_FREE_FUNCTION
	node_view<const node> TOML_CALLCONV at_path(const node& root, std::string_view path) noexcept;

#if TOML_ENABLE_WINDOWS_COMPAT

	TOML_NODISCARD
	TOML_EXPORTED_FREE_FUNCTION
	node_view<node> TOML_CALLCONV at_path(node & root, std::wstring_view path);

	TOML_NODISCARD
	TOML_EXPORTED_FREE_FUNCTION
	node_view<const node> TOML_CALLCONV at_path(const node& root, std::wstring_view path);

#endif
}
TOML_NAMESPACE_END;
