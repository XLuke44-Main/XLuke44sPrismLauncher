//# This file is a part of toml++ and is subject to the the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT
#pragma once

#include "preprocessor.h"
#if TOML_ENABLE_PARSER

#include "std_except.h"
#include "source_region.h"
#include "print_to_stream.h"
#include "header_start.h"

#if TOML_DOXYGEN || !TOML_EXCEPTIONS
#define TOML_PARSE_ERROR_BASE
#else
#define TOML_PARSE_ERROR_BASE	 : public std::runtime_error
#endif

TOML_NAMESPACE_START
{
	TOML_ABI_NAMESPACE_BOOL(TOML_EXCEPTIONS, ex, noex);

	class parse_error TOML_PARSE_ERROR_BASE
	{
	  private:
#if !TOML_EXCEPTIONS
		std::string description_;
#endif
		source_region source_;

	  public:
#if TOML_EXCEPTIONS

		TOML_NODISCARD_CTOR
		TOML_ATTR(nonnull)
		parse_error(const char* desc, source_region&& src) noexcept  
			: std::runtime_error{ desc },
			  source_{ std::move(src) }
		{}

		TOML_NODISCARD_CTOR
		TOML_ATTR(nonnull)
		parse_error(const char* desc, const source_region& src) noexcept  
			: parse_error{ desc, source_region{ src } }
		{}

		TOML_NODISCARD_CTOR
		TOML_ATTR(nonnull)
		parse_error(const char* desc, const source_position& position, const source_path_ptr& path = {}) noexcept
			: parse_error{ desc, source_region{ position, position, path } }
		{}

#else

		TOML_NODISCARD_CTOR
		parse_error(std::string&& desc, source_region&& src) noexcept  
			: description_{ std::move(desc) },
			  source_{ std::move(src) }
		{}

		TOML_NODISCARD_CTOR
		parse_error(std::string&& desc, const source_region& src) noexcept  
			: parse_error{ std::move(desc), source_region{ src } }
		{}

		TOML_NODISCARD_CTOR
		parse_error(std::string&& desc, const source_position& position, const source_path_ptr& path = {}) noexcept
			: parse_error{ std::move(desc), source_region{ position, position, path } }
		{}

#endif

		TOML_NODISCARD
		std::string_view description() const noexcept
		{
#if TOML_EXCEPTIONS
			return std::string_view{ what() };
#else
			return description_;
#endif
		}

		TOML_NODISCARD
		const source_region& source() const noexcept
		{
			return source_;
		}

		friend std::ostream& operator<<(std::ostream& lhs, const parse_error& rhs)
		{
			impl::print_to_stream(lhs, rhs.description());
			impl::print_to_stream(lhs, "\n\t(error occurred at "sv);
			impl::print_to_stream(lhs, rhs.source());
			impl::print_to_stream(lhs, ")"sv);
			return lhs;
		}
	};

	TOML_ABI_NAMESPACE_END;  
}
TOML_NAMESPACE_END;

#undef TOML_PARSE_ERROR_BASE

#include "header_end.h"
#endif  
