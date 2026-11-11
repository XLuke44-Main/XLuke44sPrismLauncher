//# This file is a part of toml++ and is subject to the the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT
#pragma once

#include "forward_declarations.h"
#include "header_start.h"

TOML_IMPL_NAMESPACE_START
{
	template <typename T>
	TOML_NODISCARD
	TOML_ATTR(returns_nonnull)
	auto* make_node_impl_specialized(T && val, [[maybe_unused]] value_flags flags)
	{
		using unwrapped_type = unwrap_node<remove_cvref<T>>;
		static_assert(!std::is_same_v<unwrapped_type, node>);
		static_assert(!is_node_view<unwrapped_type>);

		if constexpr (is_one_of<unwrapped_type, array, table>)
		{
			return new unwrapped_type(static_cast<T&&>(val));
		}

		else
		{
			using native_type = native_type_of<unwrapped_type>;
			using value_type  = value<native_type>;

			value_type* out;

			if constexpr (std::is_same_v<remove_cvref<T>, value_type>)
			{
				out = new value_type{ static_cast<T&&>(val) };
			}

			else
			{
				static_assert(!is_wide_string<T> || TOML_ENABLE_WINDOWS_COMPAT,
							  "Instantiating values from wide-character strings is only "
							  "supported on Windows with TOML_ENABLE_WINDOWS_COMPAT enabled.");

				if constexpr (!is_losslessly_convertible_to_native<unwrapped_type>)
				{
					if constexpr (std::is_same_v<native_type, int64_t>)
						static_assert(dependent_false<T>,
									  "Integral value initializers must be losslessly convertible to int64_t");
					else if constexpr (std::is_same_v<native_type, double>)
						static_assert(dependent_false<T>,
									  "Floating-point value initializers must be losslessly convertible to double");
					else
						static_assert(
							dependent_false<T>,
							"Value initializers must be losslessly convertible to one of the TOML value types");
				}

				if constexpr (is_wide_string<T>)
				{
#if TOML_ENABLE_WINDOWS_COMPAT
					out = new value_type{ narrow(static_cast<T&&>(val)) };
#else
					static_assert(dependent_false<T>, "Evaluated unreachable branch!");
#endif
				}
				else
					out = new value_type{ static_cast<T&&>(val) };
			}

			if (flags != preserve_source_value_flags)
				out->flags(flags);

			return out;
		}
	}

	template <typename T>
	TOML_NODISCARD
	auto* make_node_impl(T && val, value_flags flags = preserve_source_value_flags)
	{
		using unwrapped_type = unwrap_node<remove_cvref<T>>;
		if constexpr (std::is_same_v<unwrapped_type, node> || is_node_view<unwrapped_type>)
		{
			if constexpr (is_node_view<unwrapped_type>)
			{
				if (!val)
					return static_cast<toml::node*>(nullptr);
			}

			return static_cast<T&&>(val).visit(
				[flags](auto&& concrete) {
					return static_cast<toml::node*>(
						make_node_impl_specialized(static_cast<decltype(concrete)&&>(concrete), flags));
				});
		}
		else
			return make_node_impl_specialized(static_cast<T&&>(val), flags);
	}

	template <typename T>
	TOML_NODISCARD
	auto* make_node_impl(inserter<T> && val, value_flags flags = preserve_source_value_flags)
	{
		return make_node_impl(static_cast<T&&>(val.value), flags);
	}

	template <typename T, bool = (is_node<T> || is_node_view<T> || is_value<T> || can_partially_represent_native<T>)>
	struct inserted_type_of_
	{
		using type = std::remove_pointer_t<decltype(make_node_impl(std::declval<T>()))>;
	};

	template <typename T>
	struct inserted_type_of_<inserter<T>, false>
	{
		using type = typename inserted_type_of_<remove_cvref<T>>::type;
	};

	template <typename T>
	struct inserted_type_of_<T, false>
	{
		using type = void;
	};

	template <typename T>
	TOML_NODISCARD
	node_ptr make_node(T && val, value_flags flags = preserve_source_value_flags)
	{
		return node_ptr{ make_node_impl(static_cast<T&&>(val), flags) };
	}

	template <typename... T>
	struct emplaced_type_of_
	{
		using type = void;
	};

	template <typename T>
	struct emplaced_type_of_<T>
	{
		using type = std::conditional_t<is_one_of<T, node, node_view<node>, node_view<const node>>,
										void,
										typename inserted_type_of_<T>::type>;
	};

	template <typename T>
	struct emplaced_type_of_<inserter<T>>
	{
		using type = typename emplaced_type_of_<remove_cvref<T>>::type;
	};

	template <typename... T>
	using emplaced_type_of = typename emplaced_type_of_<remove_cvref<T>...>::type;
}
TOML_IMPL_NAMESPACE_END;

TOML_NAMESPACE_START
{

	template <typename T>
	using inserted_type_of = POXY_IMPLEMENTATION_DETAIL(typename impl::inserted_type_of_<impl::remove_cvref<T>>::type);
}
TOML_NAMESPACE_END;

#include "header_end.h"
