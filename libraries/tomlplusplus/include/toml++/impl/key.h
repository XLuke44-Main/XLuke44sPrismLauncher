//# This file is a part of toml++ and is subject to the the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT
#pragma once

#include "source_region.h"
#include "std_utility.h"
#include "print_to_stream.h"
#include "header_start.h"

TOML_NAMESPACE_START
{

	class key
	{
	  private:
		std::string key_;
		source_region source_;

	  public:

		TOML_NODISCARD_CTOR
		key() noexcept = default;

		TOML_NODISCARD_CTOR
		explicit key(std::string_view k, source_region&& src = {})  
			: key_{ k },
			  source_{ std::move(src) }
		{}

		TOML_NODISCARD_CTOR
		explicit key(std::string_view k, const source_region& src)  
			: key_{ k },
			  source_{ src }
		{}

		TOML_NODISCARD_CTOR
		explicit key(std::string&& k, source_region&& src = {}) noexcept  
			: key_{ std::move(k) },
			  source_{ std::move(src) }
		{}

		TOML_NODISCARD_CTOR
		explicit key(std::string&& k, const source_region& src) noexcept  
			: key_{ std::move(k) },
			  source_{ src }
		{}

		TOML_NODISCARD_CTOR
		explicit key(const char* k, source_region&& src = {})  
			: key_{ k },
			  source_{ std::move(src) }
		{}

		TOML_NODISCARD_CTOR
		explicit key(const char* k, const source_region& src)  
			: key_{ k },
			  source_{ src }
		{}

#if TOML_ENABLE_WINDOWS_COMPAT

		TOML_NODISCARD_CTOR
		explicit key(std::wstring_view k, source_region&& src = {})  
			: key_{ impl::narrow(k) },
			  source_{ std::move(src) }
		{}

		TOML_NODISCARD_CTOR
		explicit key(std::wstring_view k, const source_region& src)  
			: key_{ impl::narrow(k) },
			  source_{ src }
		{}

#endif

		TOML_PURE_INLINE_GETTER
		std::string_view str() const noexcept
		{
			return std::string_view{ key_ };
		}

		TOML_PURE_INLINE_GETTER
		  operator std::string_view() const noexcept
		{
			return str();
		}

		TOML_PURE_INLINE_GETTER
		bool empty() const noexcept
		{
			return key_.empty();
		}

		TOML_PURE_INLINE_GETTER
		const char* data() const noexcept
		{
			return key_.data();
		}

		TOML_PURE_INLINE_GETTER
		size_t length() const noexcept
		{
			return key_.length();
		}

		TOML_PURE_INLINE_GETTER
		const source_region& source() const noexcept
		{
			return source_;
		}

		TOML_PURE_INLINE_GETTER
		friend bool operator==(const key& lhs, const key& rhs) noexcept
		{
			return lhs.key_ == rhs.key_;
		}

		TOML_PURE_INLINE_GETTER
		friend bool operator!=(const key& lhs, const key& rhs) noexcept
		{
			return lhs.key_ != rhs.key_;
		}

		TOML_PURE_INLINE_GETTER
		friend bool operator<(const key& lhs, const key& rhs) noexcept
		{
			return lhs.key_ < rhs.key_;
		}

		TOML_PURE_INLINE_GETTER
		friend bool operator<=(const key& lhs, const key& rhs) noexcept
		{
			return lhs.key_ <= rhs.key_;
		}

		TOML_PURE_INLINE_GETTER
		friend bool operator>(const key& lhs, const key& rhs) noexcept
		{
			return lhs.key_ > rhs.key_;
		}

		TOML_PURE_INLINE_GETTER
		friend bool operator>=(const key& lhs, const key& rhs) noexcept
		{
			return lhs.key_ >= rhs.key_;
		}

		TOML_PURE_INLINE_GETTER
		friend bool operator==(const key& lhs, std::string_view rhs) noexcept
		{
			return lhs.key_ == rhs;
		}

		TOML_PURE_INLINE_GETTER
		friend bool operator!=(const key& lhs, std::string_view rhs) noexcept
		{
			return lhs.key_ != rhs;
		}

		TOML_PURE_INLINE_GETTER
		friend bool operator<(const key& lhs, std::string_view rhs) noexcept
		{
			return lhs.key_ < rhs;
		}

		TOML_PURE_INLINE_GETTER
		friend bool operator<=(const key& lhs, std::string_view rhs) noexcept
		{
			return lhs.key_ <= rhs;
		}

		TOML_PURE_INLINE_GETTER
		friend bool operator>(const key& lhs, std::string_view rhs) noexcept
		{
			return lhs.key_ > rhs;
		}

		TOML_PURE_INLINE_GETTER
		friend bool operator>=(const key& lhs, std::string_view rhs) noexcept
		{
			return lhs.key_ >= rhs;
		}

		TOML_PURE_INLINE_GETTER
		friend bool operator==(std::string_view lhs, const key& rhs) noexcept
		{
			return lhs == rhs.key_;
		}

		TOML_PURE_INLINE_GETTER
		friend bool operator!=(std::string_view lhs, const key& rhs) noexcept
		{
			return lhs != rhs.key_;
		}

		TOML_PURE_INLINE_GETTER
		friend bool operator<(std::string_view lhs, const key& rhs) noexcept
		{
			return lhs < rhs.key_;
		}

		TOML_PURE_INLINE_GETTER
		friend bool operator<=(std::string_view lhs, const key& rhs) noexcept
		{
			return lhs <= rhs.key_;
		}

		TOML_PURE_INLINE_GETTER
		friend bool operator>(std::string_view lhs, const key& rhs) noexcept
		{
			return lhs > rhs.key_;
		}

		TOML_PURE_INLINE_GETTER
		friend bool operator>=(std::string_view lhs, const key& rhs) noexcept
		{
			return lhs >= rhs.key_;
		}

		using const_iterator = const char*;

		using iterator = const_iterator;

		TOML_PURE_INLINE_GETTER
		const_iterator begin() const noexcept
		{
			return key_.data();
		}

		TOML_PURE_INLINE_GETTER
		const_iterator end() const noexcept
		{
			return key_.data() + key_.length();
		}

		friend std::ostream& operator<<(std::ostream& lhs, const key& rhs)
		{
			impl::print_to_stream(lhs, rhs.key_);
			return lhs;
		}
	};

	template <typename T>
	inline constexpr bool is_key = std::is_same_v<impl::remove_cvref<T>, toml::key>;

	template <typename T>
	inline constexpr bool is_key_or_convertible = is_key<T>  
											   || impl::is_constructible_or_convertible<toml::key, T>;
}
TOML_NAMESPACE_END;

#include "header_end.h"
