//# This file is a part of toml++ and is subject to the the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT
#pragma once

#include "preprocessor.h"
#if TOML_ENABLE_FORMATTERS

#include "std_vector.h"
#include "formatter.h"
#include "header_start.h"

TOML_NAMESPACE_START
{

	class TOML_EXPORTED_CLASS toml_formatter : impl::formatter
	{
	  private:

		using base = impl::formatter;
		std::vector<const key*> key_path_;
		bool pending_table_separator_ = false;

		TOML_EXPORTED_MEMBER_FUNCTION
		void print_pending_table_separator();

		TOML_EXPORTED_MEMBER_FUNCTION
		void print(const key&);

		TOML_EXPORTED_MEMBER_FUNCTION
		void print_inline(const toml::table&);

		TOML_EXPORTED_MEMBER_FUNCTION
		void print(const toml::array&);

		TOML_EXPORTED_MEMBER_FUNCTION
		void print(const toml::table&);

		TOML_EXPORTED_MEMBER_FUNCTION
		void print();

		static constexpr impl::formatter_constants constants = { format_flags::none,  
																 format_flags::none,  
																 "inf"sv,
																 "-inf"sv,
																 "nan"sv,
																 "true"sv,
																 "false"sv };

	  public:

		static constexpr format_flags default_flags = constants.mandatory_flags				    
													| format_flags::allow_literal_strings	    
													| format_flags::allow_multi_line_strings    
													| format_flags::allow_unicode_strings	    
													| format_flags::allow_real_tabs_in_strings  
													| format_flags::allow_binary_integers	    
													| format_flags::allow_octal_integers	    
													| format_flags::allow_hexadecimal_integers  
													| format_flags::indentation;

		TOML_NODISCARD_CTOR
		explicit toml_formatter(const toml::node& source, format_flags flags = default_flags) noexcept
			: base{ &source, nullptr, constants, { flags, "    "sv } }
		{}

#if TOML_DOXYGEN || (TOML_ENABLE_PARSER && !TOML_EXCEPTIONS)

		TOML_NODISCARD_CTOR
		explicit toml_formatter(const toml::parse_result& result, format_flags flags = default_flags) noexcept
			: base{ nullptr, &result, constants, { flags, "    "sv } }
		{}

#endif

		friend std::ostream& operator<<(std::ostream& lhs, toml_formatter& rhs)
		{
			rhs.attach(lhs);
			rhs.key_path_.clear();
			rhs.print();
			rhs.detach();
			return lhs;
		}

		friend std::ostream& operator<<(std::ostream& lhs, toml_formatter&& rhs)
		{
			return lhs << rhs;  
		}
	};
}
TOML_NAMESPACE_END;

#include "header_end.h"
#endif  
