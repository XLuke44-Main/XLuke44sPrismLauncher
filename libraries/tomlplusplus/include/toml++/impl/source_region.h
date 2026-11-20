//# This file is a part of toml++ and is subject to the the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT
#pragma once

#include "std_optional.h"
#include "std_string.h"
#include "forward_declarations.h"
#include "print_to_stream.h"
#include "header_start.h"

TOML_NAMESPACE_START
{

	using source_index = uint32_t;

	using source_path_ptr = std::shared_ptr<const std::string>;

	struct TOML_TRIVIAL_ABI source_position
	{

		source_index line;

		source_index column;

		TOML_PURE_GETTER
		explicit constexpr operator bool() const noexcept
		{
			return line > source_index{}  
				&& column > source_index{};
		}

		TOML_PURE_GETTER
		friend constexpr bool operator==(const source_position& lhs, const source_position& rhs) noexcept
		{
			return lhs.line == rhs.line  
				&& lhs.column == rhs.column;
		}

		TOML_PURE_INLINE_GETTER
		friend constexpr bool operator!=(const source_position& lhs, const source_position& rhs) noexcept
		{
			return !(lhs == rhs);
		}

	  private:

		TOML_PURE_GETTER
		static constexpr uint64_t pack(const source_position& pos) noexcept
		{
			return static_cast<uint64_t>(pos.line) << 32 | static_cast<uint64_t>(pos.column);
		}

	  public:

		TOML_PURE_GETTER
		friend constexpr bool operator<(const source_position& lhs, const source_position& rhs) noexcept
		{
			return pack(lhs) < pack(rhs);
		}

		TOML_PURE_GETTER
		friend constexpr bool operator<=(const source_position& lhs, const source_position& rhs) noexcept
		{
			return pack(lhs) <= pack(rhs);
		}

		TOML_PURE_GETTER
		friend constexpr bool operator>(const source_position& lhs, const source_position& rhs) noexcept
		{
			return pack(lhs) > pack(rhs);
		}

		TOML_PURE_GETTER
		friend constexpr bool operator>=(const source_position& lhs, const source_position& rhs) noexcept
		{
			return pack(lhs) >= pack(rhs);
		}

		friend std::ostream& operator<<(std::ostream& lhs, const source_position& rhs)
		{
			impl::print_to_stream(lhs, rhs);
			return lhs;
		}
	};

	struct source_region
	{

		source_position begin;

		source_position end;

		source_path_ptr path;

#if TOML_ENABLE_WINDOWS_COMPAT

		TOML_NODISCARD
		optional<std::wstring> wide_path() const
		{
			if (!path || path->empty())
				return {};
			return { impl::widen(*path) };
		}

#endif

		friend std::ostream& operator<<(std::ostream& lhs, const source_region& rhs)
		{
			impl::print_to_stream(lhs, rhs);
			return lhs;
		}
	};
}
TOML_NAMESPACE_END;

#include "header_end.h"
