//# This file is a part of toml++ and is subject to the the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT
#pragma once

#include "std_vector.h"
#include "std_initializer_list.h"
#include "print_to_stream.h"
#include "node.h"
#include "header_start.h"
TOML_DISABLE_ARITHMETIC_WARNINGS;

TOML_NAMESPACE_START
{

	/// \detail A node_view is like a std::optional<toml::node&> (if such a construct were legal), with lots of

	template <typename ViewedType>
	class TOML_TRIVIAL_ABI node_view
	{
		static_assert(impl::is_one_of<ViewedType, toml::node, const toml::node>,
					  "A toml::node_view<> must wrap toml::node or const toml::node.");

	  public:

		using viewed_type = ViewedType;

	  private:
		template <typename T>
		friend class node_view;

		mutable viewed_type* node_ = nullptr;

	  public:

		TOML_NODISCARD_CTOR
		node_view() noexcept = default;

		TOML_NODISCARD_CTOR
		explicit node_view(viewed_type* node) noexcept  
			: node_{ node }
		{}

		TOML_NODISCARD_CTOR
		explicit node_view(viewed_type& node) noexcept  
			: node_{ &node }
		{}

		TOML_NODISCARD_CTOR
		node_view(const node_view&) noexcept = default;

		TOML_NODISCARD_CTOR
		node_view(node_view&&) noexcept = default;

		node_view& operator=(const node_view&) & noexcept = default;

		node_view& operator=(node_view&&) & noexcept = default;

		TOML_PURE_INLINE_GETTER
		explicit operator bool() const noexcept
		{
			return node_ != nullptr;
		}

		TOML_PURE_INLINE_GETTER
		viewed_type* node() const noexcept
		{
			return node_;
		}

		TOML_PURE_GETTER
		node_type type() const noexcept
		{
			return node_ ? node_->type() : node_type::none;
		}

		TOML_PURE_GETTER
		bool is_table() const noexcept
		{
			return node_ && node_->is_table();
		}

		TOML_PURE_GETTER
		bool is_array() const noexcept
		{
			return node_ && node_->is_array();
		}

		TOML_PURE_GETTER
		bool is_value() const noexcept
		{
			return node_ && node_->is_value();
		}

		TOML_PURE_GETTER
		bool is_string() const noexcept
		{
			return node_ && node_->is_string();
		}

		TOML_PURE_GETTER
		bool is_integer() const noexcept
		{
			return node_ && node_->is_integer();
		}

		TOML_PURE_GETTER
		bool is_floating_point() const noexcept
		{
			return node_ && node_->is_floating_point();
		}

		TOML_PURE_GETTER
		bool is_number() const noexcept
		{
			return node_ && node_->is_number();
		}

		TOML_PURE_GETTER
		bool is_boolean() const noexcept
		{
			return node_ && node_->is_boolean();
		}

		TOML_PURE_GETTER
		bool is_date() const noexcept
		{
			return node_ && node_->is_date();
		}

		TOML_PURE_GETTER
		bool is_time() const noexcept
		{
			return node_ && node_->is_time();
		}

		TOML_PURE_GETTER
		bool is_date_time() const noexcept
		{
			return node_ && node_->is_date_time();
		}

		TOML_PURE_GETTER
		bool is_array_of_tables() const noexcept
		{
			return node_ && node_->is_array_of_tables();
		}

		template <typename T>
		TOML_PURE_GETTER
		bool is() const noexcept
		{
			return node_ ? node_->template is<impl::unwrap_node<impl::remove_cvref<T>>>() : false;
		}

		TOML_NODISCARD
		bool is_homogeneous(node_type ntype, viewed_type*& first_nonmatch) const noexcept
		{
			if (!node_)
			{
				first_nonmatch = {};
				return false;
			}
			return node_->is_homogeneous(ntype, first_nonmatch);
		}

		TOML_NODISCARD
		bool is_homogeneous(node_type ntype) const noexcept
		{
			return node_ ? node_->is_homogeneous(ntype) : false;
		}

		template <typename ElemType = void>
		TOML_PURE_GETTER
		bool is_homogeneous() const noexcept
		{
			return node_ ? node_->template is_homogeneous<impl::unwrap_node<impl::remove_cvref<ElemType>>>() : false;
		}

		template <typename T>
		TOML_PURE_GETTER
		auto* as() const noexcept
		{
			return node_ ? node_->template as<T>() : nullptr;
		}

		TOML_PURE_GETTER
		auto* as_table() const noexcept
		{
			return as<table>();
		}

		TOML_PURE_GETTER
		auto* as_array() const noexcept
		{
			return as<array>();
		}

		TOML_PURE_GETTER
		auto* as_string() const noexcept
		{
			return as<std::string>();
		}

		TOML_PURE_GETTER
		auto* as_integer() const noexcept
		{
			return as<int64_t>();
		}

		TOML_PURE_GETTER
		auto* as_floating_point() const noexcept
		{
			return as<double>();
		}

		TOML_PURE_GETTER
		auto* as_boolean() const noexcept
		{
			return as<bool>();
		}

		TOML_PURE_GETTER
		auto* as_date() const noexcept
		{
			return as<date>();
		}

		TOML_PURE_GETTER
		auto* as_time() const noexcept
		{
			return as<time>();
		}

		TOML_PURE_GETTER
		auto* as_date_time() const noexcept
		{
			return as<date_time>();
		}

		template <typename T>
		TOML_NODISCARD
		optional<T> value_exact() const noexcept(impl::value_retrieval_is_nothrow<T>)
		{
			if (node_)
				return node_->template value_exact<T>();
			return {};
		}

		template <typename T>
		TOML_NODISCARD
		optional<T> value() const noexcept(impl::value_retrieval_is_nothrow<T>)
		{
			if (node_)
				return node_->template value<T>();
			return {};
		}

		template <typename T>
		TOML_NODISCARD
		auto value_or(T&& default_value) const noexcept(impl::value_retrieval_is_nothrow<T>)
		{
			using namespace ::toml::impl;

			static_assert(!is_wide_string<T> || TOML_ENABLE_WINDOWS_COMPAT,
						  "Retrieving values as wide-character strings is only "
						  "supported on Windows with TOML_ENABLE_WINDOWS_COMPAT enabled.");

			if constexpr (is_wide_string<T>)
			{
#if TOML_ENABLE_WINDOWS_COMPAT

				if (node_)
					return node_->value_or(static_cast<T&&>(default_value));
				return std::wstring{ static_cast<T&&>(default_value) };

#else

				static_assert(impl::dependent_false<T>, "Evaluated unreachable branch!");

#endif
			}
			else
			{
				using value_type =
					std::conditional_t<std::is_pointer_v<std::decay_t<T>>,
									   std::add_pointer_t<std::add_const_t<std::remove_pointer_t<std::decay_t<T>>>>,
									   std::decay_t<T>>;

				if (node_)
					return node_->value_or(static_cast<T&&>(default_value));
				if constexpr (std::is_pointer_v<value_type>)
					return value_type{ default_value };
				else
					return static_cast<T&&>(default_value);
			}
		}

		template <typename T>
		TOML_PURE_INLINE_GETTER
		decltype(auto) ref() const noexcept
		{
			TOML_ASSERT_ASSUME(node_ && "toml::node_view::ref() called on a node_view that did not reference a node");
			return node_->template ref<T>();
		}

	  private:

		template <typename Func>
		static constexpr bool visit_is_nothrow = noexcept(std::declval<viewed_type*>()->visit(std::declval<Func>()));

	  public:

		template <typename Func>
		decltype(auto) visit(Func&& visitor) const noexcept(visit_is_nothrow<Func&&>)
		{
			using return_type = decltype(node_->visit(static_cast<Func&&>(visitor)));
			if (node_)
				return node_->visit(static_cast<Func&&>(visitor));
			if constexpr (!std::is_void_v<return_type>)
				return return_type{};
		}

	  public:

		template <typename T>
		TOML_PURE_GETTER
		friend bool operator==(const node_view& lhs, const node_view<T>& rhs) noexcept
		{
			return impl::node_deep_equality(lhs.node_, rhs.node_);
		}

		template <typename T>
		TOML_PURE_GETTER
		friend bool operator!=(const node_view& lhs, const node_view<T>& rhs) noexcept
		{
			return !impl::node_deep_equality(lhs.node_, rhs.node_);
		}

		TOML_NODISCARD
		friend bool operator==(const node_view& lhs, const table& rhs) noexcept
		{
			if (lhs.node_ == &rhs)
				return true;
			const auto tbl = lhs.as<table>();
			return tbl && *tbl == rhs;
		}
		TOML_ASYMMETRICAL_EQUALITY_OPS(const node_view&, const table&, );

		TOML_NODISCARD
		friend bool operator==(const node_view& lhs, const array& rhs) noexcept
		{
			if (lhs.node_ == &rhs)
				return true;
			const auto arr = lhs.as<array>();
			return arr && *arr == rhs;
		}
		TOML_ASYMMETRICAL_EQUALITY_OPS(const node_view&, const array&, );

		template <typename T>
		TOML_NODISCARD
		friend bool operator==(const node_view& lhs, const toml::value<T>& rhs) noexcept
		{
			if (lhs.node_ == &rhs)
				return true;
			const auto val = lhs.as<T>();
			return val && *val == rhs;
		}
		TOML_ASYMMETRICAL_EQUALITY_OPS(const node_view&, const toml::value<T>&, template <typename T>);

		TOML_CONSTRAINED_TEMPLATE(impl::is_losslessly_convertible_to_native<T>, typename T)
		TOML_NODISCARD
		friend bool operator==(const node_view& lhs, const T& rhs) noexcept(!impl::is_wide_string<T>)
		{
			static_assert(!impl::is_wide_string<T> || TOML_ENABLE_WINDOWS_COMPAT,
						  "Comparison with wide-character strings is only "
						  "supported on Windows with TOML_ENABLE_WINDOWS_COMPAT enabled.");

			if constexpr (impl::is_wide_string<T>)
			{
#if TOML_ENABLE_WINDOWS_COMPAT
				return lhs == impl::narrow(rhs);
#else
				static_assert(impl::dependent_false<T>, "Evaluated unreachable branch!");
#endif
			}
			else
			{
				const auto val = lhs.as<impl::native_type_of<T>>();
				return val && *val == rhs;
			}
		}
		TOML_ASYMMETRICAL_EQUALITY_OPS(const node_view&,
									   const T&,
									   TOML_CONSTRAINED_TEMPLATE(impl::is_losslessly_convertible_to_native<T>,
																 typename T));

		template <typename T>
		TOML_NODISCARD
		friend bool operator==(const node_view& lhs,
							   const std::initializer_list<T>& rhs) noexcept(!impl::is_wide_string<T>)
		{
			const auto arr = lhs.as<array>();
			return arr && *arr == rhs;
		}
		TOML_ASYMMETRICAL_EQUALITY_OPS(const node_view&, const std::initializer_list<T>&, template <typename T>);

		template <typename T>
		TOML_NODISCARD
		friend bool operator==(const node_view& lhs, const std::vector<T>& rhs) noexcept(!impl::is_wide_string<T>)
		{
			const auto arr = lhs.as<array>();
			return arr && *arr == rhs;
		}
		TOML_ASYMMETRICAL_EQUALITY_OPS(const node_view&, const std::vector<T>&, template <typename T>);

		TOML_NODISCARD
		node_view operator[](std::string_view key) const noexcept
		{
			if (auto tbl = this->as_table())
				return node_view{ tbl->get(key) };
			return {};
		}

		TOML_NODISCARD
		node_view operator[](const toml::path& path) const noexcept
		{
			return node_ ? node_->at_path(path) : node_view{};
		}

		TOML_NODISCARD
		node_view at_path(std::string_view path) const noexcept
		{
			return node_ ? node_->at_path(path) : node_view{};
		}

		TOML_NODISCARD
		node_view at_path(const toml::path& path) const noexcept
		{
			return node_ ? node_->at_path(path) : node_view{};
		}

#if TOML_ENABLE_WINDOWS_COMPAT

		TOML_NODISCARD
		node_view operator[](std::wstring_view key) const
		{
			if (auto tbl = this->as_table())
				return node_view{ tbl->get(key) };
			return {};
		}

		TOML_NODISCARD
		node_view at_path(std::wstring_view path) const
		{
			return node_ ? node_->at_path(path) : node_view{};
		}

#endif  

		TOML_NODISCARD
		node_view operator[](size_t index) const noexcept
		{
			if (auto arr = this->as_array())
				return node_view{ arr->get(index) };
			return {};
		}

#if TOML_ENABLE_FORMATTERS

		friend std::ostream& operator<<(std::ostream& os, const node_view& nv)
		{
			if (nv.node_)
				nv.node_->visit([&os](const auto& n) { os << n; });
			return os;
		}

#endif
	};

	template <typename T>
	node_view(const T&) -> node_view<const node>;

	template <typename T>
	node_view(const T*) -> node_view<const node>;

	template <typename T>
	node_view(T&) -> node_view<node>;

	template <typename T>
	node_view(T*) -> node_view<node>;

}
TOML_NAMESPACE_END;

TOML_NAMESPACE_START
{
	inline node::operator node_view<node>() noexcept
	{
		return node_view<node>{ this };
	}

	inline node::operator node_view<const node>() const noexcept
	{
		return node_view<const node>{ this };
	}
}
TOML_NAMESPACE_END;

#include "header_end.h"
