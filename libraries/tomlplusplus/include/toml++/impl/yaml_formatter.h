//# This file is a part of toml++ and is subject to the the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT
#pragma once

#include "preprocessor.h"
#if TOML_ENABLE_FORMATTERS

#include "formatter.h"
#include "header_start.h"

TOML_NAMESPACE_START
{

	class TOML_EXPORTED_CLASS yaml_formatter : impl::formatter
	{
	  private:

		using base = impl::formatter;

		TOML_EXPORTED_MEMBER_FUNCTION
		void print_yaml_string(const value<std::string>&);

		TOML_EXPORTED_MEMBER_FUNCTION
		void print(const toml::table&, bool = false);

		TOML_EXPORTED_MEMBER_FUNCTION
		void print(const toml::array&, bool = false);

		TOML_EXPORTED_MEMBER_FUNCTION
		void print();

		static constexpr impl::formatter_constants constants = {

			format_flags::quote_dates_and_times | format_flags::indentation,  
			format_flags::allow_multi_line_strings,							  
			".inf"sv,
			"-.inf"sv,
			".NAN"sv,
			"true"sv,
			"false"sv
		};

	  public:

		static constexpr format_flags default_flags = constants.mandatory_flags			   
													| format_flags::allow_literal_strings  
													| format_flags::allow_unicode_strings  
													| format_flags::allow_octal_integers   
													| format_flags::allow_hexadecimal_integers;

		TOML_NODISCARD_CTOR
		explicit yaml_formatter(const toml::node& source, format_flags flags = default_flags) noexcept
			: base{ &source, nullptr, constants, { flags, "  "sv } }
		{}

#if TOML_DOXYGEN || (TOML_ENABLE_PARSER && !TOML_EXCEPTIONS)

		TOML_NODISCARD_CTOR
		explicit yaml_formatter(const toml::parse_result& result, format_flags flags = default_flags) noexcept
			: base{ nullptr, &result, constants, { flags, "  "sv } }
		{}

#endif

		friend std::ostream& TOML_CALLCONV operator<<(std::ostream& lhs, yaml_formatter& rhs)
		{
			rhs.attach(lhs);
			rhs.print();
			rhs.detach();
			return lhs;
		}

		friend std::ostream& TOML_CALLCONV operator<<(std::ostream& lhs, yaml_formatter&& rhs)
		{
			return lhs << rhs;  
		}
	};
}
TOML_NAMESPACE_END;

#include "header_end.h"
#endif  
