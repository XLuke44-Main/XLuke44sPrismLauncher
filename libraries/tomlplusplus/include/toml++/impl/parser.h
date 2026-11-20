//# This file is a part of toml++ and is subject to the the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT
#pragma once

#include "preprocessor.h"
#if TOML_ENABLE_PARSER

#include "table.h"
#include "parse_result.h"
#include "header_start.h"

TOML_NAMESPACE_START
{
	TOML_ABI_NAMESPACE_BOOL(TOML_EXCEPTIONS, ex, noex);

	TOML_NODISCARD
	TOML_EXPORTED_FREE_FUNCTION
	parse_result TOML_CALLCONV parse(std::string_view doc, std::string_view source_path = {});

	TOML_NODISCARD
	TOML_EXPORTED_FREE_FUNCTION
	parse_result TOML_CALLCONV parse(std::string_view doc, std::string && source_path);

	TOML_NODISCARD
	TOML_EXPORTED_FREE_FUNCTION
	parse_result TOML_CALLCONV parse_file(std::string_view file_path);

#if TOML_HAS_CHAR8

	TOML_NODISCARD
	TOML_EXPORTED_FREE_FUNCTION
	parse_result TOML_CALLCONV parse(std::u8string_view doc, std::string_view source_path = {});

	TOML_NODISCARD
	TOML_EXPORTED_FREE_FUNCTION
	parse_result TOML_CALLCONV parse(std::u8string_view doc, std::string && source_path);

	TOML_NODISCARD
	TOML_EXPORTED_FREE_FUNCTION
	parse_result TOML_CALLCONV parse_file(std::u8string_view file_path);

#endif  

#if TOML_ENABLE_WINDOWS_COMPAT

	TOML_NODISCARD
	TOML_EXPORTED_FREE_FUNCTION
	parse_result TOML_CALLCONV parse(std::string_view doc, std::wstring_view source_path);

	TOML_NODISCARD
	TOML_EXPORTED_FREE_FUNCTION
	parse_result TOML_CALLCONV parse(std::istream & doc, std::wstring_view source_path);

	TOML_NODISCARD
	TOML_EXPORTED_FREE_FUNCTION
	parse_result TOML_CALLCONV parse_file(std::wstring_view file_path);

#endif  

#if TOML_HAS_CHAR8 && TOML_ENABLE_WINDOWS_COMPAT

	TOML_NODISCARD
	TOML_EXPORTED_FREE_FUNCTION
	parse_result TOML_CALLCONV parse(std::u8string_view doc, std::wstring_view source_path);

#endif  

	TOML_NODISCARD
	TOML_EXPORTED_FREE_FUNCTION
	parse_result TOML_CALLCONV parse(std::istream & doc, std::string_view source_path = {});

	TOML_NODISCARD
	TOML_EXPORTED_FREE_FUNCTION
	parse_result TOML_CALLCONV parse(std::istream & doc, std::string && source_path);

	TOML_ABI_NAMESPACE_END;  

	inline namespace literals
	{
		TOML_ABI_NAMESPACE_BOOL(TOML_EXCEPTIONS, lit_ex, lit_noex);

		TOML_NODISCARD
		TOML_ALWAYS_INLINE
		parse_result operator"" _toml(const char* str, size_t len)
		{
			return parse(std::string_view{ str, len });
		}

#if TOML_HAS_CHAR8

		TOML_NODISCARD
		TOML_ALWAYS_INLINE
		parse_result operator"" _toml(const char8_t* str, size_t len)
		{
			return parse(std::u8string_view{ str, len });
		}

#endif  

		TOML_ABI_NAMESPACE_END;  
	}
}
TOML_NAMESPACE_END;

#include "header_end.h"
#endif  
