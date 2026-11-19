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

	class TOML_EXPORTED_CLASS json_formatter : impl::formatter
	{
	  private:

		using base = impl::formatter;

		TOML_EXPORTED_MEMBER_FUNCTION
		void print(const toml::table&);

		TOML_EXPORTED_MEMBER_FUNCTION
		void print(const toml::array&);

		TOML_EXPORTED_MEMBER_FUNCTION
		void print();

		static constexpr impl::formatter_constants constants = {
			format_flags::quote_dates_and_times,										   
			format_flags::allow_literal_strings | format_flags::allow_multi_line_strings,  
			"Infinity"sv,
			"-Infinity"sv,
			"NaN"sv,
			"true"sv,
			"false"sv
		};

	  public:

		static constexpr format_flags default_flags = constants.mandatory_flags				   
													| format_flags::quote_infinities_and_nans  
													| format_flags::allow_unicode_strings	   
													| format_flags::indentation;

		TOML_NODISCARD_CTOR
		explicit json_formatter(const toml::node& source, format_flags flags = default_flags) noexcept
			: base{ &source, nullptr, constants, { flags, "    "sv } }
		{}

#if TOML_DOXYGEN || (TOML_ENABLE_PARSER && !TOML_EXCEPTIONS)

		TOML_NODISCARD_CTOR
		explicit json_formatter(const toml::parse_result& result, format_flags flags = default_flags) noexcept
			: base{ nullptr, &result, constants, { flags, "    "sv } }
		{}

#endif

		friend std::ostream& operator<<(std::ostream& lhs, json_formatter& rhs)
		{
			rhs.attach(lhs);
			rhs.print();
			rhs.detach();
			return lhs;
		}

		friend std::ostream& operator<<(std::ostream& lhs, json_formatter&& rhs)
		{
			return lhs << rhs;  
		}
	};
}
TOML_NAMESPACE_END;

#include "header_end.h"
#endif  
