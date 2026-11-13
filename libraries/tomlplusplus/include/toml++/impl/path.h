//# This file is a part of toml++ and is subject to the the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT
#pragma once

#include "forward_declarations.h"
#include "std_vector.h"
#include "header_start.h"

TOML_NAMESPACE_START
{

	enum class TOML_CLOSED_ENUM path_component_type : uint8_t
	{
		key			= 0x1,
		array_index = 0x2
	};

	class TOML_EXPORTED_CLASS path_component
	{

		struct storage_t
		{
			static constexpr size_t size =
				(sizeof(size_t) < sizeof(std::string) ? sizeof(std::string) : sizeof(size_t));
			static constexpr size_t align =
				(alignof(size_t) < alignof(std::string) ? alignof(std::string) : alignof(size_t));

			alignas(align) unsigned char bytes[size];
		};
		alignas(storage_t::align) mutable storage_t value_storage_;

		path_component_type type_;

		TOML_PURE_GETTER
		TOML_EXPORTED_STATIC_FUNCTION
		static bool TOML_CALLCONV equal(const path_component&, const path_component&) noexcept;

		template <typename Type>
		TOML_PURE_INLINE_GETTER
		static Type* get_as(storage_t& s) noexcept
		{
			return TOML_LAUNDER(reinterpret_cast<Type*>(s.bytes));
		}

		static void store_key(std::string_view key, storage_t& storage_)
		{
			::new (static_cast<void*>(storage_.bytes)) std::string{ key };
		}

		static void store_index(size_t index, storage_t& storage_) noexcept
		{
			::new (static_cast<void*>(storage_.bytes)) std::size_t{ index };
		}

		void destroy() noexcept
		{
			if (type_ == path_component_type::key)
				get_as<std::string>(value_storage_)->~basic_string();
		}

		TOML_NODISCARD
		size_t& index_ref() noexcept
		{
			TOML_ASSERT_ASSUME(type_ == path_component_type::array_index);
			return *get_as<size_t>(value_storage_);
		}

		TOML_NODISCARD
		std::string& key_ref() noexcept
		{
			TOML_ASSERT_ASSUME(type_ == path_component_type::key);
			return *get_as<std::string>(value_storage_);
		}

	  public:

		TOML_NODISCARD_CTOR
		TOML_EXPORTED_MEMBER_FUNCTION
		path_component();

		TOML_NODISCARD_CTOR
		TOML_EXPORTED_MEMBER_FUNCTION
		path_component(size_t index) noexcept;

		TOML_NODISCARD_CTOR
		TOML_EXPORTED_MEMBER_FUNCTION
		path_component(std::string_view key);

#if TOML_ENABLE_WINDOWS_COMPAT

		TOML_NODISCARD_CTOR
		TOML_EXPORTED_MEMBER_FUNCTION
		path_component(std::wstring_view key);

#endif

		TOML_NODISCARD_CTOR
		TOML_EXPORTED_MEMBER_FUNCTION
		path_component(const path_component& pc);

		TOML_NODISCARD_CTOR
		TOML_EXPORTED_MEMBER_FUNCTION
		path_component(path_component&& pc) noexcept;

		TOML_EXPORTED_MEMBER_FUNCTION
		path_component& operator=(const path_component& rhs);

		TOML_EXPORTED_MEMBER_FUNCTION
		path_component& operator=(path_component&& rhs) noexcept;

		TOML_EXPORTED_MEMBER_FUNCTION
		path_component& operator=(size_t new_index) noexcept;

		TOML_EXPORTED_MEMBER_FUNCTION
		path_component& operator=(std::string_view new_key);

#if TOML_ENABLE_WINDOWS_COMPAT

		TOML_EXPORTED_MEMBER_FUNCTION
		path_component& operator=(std::wstring_view new_key);

#endif

		~path_component() noexcept
		{
			destroy();
		}

		TOML_PURE_GETTER
		size_t index() const noexcept
		{
			TOML_ASSERT_ASSUME(type_ == path_component_type::array_index);
			return *get_as<const size_t>(value_storage_);
		}

		TOML_PURE_INLINE_GETTER
		explicit operator size_t() const noexcept
		{
			return index();
		}

		TOML_PURE_GETTER
		const std::string& key() const noexcept
		{
			TOML_ASSERT_ASSUME(type_ == path_component_type::key);
			return *get_as<const std::string>(value_storage_);
		}

		TOML_PURE_INLINE_GETTER
		explicit operator const std::string&() const noexcept
		{
			return key();
		}

		TOML_PURE_INLINE_GETTER
		path_component_type type() const noexcept
		{
			return type_;
		}

		TOML_PURE_INLINE_GETTER
		friend bool operator==(const path_component& lhs, const path_component& rhs) noexcept
		{
			return equal(lhs, rhs);
		}

		TOML_PURE_INLINE_GETTER
		friend bool operator!=(const path_component& lhs, const path_component& rhs) noexcept
		{
			return !equal(lhs, rhs);
		}

	};

	class TOML_EXPORTED_CLASS path
	{
	  private:

		std::vector<path_component> components_;

		TOML_EXPORTED_MEMBER_FUNCTION
		void print_to(std::ostream&) const;

		TOML_PURE_GETTER
		TOML_EXPORTED_STATIC_FUNCTION
		static bool TOML_CALLCONV equal(const path&, const path&) noexcept;

	  public:

		TOML_NODISCARD_CTOR
		path() noexcept = default;

		TOML_NODISCARD_CTOR
		TOML_EXPORTED_MEMBER_FUNCTION
		explicit path(std::string_view);

#if TOML_ENABLE_WINDOWS_COMPAT

		TOML_NODISCARD_CTOR
		TOML_EXPORTED_MEMBER_FUNCTION
		explicit path(std::wstring_view);

#endif

		~path() noexcept = default;

		TOML_NODISCARD_CTOR
		path(const path&) = default;

		TOML_NODISCARD_CTOR
		path(path&&) noexcept = default;

		TOML_PURE_INLINE_GETTER
		size_t size() const noexcept
		{
			return components_.size();
		}

		TOML_PURE_INLINE_GETTER
		explicit operator bool() const noexcept
		{
			return !components_.empty();
		}

		TOML_PURE_INLINE_GETTER
		bool empty() const noexcept
		{
			return components_.empty();
		}

		TOML_PURE_INLINE_GETTER
		path_component& operator[](size_t index) noexcept
		{
			TOML_ASSERT(index < size());
			return components_[index];
		}

		TOML_PURE_INLINE_GETTER
		const path_component& operator[](size_t index) const noexcept
		{
			TOML_ASSERT(index < size());
			return components_[index];
		}

		path& operator=(const path&) = default;

		path& operator=(path&&) noexcept = default;

		TOML_EXPORTED_MEMBER_FUNCTION
		path& operator=(std::string_view);

#if TOML_ENABLE_WINDOWS_COMPAT

		TOML_EXPORTED_MEMBER_FUNCTION
		path& operator=(std::wstring_view);

#endif

		TOML_ALWAYS_INLINE
		path& assign(const path& p)
		{
			return *this = p;
		}

		TOML_ALWAYS_INLINE
		path& assign(path&& p) noexcept
		{
			return *this = std::move(p);
		}

		TOML_ALWAYS_INLINE
		path& assign(std::string_view str)
		{
			return *this = str;
		}

#if TOML_ENABLE_WINDOWS_COMPAT

		TOML_ALWAYS_INLINE
		path& assign(std::wstring_view str)
		{
			return *this = str;
		}

#endif

		TOML_EXPORTED_MEMBER_FUNCTION
		path& operator+=(const path&);

		TOML_EXPORTED_MEMBER_FUNCTION
		path& operator+=(path&&);

		TOML_EXPORTED_MEMBER_FUNCTION
		path& operator+=(std::string_view);

#if TOML_ENABLE_WINDOWS_COMPAT

		TOML_EXPORTED_MEMBER_FUNCTION
		path& operator+=(std::wstring_view);

#endif

		TOML_ALWAYS_INLINE
		path& append(const path& p)
		{
			return *this += p;
		}

		TOML_ALWAYS_INLINE
		path& append(path&& p)
		{
			return *this += std::move(p);
		}

		TOML_ALWAYS_INLINE
		path& append(std::string_view str)
		{
			return *this += str;
		}

#if TOML_ENABLE_WINDOWS_COMPAT

		TOML_ALWAYS_INLINE
		path& append(std::wstring_view str)
		{
			return *this += str;
		}

#endif

		TOML_EXPORTED_MEMBER_FUNCTION
		path& prepend(const path&);

		TOML_EXPORTED_MEMBER_FUNCTION
		path& prepend(path&&);

		TOML_EXPORTED_MEMBER_FUNCTION
		path& prepend(std::string_view);

#if TOML_ENABLE_WINDOWS_COMPAT

		TOML_EXPORTED_MEMBER_FUNCTION
		path& prepend(std::wstring_view);

#endif

		TOML_NODISCARD
		friend path operator+(const path& lhs, const path& rhs)
		{
			path result = lhs;
			result += rhs;
			return result;
		}

		TOML_NODISCARD
		friend path operator+(const path& lhs, std::string_view rhs)
		{
			path result = lhs;
			result += rhs;
			return result;
		}

		TOML_NODISCARD
		friend path operator+(std::string_view lhs, const path& rhs)
		{
			path result = rhs;
			result.prepend(lhs);
			return result;
		}

#if TOML_ENABLE_WINDOWS_COMPAT

		TOML_NODISCARD
		friend path operator+(const path& lhs, std::wstring_view rhs)
		{
			path result = lhs;
			result += rhs;
			return result;
		}

		TOML_NODISCARD
		friend path operator+(std::wstring_view lhs, const path& rhs)
		{
			path result = rhs;
			result.prepend(lhs);
			return result;
		}

#endif

		TOML_ALWAYS_INLINE
		friend std::ostream& operator<<(std::ostream& os, const path& rhs)
		{
			rhs.print_to(os);
			return os;
		}

		TOML_NODISCARD
		TOML_EXPORTED_MEMBER_FUNCTION
		std::string str() const;

		TOML_NODISCARD
		TOML_ALWAYS_INLINE
		explicit operator std::string() const
		{
			return str();
		}

#if TOML_ENABLE_WINDOWS_COMPAT

		TOML_NODISCARD
		TOML_EXPORTED_MEMBER_FUNCTION
		std::wstring wide_str() const;

		TOML_NODISCARD
		TOML_ALWAYS_INLINE
		explicit operator std::wstring() const
		{
			return wide_str();
		}

#endif

		TOML_PURE_INLINE_GETTER
		friend bool operator==(const path& lhs, const path& rhs) noexcept
		{
			return equal(lhs, rhs);
		}

		TOML_PURE_INLINE_GETTER
		friend bool operator!=(const path& lhs, const path& rhs) noexcept
		{
			return !equal(lhs, rhs);
		}

		TOML_NODISCARD
		TOML_ALWAYS_INLINE
		friend bool operator==(const path& lhs, std::string_view rhs)
		{
			return lhs == path{ rhs };
		}

		TOML_NODISCARD
		TOML_ALWAYS_INLINE
		friend bool operator==(std::string_view lhs, const path& rhs)
		{
			return rhs == lhs;
		}

		TOML_NODISCARD
		TOML_ALWAYS_INLINE
		friend bool operator!=(const path& lhs, std::string_view rhs)
		{
			return lhs != path{ rhs };
		}

		TOML_NODISCARD
		TOML_ALWAYS_INLINE
		friend bool operator!=(std::string_view lhs, const path& rhs)
		{
			return rhs != lhs;
		}

#if TOML_ENABLE_WINDOWS_COMPAT

		TOML_NODISCARD
		TOML_ALWAYS_INLINE
		friend bool operator==(const path& lhs, std::wstring_view rhs)
		{
			return lhs == path{ rhs };
		}

		TOML_NODISCARD
		TOML_ALWAYS_INLINE
		friend bool operator==(std::wstring_view lhs, const path& rhs)
		{
			return rhs == lhs;
		}

		TOML_NODISCARD
		TOML_ALWAYS_INLINE
		friend bool operator!=(const path& lhs, std::wstring_view rhs)
		{
			return lhs != path{ rhs };
		}

		TOML_NODISCARD
		TOML_ALWAYS_INLINE
		friend bool operator!=(std::wstring_view lhs, const path& rhs)
		{
			return rhs != lhs;
		}

#endif  

		using iterator = std::vector<path_component>::iterator;

		using const_iterator = std::vector<path_component>::const_iterator;

		TOML_PURE_INLINE_GETTER
		iterator begin() noexcept
		{
			return components_.begin();
		}

		TOML_PURE_INLINE_GETTER
		iterator end() noexcept
		{
			return components_.end();
		}

		TOML_PURE_INLINE_GETTER
		const_iterator begin() const noexcept
		{
			return components_.begin();
		}

		TOML_PURE_INLINE_GETTER
		const_iterator end() const noexcept
		{
			return components_.end();
		}

		TOML_PURE_INLINE_GETTER
		const_iterator cbegin() const noexcept
		{
			return components_.begin();
		}

		TOML_PURE_INLINE_GETTER
		const_iterator cend() const noexcept
		{
			return components_.end();
		}

		TOML_EXPORTED_MEMBER_FUNCTION
		void clear() noexcept;

		TOML_EXPORTED_MEMBER_FUNCTION
		path& truncate(size_t n);

		TOML_NODISCARD
		TOML_EXPORTED_MEMBER_FUNCTION
		path truncated(size_t n) const;

		TOML_NODISCARD
		TOML_EXPORTED_MEMBER_FUNCTION
		path parent() const;

		TOML_NODISCARD
		TOML_EXPORTED_MEMBER_FUNCTION
		path leaf(size_t n = 1) const;

		TOML_NODISCARD
		TOML_EXPORTED_MEMBER_FUNCTION
		path subpath(const_iterator start, const_iterator end) const;

		TOML_NODISCARD
		TOML_EXPORTED_MEMBER_FUNCTION
		path subpath(size_t start, size_t length) const;

	};

	inline namespace literals
	{

		TOML_NODISCARD
		TOML_ALWAYS_INLINE
		path operator"" _tpath(const char* str, size_t len)
		{
			return path(std::string_view{ str, len });
		}
	}

	TOML_NODISCARD
	TOML_EXPORTED_FREE_FUNCTION
	node_view<node> TOML_CALLCONV at_path(node & root, const toml::path& path) noexcept;

	TOML_NODISCARD
	TOML_EXPORTED_FREE_FUNCTION
	node_view<const node> TOML_CALLCONV at_path(const node& root, const toml::path& path) noexcept;
}
TOML_NAMESPACE_END;

#include "header_end.h"
