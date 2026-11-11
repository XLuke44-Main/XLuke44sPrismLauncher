//# This file is a part of toml++ and is subject to the the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT
#pragma once

#include "preprocessor.h"
#if TOML_DOXYGEN || (TOML_ENABLE_PARSER && !TOML_EXCEPTIONS)

#include "table.h"
#include "parse_error.h"
#include "header_start.h"

TOML_NAMESPACE_START
{
	TOML_ABI_NAMESPACE_START(noex);

	class parse_result
	{
	  private:
		struct storage_t
		{
			static constexpr size_t size =
				(sizeof(toml::table) < sizeof(parse_error) ? sizeof(parse_error) : sizeof(toml::table));
			static constexpr size_t align =
				(alignof(toml::table) < alignof(parse_error) ? alignof(parse_error) : alignof(toml::table));

			alignas(align) unsigned char bytes[size];
		};

		alignas(storage_t::align) mutable storage_t storage_;
		bool err_;

		template <typename Type>
		TOML_NODISCARD
		TOML_ALWAYS_INLINE
		static Type* get_as(storage_t& s) noexcept
		{
			return TOML_LAUNDER(reinterpret_cast<Type*>(s.bytes));
		}

		void destroy() noexcept
		{
			if (err_)
				get_as<parse_error>(storage_)->~parse_error();
			else
				get_as<toml::table>(storage_)->~table();
		}

	  public:

		TOML_NODISCARD_CTOR
		parse_result() noexcept  
			: err_{ true }
		{
			::new (static_cast<void*>(storage_.bytes)) parse_error{ std::string{}, source_region{} };
		}

		TOML_NODISCARD_CTOR
		explicit parse_result(toml::table&& tbl) noexcept  
			: err_{ false }
		{
			::new (static_cast<void*>(storage_.bytes)) toml::table{ std::move(tbl) };
		}

		TOML_NODISCARD_CTOR
		explicit parse_result(parse_error&& err) noexcept  
			: err_{ true }
		{
			::new (static_cast<void*>(storage_.bytes)) parse_error{ std::move(err) };
		}

		TOML_NODISCARD_CTOR
		parse_result(parse_result&& res) noexcept  
			: err_{ res.err_ }
		{
			if (err_)
				::new (static_cast<void*>(storage_.bytes)) parse_error{ std::move(res).error() };
			else
				::new (static_cast<void*>(storage_.bytes)) toml::table{ std::move(res).table() };
		}

		parse_result& operator=(parse_result&& rhs) noexcept
		{
			if (err_ != rhs.err_)
			{
				destroy();
				err_ = rhs.err_;
				if (err_)
					::new (static_cast<void*>(storage_.bytes)) parse_error{ std::move(rhs).error() };
				else
					::new (static_cast<void*>(storage_.bytes)) toml::table{ std::move(rhs).table() };
			}
			else
			{
				if (err_)
					error() = std::move(rhs).error();
				else
					table() = std::move(rhs).table();
			}
			return *this;
		}

		~parse_result() noexcept
		{
			destroy();
		}

		TOML_NODISCARD
		bool succeeded() const noexcept
		{
			return !err_;
		}

		TOML_NODISCARD
		bool failed() const noexcept
		{
			return err_;
		}

		TOML_NODISCARD
		explicit operator bool() const noexcept
		{
			return !err_;
		}

		TOML_NODISCARD
		toml::table& table() & noexcept
		{
			TOML_ASSERT_ASSUME(!err_);
			return *get_as<toml::table>(storage_);
		}

		TOML_NODISCARD
		toml::table&& table() && noexcept
		{
			TOML_ASSERT_ASSUME(!err_);
			return static_cast<toml::table&&>(*get_as<toml::table>(storage_));
		}

		TOML_NODISCARD
		const toml::table& table() const& noexcept
		{
			TOML_ASSERT_ASSUME(!err_);
			return *get_as<const toml::table>(storage_);
		}

		TOML_NODISCARD
		  operator toml::table&() noexcept
		{
			return table();
		}

		TOML_NODISCARD
		  operator toml::table&&() noexcept
		{
			return std::move(table());
		}

		TOML_NODISCARD
		  operator const toml::table&() const noexcept
		{
			return table();
		}

		TOML_NODISCARD
		parse_error& error() & noexcept
		{
			TOML_ASSERT_ASSUME(err_);
			return *get_as<parse_error>(storage_);
		}

		TOML_NODISCARD
		parse_error&& error() && noexcept
		{
			TOML_ASSERT_ASSUME(err_);
			return static_cast<parse_error&&>(*get_as<parse_error>(storage_));
		}

		TOML_NODISCARD
		const parse_error& error() const& noexcept
		{
			TOML_ASSERT_ASSUME(err_);
			return *get_as<const parse_error>(storage_);
		}

		TOML_NODISCARD
		explicit operator parse_error&() noexcept
		{
			return error();
		}

		TOML_NODISCARD
		explicit operator parse_error&&() noexcept
		{
			return std::move(error());
		}

		TOML_NODISCARD
		explicit operator const parse_error&() const noexcept
		{
			return error();
		}

		using iterator = table_iterator;

		using const_iterator = const_table_iterator;

		TOML_NODISCARD
		table_iterator begin() noexcept
		{
			return err_ ? table_iterator{} : table().begin();
		}

		TOML_NODISCARD
		const_table_iterator begin() const noexcept
		{
			return err_ ? const_table_iterator{} : table().begin();
		}

		TOML_NODISCARD
		const_table_iterator cbegin() const noexcept
		{
			return err_ ? const_table_iterator{} : table().cbegin();
		}

		TOML_NODISCARD
		table_iterator end() noexcept
		{
			return err_ ? table_iterator{} : table().end();
		}

		TOML_NODISCARD
		const_table_iterator end() const noexcept
		{
			return err_ ? const_table_iterator{} : table().end();
		}

		TOML_NODISCARD
		const_table_iterator cend() const noexcept
		{
			return err_ ? const_table_iterator{} : table().cend();
		}

		TOML_NODISCARD
		node_view<node> at_path(std::string_view path) noexcept
		{
			return err_ ? node_view<node>{} : table().at_path(path);
		}

		TOML_NODISCARD
		node_view<const node> at_path(std::string_view path) const noexcept
		{
			return err_ ? node_view<const node>{} : table().at_path(path);
		}

		TOML_NODISCARD
		node_view<node> at_path(const toml::path& path) noexcept
		{
			return err_ ? node_view<node>{} : table().at_path(path);
		}

		TOML_NODISCARD
		node_view<const node> at_path(const toml::path& path) const noexcept
		{
			return err_ ? node_view<const node>{} : table().at_path(path);
		}

#if TOML_ENABLE_WINDOWS_COMPAT

		TOML_NODISCARD
		node_view<node> at_path(std::wstring_view path)
		{
			return err_ ? node_view<node>{} : table().at_path(path);
		}

		TOML_NODISCARD
		node_view<const node> at_path(std::wstring_view path) const
		{
			return err_ ? node_view<const node>{} : table().at_path(path);
		}

#endif

		TOML_NODISCARD
		node_view<node> operator[](const toml::path& path) noexcept
		{
			return err_ ? node_view<node>{} : table()[path];
		}

		TOML_NODISCARD
		node_view<const node> operator[](const toml::path& path) const noexcept
		{
			return err_ ? node_view<const node>{} : table()[path];
		}

		TOML_NODISCARD
		node_view<node> operator[](std::string_view key) noexcept
		{
			return err_ ? node_view<node>{} : table()[key];
		}

		TOML_NODISCARD
		node_view<const node> operator[](std::string_view key) const noexcept
		{
			return err_ ? node_view<const node>{} : table()[key];
		}

#if TOML_ENABLE_WINDOWS_COMPAT

		TOML_NODISCARD
		node_view<node> operator[](std::wstring_view key)
		{
			return err_ ? node_view<node>{} : table()[key];
		}

		TOML_NODISCARD
		node_view<const node> operator[](std::wstring_view key) const
		{
			return err_ ? node_view<const node>{} : table()[key];
		}

#endif  

#if TOML_ENABLE_FORMATTERS

		friend std::ostream& operator<<(std::ostream& os, const parse_result& result)
		{
			return result.err_ ? (os << result.error()) : (os << result.table());
		}

#endif
	};

	TOML_ABI_NAMESPACE_END;
}
TOML_NAMESPACE_END;

#include "header_end.h"
#endif  
