//# This file is a part of toml++ and is subject to the the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT
#pragma once

#include "date_time.h"
#include "node.h"
#include "print_to_stream.h"
#include "header_start.h"
TOML_DISABLE_ARITHMETIC_WARNINGS;

// clang-format off

#if TOML_ENABLE_WINDOWS_COMPAT
	#define TOML_SA_VALUE_MESSAGE_WSTRING			TOML_SA_LIST_SEP "std::wstring"
#else
	#define TOML_SA_VALUE_MESSAGE_WSTRING
#endif
#if TOML_HAS_CHAR8
	#define TOML_SA_VALUE_MESSAGE_U8STRING_VIEW		TOML_SA_LIST_SEP "std::u8string_view"
	#define TOML_SA_VALUE_MESSAGE_CONST_CHAR8		TOML_SA_LIST_SEP "const char8_t*"
#else
	#define TOML_SA_VALUE_MESSAGE_U8STRING_VIEW
	#define TOML_SA_VALUE_MESSAGE_CONST_CHAR8
#endif

#define TOML_SA_VALUE_EXACT_FUNC_MESSAGE(type_arg)															\
	"The " type_arg " must be one of:"																		\
	TOML_SA_LIST_NEW "A native TOML value type"																\
	TOML_SA_NATIVE_VALUE_TYPE_LIST																			\
																											\
	TOML_SA_LIST_NXT "A non-view type capable of losslessly representing a native TOML value type"			\
	TOML_SA_LIST_BEG "std::string"																			\
	TOML_SA_VALUE_MESSAGE_WSTRING																			\
	TOML_SA_LIST_SEP "any signed integer type >= 64 bits"													\
	TOML_SA_LIST_SEP "any floating-point type >= 64 bits"													\
	TOML_SA_LIST_END																						\
																											\
	TOML_SA_LIST_NXT "An immutable view type not requiring additional temporary storage"					\
	TOML_SA_LIST_BEG "std::string_view"																		\
	TOML_SA_VALUE_MESSAGE_U8STRING_VIEW																		\
	TOML_SA_LIST_SEP "const char*"																			\
	TOML_SA_VALUE_MESSAGE_CONST_CHAR8																		\
		TOML_SA_LIST_END

#define TOML_SA_VALUE_FUNC_MESSAGE(type_arg)																\
	"The " type_arg " must be one of:"																		\
	TOML_SA_LIST_NEW "A native TOML value type"																\
	TOML_SA_NATIVE_VALUE_TYPE_LIST																			\
																											\
	TOML_SA_LIST_NXT "A non-view type capable of losslessly representing a native TOML value type"			\
	TOML_SA_LIST_BEG "std::string"																			\
	TOML_SA_VALUE_MESSAGE_WSTRING																			\
	TOML_SA_LIST_SEP "any signed integer type >= 64 bits"													\
	TOML_SA_LIST_SEP "any floating-point type >= 64 bits"													\
	TOML_SA_LIST_END																						\
																											\
	TOML_SA_LIST_NXT "A non-view type capable of (reasonably) representing a native TOML value type"		\
	TOML_SA_LIST_BEG "any other integer type"																\
	TOML_SA_LIST_SEP "any floating-point type"																\
	TOML_SA_LIST_END																						\
																											\
	TOML_SA_LIST_NXT "An immutable view type not requiring additional temporary storage"					\
	TOML_SA_LIST_BEG "std::string_view"																		\
	TOML_SA_VALUE_MESSAGE_U8STRING_VIEW																		\
	TOML_SA_LIST_SEP "const char*"																			\
	TOML_SA_VALUE_MESSAGE_CONST_CHAR8																		\
	TOML_SA_LIST_END

// clang-format on

TOML_IMPL_NAMESPACE_START
{
	template <typename T, typename...>
	struct native_value_maker
	{
		template <typename... Args>
		TOML_NODISCARD
		static T make(Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args&&...>)
		{
			if constexpr (std::is_aggregate_v<T>)
				return T{ static_cast<Args&&>(args)... };
			else
				return T(static_cast<Args&&>(args)...);
		}
	};

	template <typename T>
	struct native_value_maker<T, T>
	{
		template <typename U>
		TOML_NODISCARD
		TOML_ALWAYS_INLINE
		static U&& make(U&& val) noexcept
		{
			return static_cast<U&&>(val);
		}
	};

#if TOML_HAS_CHAR8 || TOML_ENABLE_WINDOWS_COMPAT

	struct string_maker
	{
		template <typename T>
		TOML_NODISCARD
		static std::string make(T&& arg) noexcept
		{
			using arg_type = std::decay_t<T>;
#if TOML_HAS_CHAR8
			if constexpr (is_one_of<arg_type, char8_t*, const char8_t*>)
			{
				return std::string(reinterpret_cast<const char*>(static_cast<const char8_t*>(arg)));
			}
			if constexpr (is_one_of<arg_type, std::u8string, std::u8string_view>)
			{
				return std::string(reinterpret_cast<const char*>(static_cast<const char8_t*>(arg.data())),
								   arg.length());
			}
#endif

#if TOML_ENABLE_WINDOWS_COMPAT
			if constexpr (is_wide_string<arg_type>)
			{
				return narrow(static_cast<T&&>(arg));
			}
#endif
		}
	};

#if TOML_HAS_CHAR8
	template <>
	struct native_value_maker<std::string, char8_t*> : string_maker
	{};
	template <>
	struct native_value_maker<std::string, const char8_t*> : string_maker
	{};
	template <>
	struct native_value_maker<std::string, std::u8string> : string_maker
	{};
	template <>
	struct native_value_maker<std::string, std::u8string_view> : string_maker
	{};
#endif  

#if TOML_ENABLE_WINDOWS_COMPAT
	template <>
	struct native_value_maker<std::string, wchar_t*> : string_maker
	{};
	template <>
	struct native_value_maker<std::string, const wchar_t*> : string_maker
	{};
	template <>
	struct native_value_maker<std::string, std::wstring> : string_maker
	{};
	template <>
	struct native_value_maker<std::string, std::wstring_view> : string_maker
	{};
#endif  

#endif  

	template <typename T>
	TOML_CONST_GETTER
	inline optional<T> node_integer_cast(int64_t val) noexcept
	{
		static_assert(node_type_of<T> == node_type::integer);
		static_assert(!is_cvref<T>);

		using traits = value_traits<T>;
		if constexpr (!traits::is_signed)
		{
			if constexpr ((sizeof(T) * CHAR_BIT) < 63)  
			{
				using common_t = decltype(int64_t{} + T{});
				if (val < int64_t{} || static_cast<common_t>(val) > static_cast<common_t>(traits::max))
					return {};
			}
			else
			{
				if (val < int64_t{})
					return {};
			}
		}
		else
		{
			if (val < traits::min || val > traits::max)
				return {};
		}
		return { static_cast<T>(val) };
	}
}
TOML_IMPL_NAMESPACE_END;

TOML_NAMESPACE_START
{

	template <typename ValueType>
	class value : public node
	{
		static_assert(impl::is_native<ValueType> && !impl::is_cvref<ValueType>,
					  "A toml::value<> must model one of the native TOML value types:" TOML_SA_NATIVE_VALUE_TYPE_LIST);

	  private:

		friend class TOML_PARSER_TYPENAME;

		template <typename T, typename U>
		TOML_CONST_INLINE_GETTER
		static auto as_value([[maybe_unused]] U* ptr) noexcept
		{
			if constexpr (std::is_same_v<value_type, T>)
				return ptr;
			else
				return nullptr;
		}

		ValueType val_;
		value_flags flags_ = value_flags::none;

	  public:

		using value_type = ValueType;

		using value_arg = POXY_IMPLEMENTATION_DETAIL(
			std::conditional_t<
				std::is_same_v<value_type, std::string>,
				std::string_view,
				std::conditional_t<impl::is_one_of<value_type, double, int64_t, bool>, value_type, const value_type&>>);

		template <typename... Args>
		TOML_NODISCARD_CTOR
		explicit value(Args&&... args) noexcept(noexcept(value_type(
			impl::native_value_maker<value_type, std::decay_t<Args>...>::make(static_cast<Args&&>(args)...))))
			: val_(impl::native_value_maker<value_type, std::decay_t<Args>...>::make(static_cast<Args&&>(args)...))
		{
#if TOML_LIFETIME_HOOKS
			TOML_VALUE_CREATED;
#endif
		}

		TOML_NODISCARD_CTOR
		value(const value& other) noexcept  
			: node(other),
			  val_{ other.val_ },
			  flags_{ other.flags_ }
		{
#if TOML_LIFETIME_HOOKS
			TOML_VALUE_CREATED;
#endif
		}

		TOML_NODISCARD_CTOR
		value(const value& other, value_flags flags) noexcept  
			: node(other),
			  val_{ other.val_ },
			  flags_{ flags == preserve_source_value_flags ? other.flags_ : flags }
		{
#if TOML_LIFETIME_HOOKS
			TOML_VALUE_CREATED;
#endif
		}

		TOML_NODISCARD_CTOR
		value(value&& other) noexcept  
			: node(std::move(other)),
			  val_{ std::move(other.val_) },
			  flags_{ other.flags_ }
		{
#if TOML_LIFETIME_HOOKS
			TOML_VALUE_CREATED;
#endif
		}

		TOML_NODISCARD_CTOR
		value(value&& other, value_flags flags) noexcept  
			: node(std::move(other)),
			  val_{ std::move(other.val_) },
			  flags_{ flags == preserve_source_value_flags ? other.flags_ : flags }
		{
#if TOML_LIFETIME_HOOKS
			TOML_VALUE_CREATED;
#endif
		}

		value& operator=(const value& rhs) noexcept
		{
			node::operator=(rhs);
			val_   = rhs.val_;
			flags_ = rhs.flags_;
			return *this;
		}

		value& operator=(value&& rhs) noexcept
		{
			if (&rhs != this)
			{
				node::operator=(std::move(rhs));
				val_   = std::move(rhs.val_);
				flags_ = rhs.flags_;
			}
			return *this;
		}

#if TOML_LIFETIME_HOOKS
		~value() noexcept
		{
			TOML_VALUE_DESTROYED;
		}
#endif

		TOML_CONST_INLINE_GETTER
		node_type type() const noexcept final
		{
			return impl::node_type_of<value_type>;
		}

		TOML_PURE_GETTER
		bool is_homogeneous(node_type ntype) const noexcept final
		{
			return ntype == node_type::none || ntype == impl::node_type_of<value_type>;
		}

		TOML_PURE_GETTER
		bool is_homogeneous(node_type ntype, node*& first_nonmatch) noexcept final
		{
			if (ntype != node_type::none && ntype != impl::node_type_of<value_type>)
			{
				first_nonmatch = this;
				return false;
			}
			return true;
		}

		TOML_PURE_GETTER
		bool is_homogeneous(node_type ntype, const node*& first_nonmatch) const noexcept final
		{
			if (ntype != node_type::none && ntype != impl::node_type_of<value_type>)
			{
				first_nonmatch = this;
				return false;
			}
			return true;
		}

		template <typename ElemType = void>
		TOML_PURE_GETTER
		bool is_homogeneous() const noexcept
		{
			using type = impl::remove_cvref<impl::unwrap_node<ElemType>>;
			static_assert(std::is_void_v<type> || toml::is_value<type> || toml::is_container<type>,
						  "The template type argument of value::is_homogeneous() must be void or one "
						  "of:" TOML_SA_UNWRAPPED_NODE_TYPE_LIST);

			if constexpr (std::is_void_v<type>)
				return true;
			else
				return impl::node_type_of<type> == impl::node_type_of<value_type>;
		}

		TOML_CONST_INLINE_GETTER
		bool is_table() const noexcept final
		{
			return false;
		}

		TOML_CONST_INLINE_GETTER
		bool is_array() const noexcept final
		{
			return false;
		}

		TOML_CONST_INLINE_GETTER
		bool is_array_of_tables() const noexcept final
		{
			return false;
		}

		TOML_CONST_INLINE_GETTER
		bool is_value() const noexcept final
		{
			return true;
		}

		TOML_CONST_INLINE_GETTER
		bool is_string() const noexcept final
		{
			return std::is_same_v<value_type, std::string>;
		}

		TOML_CONST_INLINE_GETTER
		bool is_integer() const noexcept final
		{
			return std::is_same_v<value_type, int64_t>;
		}

		TOML_CONST_INLINE_GETTER
		bool is_floating_point() const noexcept final
		{
			return std::is_same_v<value_type, double>;
		}

		TOML_CONST_INLINE_GETTER
		bool is_number() const noexcept final
		{
			return impl::is_one_of<value_type, int64_t, double>;
		}

		TOML_CONST_INLINE_GETTER
		bool is_boolean() const noexcept final
		{
			return std::is_same_v<value_type, bool>;
		}

		TOML_CONST_INLINE_GETTER
		bool is_date() const noexcept final
		{
			return std::is_same_v<value_type, date>;
		}

		TOML_CONST_INLINE_GETTER
		bool is_time() const noexcept final
		{
			return std::is_same_v<value_type, time>;
		}

		TOML_CONST_INLINE_GETTER
		bool is_date_time() const noexcept final
		{
			return std::is_same_v<value_type, date_time>;
		}

		TOML_CONST_INLINE_GETTER
		table* as_table() noexcept final
		{
			return nullptr;
		}

		TOML_CONST_INLINE_GETTER
		array* as_array() noexcept final
		{
			return nullptr;
		}

		TOML_CONST_INLINE_GETTER
		value<std::string>* as_string() noexcept final
		{
			return as_value<std::string>(this);
		}

		TOML_CONST_INLINE_GETTER
		value<int64_t>* as_integer() noexcept final
		{
			return as_value<int64_t>(this);
		}

		TOML_CONST_INLINE_GETTER
		value<double>* as_floating_point() noexcept final
		{
			return as_value<double>(this);
		}

		TOML_CONST_INLINE_GETTER
		value<bool>* as_boolean() noexcept final
		{
			return as_value<bool>(this);
		}

		TOML_CONST_INLINE_GETTER
		value<date>* as_date() noexcept final
		{
			return as_value<date>(this);
		}

		TOML_CONST_INLINE_GETTER
		value<time>* as_time() noexcept final
		{
			return as_value<time>(this);
		}

		TOML_CONST_INLINE_GETTER
		value<date_time>* as_date_time() noexcept final
		{
			return as_value<date_time>(this);
		}

		TOML_CONST_INLINE_GETTER
		const table* as_table() const noexcept final
		{
			return nullptr;
		}

		TOML_CONST_INLINE_GETTER
		const array* as_array() const noexcept final
		{
			return nullptr;
		}

		TOML_CONST_INLINE_GETTER
		const value<std::string>* as_string() const noexcept final
		{
			return as_value<std::string>(this);
		}

		TOML_CONST_INLINE_GETTER
		const value<int64_t>* as_integer() const noexcept final
		{
			return as_value<int64_t>(this);
		}

		TOML_CONST_INLINE_GETTER
		const value<double>* as_floating_point() const noexcept final
		{
			return as_value<double>(this);
		}

		TOML_CONST_INLINE_GETTER
		const value<bool>* as_boolean() const noexcept final
		{
			return as_value<bool>(this);
		}

		TOML_CONST_INLINE_GETTER
		const value<date>* as_date() const noexcept final
		{
			return as_value<date>(this);
		}

		TOML_CONST_INLINE_GETTER
		const value<time>* as_time() const noexcept final
		{
			return as_value<time>(this);
		}

		TOML_CONST_INLINE_GETTER
		const value<date_time>* as_date_time() const noexcept final
		{
			return as_value<date_time>(this);
		}

		TOML_PURE_INLINE_GETTER
		value_type& get() & noexcept
		{
			return val_;
		}

		TOML_PURE_INLINE_GETTER
		value_type&& get() && noexcept
		{
			return static_cast<value_type&&>(val_);
		}

		TOML_PURE_INLINE_GETTER
		const value_type& get() const& noexcept
		{
			return val_;
		}

		TOML_PURE_INLINE_GETTER
		const value_type&& get() const&& noexcept
		{
			return static_cast<const value_type&&>(val_);
		}

		TOML_PURE_INLINE_GETTER
		value_type& operator*() & noexcept
		{
			return val_;
		}

		TOML_PURE_INLINE_GETTER
		value_type&& operator*() && noexcept
		{
			return static_cast<value_type&&>(val_);
		}

		TOML_PURE_INLINE_GETTER
		const value_type& operator*() const& noexcept
		{
			return val_;
		}

		TOML_PURE_INLINE_GETTER
		const value_type&& operator*() const&& noexcept
		{
			return static_cast<const value_type&&>(val_);
		}

		TOML_PURE_INLINE_GETTER
		explicit operator value_type&() & noexcept
		{
			return val_;
		}

		TOML_PURE_INLINE_GETTER
		explicit operator value_type&&() && noexcept
		{
			return static_cast<value_type&&>(val_);
		}

		TOML_PURE_INLINE_GETTER
		explicit operator const value_type&() const& noexcept
		{
			return val_;
		}

		TOML_PURE_INLINE_GETTER
		explicit operator const value_type&&() && noexcept
		{
			return static_cast<const value_type&&>(val_);
		}

		TOML_HIDDEN_CONSTRAINT(std::is_class_v<T>, typename T = value_type)
		TOML_PURE_INLINE_GETTER
		value_type* operator->() noexcept
		{
			return &val_;
		}

		TOML_HIDDEN_CONSTRAINT(std::is_class_v<T>, typename T = value_type)
		TOML_PURE_INLINE_GETTER
		const value_type* operator->() const noexcept
		{
			return &val_;
		}

		TOML_NODISCARD
		value_flags flags() const noexcept
		{
			return flags_;
		}

		value& flags(value_flags new_flags) noexcept
		{
			flags_ = new_flags;
			return *this;
		}

		value& operator=(value_arg rhs) noexcept
		{
			if constexpr (std::is_same_v<value_type, std::string>)
				val_.assign(rhs);
			else
				val_ = rhs;
			return *this;
		}

		TOML_CONSTRAINED_TEMPLATE((std::is_same_v<T, std::string>), typename T = value_type)
		value& operator=(std::string&& rhs) noexcept
		{
			val_ = std::move(rhs);
			return *this;
		}

		TOML_PURE_GETTER
		friend bool operator==(const value& lhs, value_arg rhs) noexcept
		{
			if constexpr (std::is_same_v<value_type, double>)
			{
				const auto lhs_nan = impl::fpclassify(lhs.val_) == impl::fp_class::nan;
				const auto rhs_nan = impl::fpclassify(rhs) == impl::fp_class::nan;
				if (lhs_nan != rhs_nan)
					return false;
				if (lhs_nan)
					return true;
			}
			return lhs.val_ == rhs;
		}
		TOML_ASYMMETRICAL_EQUALITY_OPS(const value&, value_arg, );

		TOML_PURE_GETTER
		friend bool operator<(const value& lhs, value_arg rhs) noexcept
		{
			return lhs.val_ < rhs;
		}

		TOML_PURE_GETTER
		friend bool operator<(value_arg lhs, const value& rhs) noexcept
		{
			return lhs < rhs.val_;
		}

		TOML_PURE_GETTER
		friend bool operator<=(const value& lhs, value_arg rhs) noexcept
		{
			return lhs.val_ <= rhs;
		}

		TOML_PURE_GETTER
		friend bool operator<=(value_arg lhs, const value& rhs) noexcept
		{
			return lhs <= rhs.val_;
		}

		TOML_PURE_GETTER
		friend bool operator>(const value& lhs, value_arg rhs) noexcept
		{
			return lhs.val_ > rhs;
		}

		TOML_PURE_GETTER
		friend bool operator>(value_arg lhs, const value& rhs) noexcept
		{
			return lhs > rhs.val_;
		}

		TOML_PURE_GETTER
		friend bool operator>=(const value& lhs, value_arg rhs) noexcept
		{
			return lhs.val_ >= rhs;
		}

		TOML_PURE_GETTER
		friend bool operator>=(value_arg lhs, const value& rhs) noexcept
		{
			return lhs >= rhs.val_;
		}

		template <typename T>
		TOML_PURE_GETTER
		friend bool operator==(const value& lhs, const value<T>& rhs) noexcept
		{
			if constexpr (std::is_same_v<value_type, T>)
				return lhs == rhs.val_;  
			else
				return false;
		}

		template <typename T>
		TOML_PURE_INLINE_GETTER
		friend bool operator!=(const value& lhs, const value<T>& rhs) noexcept
		{
			return !(lhs == rhs);
		}

		template <typename T>
		TOML_PURE_GETTER
		friend bool operator<(const value& lhs, const value<T>& rhs) noexcept
		{
			if constexpr (std::is_same_v<value_type, T>)
				return lhs.val_ < rhs.val_;
			else
				return impl::node_type_of<value_type> < impl::node_type_of<T>;
		}

		template <typename T>
		TOML_PURE_GETTER
		friend bool operator<=(const value& lhs, const value<T>& rhs) noexcept
		{
			if constexpr (std::is_same_v<value_type, T>)
				return lhs.val_ <= rhs.val_;
			else
				return impl::node_type_of<value_type> <= impl::node_type_of<T>;
		}

		template <typename T>
		TOML_PURE_GETTER
		friend bool operator>(const value& lhs, const value<T>& rhs) noexcept
		{
			if constexpr (std::is_same_v<value_type, T>)
				return lhs.val_ > rhs.val_;
			else
				return impl::node_type_of<value_type> > impl::node_type_of<T>;
		}

		template <typename T>
		TOML_PURE_GETTER
		friend bool operator>=(const value& lhs, const value<T>& rhs) noexcept
		{
			if constexpr (std::is_same_v<value_type, T>)
				return lhs.val_ >= rhs.val_;
			else
				return impl::node_type_of<value_type> >= impl::node_type_of<T>;
		}

#if TOML_ENABLE_FORMATTERS

		friend std::ostream& operator<<(std::ostream& lhs, const value& rhs)
		{
			impl::print_to_stream(lhs, rhs);
			return lhs;
		}

#endif
	};

	template <typename T>
	value(T) -> value<impl::native_type_of<impl::remove_cvref<T>>>;

	template <typename T>
	TOML_NODISCARD
	inline decltype(auto) node::get_value_exact() const noexcept(impl::value_retrieval_is_nothrow<T>)
	{
		using namespace impl;

		static_assert(node_type_of<T> != node_type::none);
		static_assert(node_type_of<T> != node_type::table);
		static_assert(node_type_of<T> != node_type::array);
		static_assert(is_native<T> || can_represent_native<T>);
		static_assert(!is_cvref<T>);
		TOML_ASSERT(this->type() == node_type_of<T>);

		if constexpr (node_type_of<T> == node_type::string)
		{
			const auto& str = *ref_cast<std::string>();
			if constexpr (std::is_same_v<T, std::string>)
				return str;
			else if constexpr (std::is_same_v<T, std::string_view>)
				return T{ str };
			else if constexpr (std::is_same_v<T, const char*>)
				return str.c_str();

			else if constexpr (std::is_same_v<T, std::wstring>)
			{
#if TOML_ENABLE_WINDOWS_COMPAT
				return widen(str);
#else
				static_assert(dependent_false<T>, "Evaluated unreachable branch!");
#endif
			}

#if TOML_HAS_CHAR8

			else if constexpr (is_one_of<T, std::u8string, std::u8string_view>)
				return T(reinterpret_cast<const char8_t*>(str.c_str()), str.length());
			else if constexpr (std::is_same_v<T, const char8_t*>)
				return reinterpret_cast<const char8_t*>(str.c_str());
			else
				static_assert(dependent_false<T>, "Evaluated unreachable branch!");

#endif
		}
		else
			return static_cast<T>(*ref_cast<native_type_of<T>>());
	}

	template <typename T>
	TOML_NODISCARD
	inline optional<T> node::value_exact() const noexcept(impl::value_retrieval_is_nothrow<T>)
	{
		using namespace impl;

		static_assert(!is_wide_string<T> || TOML_ENABLE_WINDOWS_COMPAT,
					  "Retrieving values as wide-character strings with node::value_exact() is only "
					  "supported on Windows with TOML_ENABLE_WINDOWS_COMPAT enabled.");

		static_assert((is_native<T> || can_represent_native<T>)&&!is_cvref<T>,
					  TOML_SA_VALUE_EXACT_FUNC_MESSAGE("return type of node::value_exact()"));

		if constexpr ((is_native<T> || can_represent_native<T>)&&!is_cvref<T>)
		{
			if (type() == node_type_of<T>)
				return { this->get_value_exact<T>() };
			else
				return {};
		}
	}

	template <typename T>
	TOML_NODISCARD
	inline optional<T> node::value() const noexcept(impl::value_retrieval_is_nothrow<T>)
	{
		using namespace impl;

		static_assert(!is_wide_string<T> || TOML_ENABLE_WINDOWS_COMPAT,
					  "Retrieving values as wide-character strings with node::value() is only "
					  "supported on Windows with TOML_ENABLE_WINDOWS_COMPAT enabled.");
		static_assert((is_native<T> || can_represent_native<T> || can_partially_represent_native<T>)&&!is_cvref<T>,
					  TOML_SA_VALUE_FUNC_MESSAGE("return type of node::value()"));

		if constexpr (is_natively_one_of<T, std::string, time, date, date_time>)
		{
			if (type() == node_type_of<T>)
				return { this->get_value_exact<T>() };
			else
				return {};
		}

		else
		{
			switch (type())
			{

				case node_type::integer:
				{

					if constexpr (is_natively_one_of<T, int64_t>)
					{
						if constexpr (is_native<T> || can_represent_native<T>)
							return static_cast<T>(*ref_cast<int64_t>());
						else
							return node_integer_cast<T>(*ref_cast<int64_t>());
					}

					else if constexpr (is_natively_one_of<T, double>)
					{
						const int64_t val = *ref_cast<int64_t>();
						if constexpr (std::numeric_limits<T>::digits < 64)
						{
							constexpr auto largest_whole_float = (int64_t{ 1 } << std::numeric_limits<T>::digits);
							if (val < -largest_whole_float || val > largest_whole_float)
								return {};
						}
						return static_cast<T>(val);
					}

					else if constexpr (is_natively_one_of<T, bool>)
						return static_cast<bool>(*ref_cast<int64_t>());

					else
						return {};
				}

				case node_type::floating_point:
				{

					if constexpr (is_natively_one_of<T, double>)
					{
						if constexpr (is_native<T> || can_represent_native<T>)
							return { static_cast<T>(*ref_cast<double>()) };
						else
						{
							const double val = *ref_cast<double>();
							if (impl::fpclassify(val) == fp_class::ok
								&& (val < (std::numeric_limits<T>::lowest)() || val > (std::numeric_limits<T>::max)()))
								return {};
							return { static_cast<T>(val) };
						}
					}

					else if constexpr (is_natively_one_of<T, int64_t>)
					{
						const double val = *ref_cast<double>();
						if (impl::fpclassify(val) == fp_class::ok
							&& static_cast<double>(static_cast<int64_t>(val)) == val)
							return node_integer_cast<T>(static_cast<int64_t>(val));
						else
							return {};
					}

					else
						return {};
				}

				case node_type::boolean:
				{

					if constexpr (is_natively_one_of<T, bool>)
						return { *ref_cast<bool>() };

					else if constexpr (is_natively_one_of<T, int64_t>)
						return { static_cast<T>(*ref_cast<bool>()) };

					else
						return {};
				}
			}

			return {};
		}
	}

	template <typename T>
	TOML_NODISCARD
	inline auto node::value_or(T && default_value) const noexcept(impl::value_retrieval_is_nothrow<T>)
	{
		using namespace impl;

		static_assert(!is_wide_string<T> || TOML_ENABLE_WINDOWS_COMPAT,
					  "Retrieving values as wide-character strings with node::value_or() is only "
					  "supported on Windows with TOML_ENABLE_WINDOWS_COMPAT enabled.");

		if constexpr (is_wide_string<T>)
		{
#if TOML_ENABLE_WINDOWS_COMPAT

			if (type() == node_type::string)
				return widen(*ref_cast<std::string>());
			return std::wstring{ static_cast<T&&>(default_value) };

#else

			static_assert(dependent_false<T>, "Evaluated unreachable branch!");

#endif
		}
		else
		{
			using value_type =
				std::conditional_t<std::is_pointer_v<std::decay_t<T>>,
								   std::add_pointer_t<std::add_const_t<std::remove_pointer_t<std::decay_t<T>>>>,
								   std::decay_t<T>>;
			using traits = value_traits<value_type>;

			// clang-format off

			static_assert(
				traits::is_native || traits::can_represent_native || traits::can_partially_represent_native,
				"The default value type of node::value_or() must be one of:"
				TOML_SA_LIST_NEW "A native TOML value type"
				TOML_SA_NATIVE_VALUE_TYPE_LIST

				TOML_SA_LIST_NXT "A non-view type capable of losslessly representing a native TOML value type"
				TOML_SA_LIST_BEG "std::string"
				#if TOML_ENABLE_WINDOWS_COMPAT
				TOML_SA_LIST_SEP "std::wstring"
				#endif
				TOML_SA_LIST_SEP "any signed integer type >= 64 bits"
				TOML_SA_LIST_SEP "any floating-point type >= 64 bits"
				TOML_SA_LIST_END

				TOML_SA_LIST_NXT "A non-view type capable of (reasonably) representing a native TOML value type"
				TOML_SA_LIST_BEG "any other integer type"
				TOML_SA_LIST_SEP "any floating-point type"
				TOML_SA_LIST_END

				TOML_SA_LIST_NXT "A compatible view type"
				TOML_SA_LIST_BEG "std::string_view"
				#if TOML_HAS_CHAR8
				TOML_SA_LIST_SEP "std::u8string_view"
				#endif
				#if TOML_ENABLE_WINDOWS_COMPAT
				TOML_SA_LIST_SEP "std::wstring_view"
				#endif
				TOML_SA_LIST_SEP "const char*"
				#if TOML_HAS_CHAR8
				TOML_SA_LIST_SEP "const char8_t*"
				#endif
				#if TOML_ENABLE_WINDOWS_COMPAT
				TOML_SA_LIST_SEP "const wchar_t*"
				#endif
				TOML_SA_LIST_END
			);

			// clang-format on

			if constexpr (traits::is_native || traits::can_represent_native || traits::can_partially_represent_native)
			{
				if constexpr (traits::is_native)
				{
					if (type() == node_type_of<value_type>)
						return *ref_cast<typename traits::native_type>();
				}
				if (auto val = this->value<value_type>())
					return *val;
				if constexpr (std::is_pointer_v<value_type>)
					return value_type{ default_value };
				else
					return static_cast<T&&>(default_value);
			}
		}
	}

}
TOML_NAMESPACE_END;

#include "header_end.h"
