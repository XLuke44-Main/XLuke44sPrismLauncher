//# This file is a part of toml++ and is subject to the the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT
#pragma once

#include "forward_declarations.h"
#include "std_map.h"
#include "std_initializer_list.h"
#include "array.h"
#include "make_node.h"
#include "node_view.h"
#include "key.h"
#include "header_start.h"

TOML_IMPL_NAMESPACE_START
{
	template <bool IsConst>
	struct table_proxy_pair
	{
		using value_type = std::conditional_t<IsConst, const node, node>;

		const toml::key& first;
		value_type& second;
	};

	template <bool IsConst>
	class table_iterator
	{
	  private:
		template <bool>
		friend class table_iterator;

		using proxy_type		   = table_proxy_pair<IsConst>;
		using mutable_map_iterator = std::map<toml::key, node_ptr, std::less<>>::iterator;
		using const_map_iterator   = std::map<toml::key, node_ptr, std::less<>>::const_iterator;
		using map_iterator		   = std::conditional_t<IsConst, const_map_iterator, mutable_map_iterator>;

		mutable map_iterator iter_;
		alignas(proxy_type) mutable unsigned char proxy_[sizeof(proxy_type)];
		mutable bool proxy_instantiated_ = false;

		TOML_NODISCARD
		proxy_type* get_proxy() const noexcept
		{
			if (!proxy_instantiated_)
			{
				auto p = ::new (static_cast<void*>(proxy_)) proxy_type{ iter_->first, *iter_->second.get() };
				proxy_instantiated_ = true;
				return p;
			}
			else
				return TOML_LAUNDER(reinterpret_cast<proxy_type*>(proxy_));
		}

	  public:
		TOML_NODISCARD_CTOR
		table_iterator() noexcept = default;

		TOML_NODISCARD_CTOR
		explicit table_iterator(mutable_map_iterator iter) noexcept  
			: iter_{ iter }
		{}

		TOML_CONSTRAINED_TEMPLATE(C, bool C = IsConst)
		TOML_NODISCARD_CTOR
		explicit table_iterator(const_map_iterator iter) noexcept  
			: iter_{ iter }
		{}

		TOML_CONSTRAINED_TEMPLATE(C, bool C = IsConst)
		TOML_NODISCARD_CTOR
		table_iterator(const table_iterator<false>& other) noexcept  
			: iter_{ other.iter_ }
		{}

		TOML_NODISCARD_CTOR
		table_iterator(const table_iterator& other) noexcept  
			: iter_{ other.iter_ }
		{}

		table_iterator& operator=(const table_iterator& rhs) noexcept
		{
			iter_				= rhs.iter_;
			proxy_instantiated_ = false;
			return *this;
		}

		using value_type		= table_proxy_pair<IsConst>;
		using reference			= value_type&;
		using pointer			= value_type*;
		using difference_type	= typename std::iterator_traits<map_iterator>::difference_type;
		using iterator_category = typename std::iterator_traits<map_iterator>::iterator_category;

		table_iterator& operator++() noexcept  
		{
			++iter_;
			proxy_instantiated_ = false;
			return *this;
		}

		table_iterator operator++(int) noexcept  
		{
			table_iterator out{ iter_ };
			++iter_;
			proxy_instantiated_ = false;
			return out;
		}

		table_iterator& operator--() noexcept  
		{
			--iter_;
			proxy_instantiated_ = false;
			return *this;
		}

		table_iterator operator--(int) noexcept  
		{
			table_iterator out{ iter_ };
			--iter_;
			proxy_instantiated_ = false;
			return out;
		}

		TOML_PURE_INLINE_GETTER
		reference operator*() const noexcept
		{
			return *get_proxy();
		}

		TOML_PURE_INLINE_GETTER
		pointer operator->() const noexcept
		{
			return get_proxy();
		}

		TOML_PURE_INLINE_GETTER
		explicit operator const map_iterator&() const noexcept
		{
			return iter_;
		}

		TOML_CONSTRAINED_TEMPLATE(!C, bool C = IsConst)
		TOML_PURE_INLINE_GETTER
		explicit operator const const_map_iterator() const noexcept
		{
			return iter_;
		}

		TOML_PURE_INLINE_GETTER
		friend bool operator==(const table_iterator& lhs, const table_iterator& rhs) noexcept
		{
			return lhs.iter_ == rhs.iter_;
		}

		TOML_PURE_INLINE_GETTER
		friend bool operator!=(const table_iterator& lhs, const table_iterator& rhs) noexcept
		{
			return lhs.iter_ != rhs.iter_;
		}
	};

	struct table_init_pair
	{
		mutable toml::key key;
		mutable node_ptr value;

		template <typename K, typename V>
		TOML_NODISCARD_CTOR
		table_init_pair(K&& k, V&& v, value_flags flags = preserve_source_value_flags)  
			: key{ static_cast<K&&>(k) },
			  value{ make_node(static_cast<V&&>(v), flags) }
		{}
	};
}
TOML_IMPL_NAMESPACE_END;

TOML_NAMESPACE_START
{

	using table_iterator = POXY_IMPLEMENTATION_DETAIL(impl::table_iterator<false>);

	using const_table_iterator = POXY_IMPLEMENTATION_DETAIL(impl::table_iterator<true>);

	class TOML_EXPORTED_CLASS table : public node
	{
	  private:

		using map_type			 = std::map<toml::key, impl::node_ptr, std::less<>>;
		using map_pair			 = std::pair<const toml::key, impl::node_ptr>;
		using map_iterator		 = typename map_type::iterator;
		using const_map_iterator = typename map_type::const_iterator;
		map_type map_;

		bool inline_ = false;

		TOML_NODISCARD_CTOR
		TOML_EXPORTED_MEMBER_FUNCTION
		table(const impl::table_init_pair*, const impl::table_init_pair*);

	  public:

		TOML_NODISCARD_CTOR
		TOML_EXPORTED_MEMBER_FUNCTION
		table() noexcept;

		TOML_EXPORTED_MEMBER_FUNCTION
		~table() noexcept;

		TOML_NODISCARD_CTOR
		TOML_EXPORTED_MEMBER_FUNCTION
		table(const table&);

		TOML_NODISCARD_CTOR
		TOML_EXPORTED_MEMBER_FUNCTION
		table(table&& other) noexcept;

		TOML_NODISCARD_CTOR
		TOML_EXPORTED_MEMBER_FUNCTION
		explicit table(std::initializer_list<impl::table_init_pair> kvps)  
			: table(kvps.begin(), kvps.end())
		{}

		TOML_EXPORTED_MEMBER_FUNCTION
		table& operator=(const table&);

		TOML_EXPORTED_MEMBER_FUNCTION
		table& operator=(table&& rhs) noexcept;

		TOML_CONST_INLINE_GETTER
		node_type type() const noexcept final
		{
			return node_type::table;
		}

		TOML_PURE_GETTER
		TOML_EXPORTED_MEMBER_FUNCTION
		bool is_homogeneous(node_type ntype) const noexcept final;

		TOML_PURE_GETTER
		TOML_EXPORTED_MEMBER_FUNCTION
		bool is_homogeneous(node_type ntype, node*& first_nonmatch) noexcept final;

		TOML_PURE_GETTER
		TOML_EXPORTED_MEMBER_FUNCTION
		bool is_homogeneous(node_type ntype, const node*& first_nonmatch) const noexcept final;

		template <typename ElemType = void>
		TOML_PURE_GETTER
		bool is_homogeneous() const noexcept
		{
			using type = impl::remove_cvref<impl::unwrap_node<ElemType>>;
			static_assert(std::is_void_v<type> || toml::is_value<type> || toml::is_container<type>,
						  "The template type argument of table::is_homogeneous() must be void or one "
						  "of:" TOML_SA_UNWRAPPED_NODE_TYPE_LIST);

			return is_homogeneous(impl::node_type_of<type>);
		}

		TOML_CONST_INLINE_GETTER
		bool is_table() const noexcept final
		{
			return true;
		}

		TOML_CONST_INLINE_GETTER
		bool is_array() const noexcept final
		{
			return false;
		}

		TOML_PURE_GETTER
		bool is_array_of_tables() const noexcept final
		{
			return false;
		}

		TOML_CONST_INLINE_GETTER
		bool is_value() const noexcept final
		{
			return false;
		}

		TOML_CONST_INLINE_GETTER
		bool is_string() const noexcept final
		{
			return false;
		}

		TOML_CONST_INLINE_GETTER
		bool is_integer() const noexcept final
		{
			return false;
		}

		TOML_CONST_INLINE_GETTER
		bool is_floating_point() const noexcept final
		{
			return false;
		}

		TOML_CONST_INLINE_GETTER
		bool is_number() const noexcept final
		{
			return false;
		}

		TOML_CONST_INLINE_GETTER
		bool is_boolean() const noexcept final
		{
			return false;
		}

		TOML_CONST_INLINE_GETTER
		bool is_date() const noexcept final
		{
			return false;
		}

		TOML_CONST_INLINE_GETTER
		bool is_time() const noexcept final
		{
			return false;
		}

		TOML_CONST_INLINE_GETTER
		bool is_date_time() const noexcept final
		{
			return false;
		}

		TOML_CONST_INLINE_GETTER
		table* as_table() noexcept final
		{
			return this;
		}

		TOML_CONST_INLINE_GETTER
		array* as_array() noexcept final
		{
			return nullptr;
		}

		TOML_CONST_INLINE_GETTER
		toml::value<std::string>* as_string() noexcept final
		{
			return nullptr;
		}

		TOML_CONST_INLINE_GETTER
		toml::value<int64_t>* as_integer() noexcept final
		{
			return nullptr;
		}

		TOML_CONST_INLINE_GETTER
		toml::value<double>* as_floating_point() noexcept final
		{
			return nullptr;
		}

		TOML_CONST_INLINE_GETTER
		toml::value<bool>* as_boolean() noexcept final
		{
			return nullptr;
		}

		TOML_CONST_INLINE_GETTER
		toml::value<date>* as_date() noexcept final
		{
			return nullptr;
		}

		TOML_CONST_INLINE_GETTER
		toml::value<time>* as_time() noexcept final
		{
			return nullptr;
		}

		TOML_CONST_INLINE_GETTER
		toml::value<date_time>* as_date_time() noexcept final
		{
			return nullptr;
		}

		TOML_CONST_INLINE_GETTER
		const table* as_table() const noexcept final
		{
			return this;
		}

		TOML_CONST_INLINE_GETTER
		const array* as_array() const noexcept final
		{
			return nullptr;
		}

		TOML_CONST_INLINE_GETTER
		const toml::value<std::string>* as_string() const noexcept final
		{
			return nullptr;
		}

		TOML_CONST_INLINE_GETTER
		const toml::value<int64_t>* as_integer() const noexcept final
		{
			return nullptr;
		}

		TOML_CONST_INLINE_GETTER
		const toml::value<double>* as_floating_point() const noexcept final
		{
			return nullptr;
		}

		TOML_CONST_INLINE_GETTER
		const toml::value<bool>* as_boolean() const noexcept final
		{
			return nullptr;
		}

		TOML_CONST_INLINE_GETTER
		const toml::value<date>* as_date() const noexcept final
		{
			return nullptr;
		}

		TOML_CONST_INLINE_GETTER
		const toml::value<time>* as_time() const noexcept final
		{
			return nullptr;
		}

		TOML_CONST_INLINE_GETTER
		const toml::value<date_time>* as_date_time() const noexcept final
		{
			return nullptr;
		}

		TOML_PURE_INLINE_GETTER
		bool is_inline() const noexcept
		{
			return inline_;
		}

		void is_inline(bool val) noexcept
		{
			inline_ = val;
		}

		TOML_PURE_GETTER
		TOML_EXPORTED_MEMBER_FUNCTION
		node* get(std::string_view key) noexcept;

		TOML_PURE_INLINE_GETTER
		const node* get(std::string_view key) const noexcept
		{
			return const_cast<table&>(*this).get(key);
		}

#if TOML_ENABLE_WINDOWS_COMPAT

		TOML_NODISCARD
		node* get(std::wstring_view key)
		{
			if (empty())
				return nullptr;

			return get(impl::narrow(key));
		}

		TOML_NODISCARD
		const node* get(std::wstring_view key) const
		{
			return const_cast<table&>(*this).get(key);
		}

#endif  

		template <typename T>
		TOML_PURE_GETTER
		impl::wrap_node<T>* get_as(std::string_view key) noexcept
		{
			const auto n = this->get(key);
			return n ? n->template as<T>() : nullptr;
		}

		template <typename T>
		TOML_PURE_GETTER
		const impl::wrap_node<T>* get_as(std::string_view key) const noexcept
		{
			return const_cast<table&>(*this).template get_as<T>(key);
		}

#if TOML_ENABLE_WINDOWS_COMPAT

		template <typename T>
		TOML_NODISCARD
		impl::wrap_node<T>* get_as(std::wstring_view key)
		{
			if (empty())
				return nullptr;

			return get_as<T>(impl::narrow(key));
		}

		template <typename T>
		TOML_NODISCARD
		const impl::wrap_node<T>* get_as(std::wstring_view key) const
		{
			return const_cast<table&>(*this).template get_as<T>(key);
		}

#endif  

		TOML_NODISCARD
		TOML_EXPORTED_MEMBER_FUNCTION
		node& at(std::string_view key);

		TOML_NODISCARD
		const node& at(std::string_view key) const
		{
			return const_cast<table&>(*this).at(key);
		}

#if TOML_ENABLE_WINDOWS_COMPAT

		TOML_NODISCARD
		node& at(std::wstring_view key)
		{
			return at(impl::narrow(key));
		}

		TOML_NODISCARD
		const node& at(std::wstring_view key) const
		{
			return const_cast<table&>(*this).at(key);
		}

#endif  

		using iterator = toml::table_iterator;

		using const_iterator = toml::const_table_iterator;

		TOML_PURE_INLINE_GETTER
		iterator begin() noexcept
		{
			return iterator{ map_.begin() };
		}

		TOML_PURE_INLINE_GETTER
		const_iterator begin() const noexcept
		{
			return const_iterator{ map_.cbegin() };
		}

		TOML_PURE_INLINE_GETTER
		const_iterator cbegin() const noexcept
		{
			return const_iterator{ map_.cbegin() };
		}

		TOML_PURE_INLINE_GETTER
		iterator end() noexcept
		{
			return iterator{ map_.end() };
		}

		TOML_PURE_INLINE_GETTER
		const_iterator end() const noexcept
		{
			return const_iterator{ map_.cend() };
		}

		TOML_PURE_INLINE_GETTER
		const_iterator cend() const noexcept
		{
			return const_iterator{ map_.cend() };
		}

	  private:

		template <typename T, typename Table>
		using for_each_value_ref = impl::copy_cvref<impl::wrap_node<impl::remove_cvref<impl::unwrap_node<T>>>, Table>;

		template <typename Func, typename Table, typename T>
		static constexpr bool can_for_each = std::is_invocable_v<Func, const key&, for_each_value_ref<T, Table>>  
										  || std::is_invocable_v<Func, for_each_value_ref<T, Table>>;

		template <typename Func, typename Table, typename T>
		static constexpr bool can_for_each_nothrow =
			std::is_nothrow_invocable_v<Func, const key&, for_each_value_ref<T, Table>>  
			|| std::is_nothrow_invocable_v<Func, for_each_value_ref<T, Table>>;

		template <typename Func, typename Table>
		static constexpr bool can_for_each_any = can_for_each<Func, Table, table>		 
											  || can_for_each<Func, Table, array>		 
											  || can_for_each<Func, Table, std::string>  
											  || can_for_each<Func, Table, int64_t>		 
											  || can_for_each<Func, Table, double>		 
											  || can_for_each<Func, Table, bool>		 
											  || can_for_each<Func, Table, date>		 
											  || can_for_each<Func, Table, time>		 
											  || can_for_each<Func, Table, date_time>;

		template <typename Func, typename Table, typename T>
		static constexpr bool for_each_is_nothrow_one = !can_for_each<Func, Table, T>  
													 || can_for_each_nothrow<Func, Table, T>;

		 

		  template <typename Func, typename Table>
		  static constexpr bool for_each_is_nothrow = for_each_is_nothrow_one<Func, Table, table>		 
												   && for_each_is_nothrow_one<Func, Table, array>		 
												   && for_each_is_nothrow_one<Func, Table, std::string>  
												   && for_each_is_nothrow_one<Func, Table, int64_t>		 
												   && for_each_is_nothrow_one<Func, Table, double>		 
												   && for_each_is_nothrow_one<Func, Table, bool>		 
												   && for_each_is_nothrow_one<Func, Table, date>		 
												   && for_each_is_nothrow_one<Func, Table, time>		 
												   && for_each_is_nothrow_one<Func, Table, date_time>;

		 

		template <typename Func, typename Table>
		static void do_for_each(Func&& visitor, Table&& tbl) noexcept(for_each_is_nothrow<Func&&, Table&&>)
		{
			static_assert(can_for_each_any<Func&&, Table&&>,
						  "TOML table for_each visitors must be invocable for at least one of the toml::node "
						  "specializations:" TOML_SA_NODE_TYPE_LIST);

			using kvp_type = impl::copy_cv<map_pair, std::remove_reference_t<Table>>;

			for (kvp_type& kvp : tbl.map_)
			{
				using node_ref = impl::copy_cvref<toml::node, Table&&>;
				static_assert(std::is_reference_v<node_ref>);

				const auto keep_going =
					static_cast<node_ref>(*kvp.second)
						.visit(
							[&](auto&& v)
#if !TOML_MSVC  
								noexcept(for_each_is_nothrow_one<Func&&, Table&&, decltype(v)>)
#endif
							{
								using value_ref = for_each_value_ref<decltype(v), Table&&>;
								static_assert(std::is_reference_v<value_ref>);

								if constexpr (std::is_invocable_v<Func&&, const key&, value_ref>)
								{
									using return_type = decltype(static_cast<Func&&>(
										visitor)(static_cast<const key&>(kvp.first), static_cast<value_ref>(v)));

									if constexpr (impl::is_constructible_or_convertible<bool, return_type>)
									{
										return static_cast<bool>(static_cast<Func&&>(
											visitor)(static_cast<const key&>(kvp.first), static_cast<value_ref>(v)));
									}
									else
									{
										static_cast<Func&&>(visitor)(static_cast<const key&>(kvp.first),
																	 static_cast<value_ref>(v));
										return true;
									}
								}

								else if constexpr (std::is_invocable_v<Func&&, value_ref>)
								{
									using return_type =
										decltype(static_cast<Func&&>(visitor)(static_cast<value_ref>(v)));

									if constexpr (impl::is_constructible_or_convertible<bool, return_type>)
									{
										return static_cast<bool>(
											static_cast<Func&&>(visitor)(static_cast<value_ref>(v)));
									}
									else
									{
										static_cast<Func&&>(visitor)(static_cast<value_ref>(v));
										return true;
									}
								}

								else
									return true;
							});

				if (!keep_going)
					return;
			}
		}

	  public:

		template <typename Func>
		table& for_each(Func&& visitor) & noexcept(for_each_is_nothrow<Func&&, table&>)
		{
			do_for_each(static_cast<Func&&>(visitor), *this);
			return *this;
		}

		template <typename Func>
		table&& for_each(Func&& visitor) && noexcept(for_each_is_nothrow<Func&&, table&&>)
		{
			do_for_each(static_cast<Func&&>(visitor), static_cast<table&&>(*this));
			return static_cast<table&&>(*this);
		}

		template <typename Func>
		const table& for_each(Func&& visitor) const& noexcept(for_each_is_nothrow<Func&&, const table&>)
		{
			do_for_each(static_cast<Func&&>(visitor), *this);
			return *this;
		}

		template <typename Func>
		const table&& for_each(Func&& visitor) const&& noexcept(for_each_is_nothrow<Func&&, const table&&>)
		{
			do_for_each(static_cast<Func&&>(visitor), static_cast<const table&&>(*this));
			return static_cast<const table&&>(*this);
		}

		TOML_PURE_INLINE_GETTER
		bool empty() const noexcept
		{
			return map_.empty();
		}

		TOML_PURE_INLINE_GETTER
		size_t size() const noexcept
		{
			return map_.size();
		}

	  private:

		TOML_PURE_GETTER
		TOML_EXPORTED_MEMBER_FUNCTION
		map_iterator get_lower_bound(std::string_view) noexcept;

	  public:

		TOML_PURE_GETTER
		iterator lower_bound(std::string_view key) noexcept
		{
			return iterator{ get_lower_bound(key) };
		}

		TOML_PURE_GETTER
		const_iterator lower_bound(std::string_view key) const noexcept
		{
			return const_iterator{ const_cast<table&>(*this).get_lower_bound(key) };
		}

#if TOML_ENABLE_WINDOWS_COMPAT

		TOML_NODISCARD
		iterator lower_bound(std::wstring_view key)
		{
			if (empty())
				return end();

			return lower_bound(impl::narrow(key));
		}

		TOML_NODISCARD
		const_iterator lower_bound(std::wstring_view key) const
		{
			if (empty())
				return end();

			return lower_bound(impl::narrow(key));
		}

#endif  

		TOML_PURE_GETTER
		TOML_EXPORTED_MEMBER_FUNCTION
		iterator find(std::string_view key) noexcept;

		TOML_PURE_GETTER
		TOML_EXPORTED_MEMBER_FUNCTION
		const_iterator find(std::string_view key) const noexcept;

		TOML_PURE_GETTER
		bool contains(std::string_view key) const noexcept
		{
			return get(key) != nullptr;
		}

#if TOML_ENABLE_WINDOWS_COMPAT

		TOML_NODISCARD
		iterator find(std::wstring_view key)
		{
			if (empty())
				return end();

			return find(impl::narrow(key));
		}

		TOML_NODISCARD
		const_iterator find(std::wstring_view key) const
		{
			return find(impl::narrow(key));
		}

		TOML_NODISCARD
		bool contains(std::wstring_view key) const
		{
			return contains(impl::narrow(key));
		}

#endif  

	  private:

		TOML_EXPORTED_MEMBER_FUNCTION
		map_iterator erase(const_map_iterator) noexcept;

		TOML_EXPORTED_MEMBER_FUNCTION
		map_iterator erase(const_map_iterator, const_map_iterator) noexcept;

	  public:

		iterator erase(iterator pos) noexcept
		{
			return iterator{ erase(const_map_iterator{ pos }) };
		}

		iterator erase(const_iterator pos) noexcept
		{
			return iterator{ erase(const_map_iterator{ pos }) };
		}

		iterator erase(const_iterator begin, const_iterator end) noexcept
		{
			return iterator{ erase(const_map_iterator{ begin }, const_map_iterator{ end }) };
		}

		TOML_EXPORTED_MEMBER_FUNCTION
		size_t erase(std::string_view key) noexcept;

#if TOML_ENABLE_WINDOWS_COMPAT

		size_t erase(std::wstring_view key)
		{
			if (empty())
				return false;

			return erase(impl::narrow(key));
		}

#endif  

		TOML_EXPORTED_MEMBER_FUNCTION
		table& prune(bool recursive = true) & noexcept;

		table&& prune(bool recursive = true) && noexcept
		{
			return static_cast<toml::table&&>(this->prune(recursive));
		}

		TOML_EXPORTED_MEMBER_FUNCTION
		void clear() noexcept;

	  private:

		TOML_EXPORTED_MEMBER_FUNCTION
		map_iterator insert_with_hint(const_iterator, key&&, impl::node_ptr&&);

	  public:

		TOML_CONSTRAINED_TEMPLATE((is_key_or_convertible<KeyType&&> || impl::is_wide_string<KeyType>),
								  typename ValueType = void,
								  typename KeyType,
								  typename... ValueArgs)
		iterator emplace_hint(const_iterator hint, KeyType&& key, ValueArgs&&... args)
		{
			static_assert(!impl::is_wide_string<KeyType> || TOML_ENABLE_WINDOWS_COMPAT,
						  "Emplacement using wide-character keys is only supported on Windows with "
						  "TOML_ENABLE_WINDOWS_COMPAT enabled.");

			using raw_value_type = impl::remove_cvref<ValueType>;
			using value_type	 = std::
				conditional_t<std::is_void_v<raw_value_type>, impl::emplaced_type_of<ValueArgs&&...>, raw_value_type>;

			if constexpr (impl::is_wide_string<KeyType>)
			{
#if TOML_ENABLE_WINDOWS_COMPAT
				return emplace_hint<value_type>(hint,
												impl::narrow(static_cast<KeyType&&>(key)),
												static_cast<ValueArgs&&>(args)...);
#else
				static_assert(impl::dependent_false<KeyType>, "Evaluated unreachable branch!");
#endif
			}
			else
			{
				static constexpr auto moving_node_ptr = std::is_same_v<value_type, impl::node_ptr>  
													 && sizeof...(ValueArgs) == 1u				    
													 && impl::first_is_same<impl::node_ptr&&, ValueArgs&&...>;
				using unwrapped_type = impl::remove_cvref<impl::unwrap_node<value_type>>;

				static_assert(moving_node_ptr										 
								  || impl::is_native<unwrapped_type>				 
								  || impl::is_one_of<unwrapped_type, table, array>,  
							  "ValueType argument of table::emplace_hint() must be one "
							  "of:" TOML_SA_UNWRAPPED_NODE_TYPE_LIST);

				map_iterator ipos = insert_with_hint(hint, toml::key{ static_cast<KeyType&&>(key) }, nullptr);

				if (!ipos->second)
				{
					if constexpr (moving_node_ptr)
						ipos->second = std::move(static_cast<ValueArgs&&>(args)...);
					else
					{
#if TOML_COMPILER_HAS_EXCEPTIONS
						try
						{
#endif
							ipos->second.reset(
								new impl::wrap_node<unwrapped_type>{ static_cast<ValueArgs&&>(args)... });
#if TOML_COMPILER_HAS_EXCEPTIONS
						}
						catch (...)
						{
							erase(const_map_iterator{ ipos });  
							throw;
						}
#endif
					}
				}
				return iterator{ ipos };
			}
		}

		TOML_CONSTRAINED_TEMPLATE((is_key_or_convertible<KeyType&&> || impl::is_wide_string<KeyType>),
								  typename KeyType,
								  typename ValueType)
		std::pair<iterator, bool> insert(KeyType&& key,
										 ValueType&& val,
										 value_flags flags = preserve_source_value_flags)
		{
			static_assert(!impl::is_wide_string<KeyType> || TOML_ENABLE_WINDOWS_COMPAT,
						  "Insertion using wide-character keys is only supported on Windows with "
						  "TOML_ENABLE_WINDOWS_COMPAT enabled.");

			if constexpr (is_node_view<ValueType>)
			{
				if (!val)
					return { end(), false };
			}

			if constexpr (impl::is_wide_string<KeyType>)
			{
#if TOML_ENABLE_WINDOWS_COMPAT
				return insert(impl::narrow(static_cast<KeyType&&>(key)), static_cast<ValueType&&>(val), flags);
#else
				static_assert(impl::dependent_false<KeyType>, "Evaluated unreachable branch!");
#endif
			}
			else
			{
				const auto key_view = std::string_view{ key };
				map_iterator ipos	= get_lower_bound(key_view);
				if (ipos == map_.end() || ipos->first != key_view)
				{
					ipos = insert_with_hint(const_iterator{ ipos },
											toml::key{ static_cast<KeyType&&>(key) },
											impl::make_node(static_cast<ValueType&&>(val), flags));
					return { iterator{ ipos }, true };
				}
				return { iterator{ ipos }, false };
			}
		}

		TOML_CONSTRAINED_TEMPLATE((!is_key_or_convertible<Iter> && !impl::is_wide_string<Iter>), typename Iter)
		void insert(Iter begin, Iter end, value_flags flags = preserve_source_value_flags)
		{
			if (begin == end)
				return;
			for (auto it = begin; it != end; it++)
			{
				if constexpr (std::is_rvalue_reference_v<decltype(*it)>)
					insert(std::move((*it).first), std::move((*it).second), flags);
				else
					insert((*it).first, (*it).second, flags);
			}
		}

		TOML_CONSTRAINED_TEMPLATE((is_key_or_convertible<KeyType&&> || impl::is_wide_string<KeyType>),
								  typename KeyType,
								  typename ValueType)
		std::pair<iterator, bool> insert_or_assign(KeyType&& key,
												   ValueType&& val,
												   value_flags flags = preserve_source_value_flags)
		{
			static_assert(!impl::is_wide_string<KeyType> || TOML_ENABLE_WINDOWS_COMPAT,
						  "Insertion using wide-character keys is only supported on Windows with "
						  "TOML_ENABLE_WINDOWS_COMPAT enabled.");

			if constexpr (is_node_view<ValueType>)
			{
				if (!val)
					return { end(), false };
			}

			if constexpr (impl::is_wide_string<KeyType>)
			{
#if TOML_ENABLE_WINDOWS_COMPAT
				return insert_or_assign(impl::narrow(static_cast<KeyType&&>(key)),
										static_cast<ValueType&&>(val),
										flags);
#else
				static_assert(impl::dependent_false<KeyType>, "Evaluated unreachable branch!");
#endif
			}
			else
			{
				const auto key_view = std::string_view{ key };
				map_iterator ipos	= get_lower_bound(key_view);
				if (ipos == map_.end() || ipos->first != key_view)
				{
					ipos = insert_with_hint(const_iterator{ ipos },
											toml::key{ static_cast<KeyType&&>(key) },
											impl::make_node(static_cast<ValueType&&>(val), flags));
					return { iterator{ ipos }, true };
				}
				else
				{
					(*ipos).second = impl::make_node(static_cast<ValueType&&>(val), flags);
					return { iterator{ ipos }, false };
				}
			}
		}

		TOML_CONSTRAINED_TEMPLATE((is_key_or_convertible<KeyType&&> || impl::is_wide_string<KeyType>),
								  typename ValueType = void,
								  typename KeyType,
								  typename... ValueArgs)
		std::pair<iterator, bool> emplace(KeyType&& key, ValueArgs&&... args)
		{
			static_assert(!impl::is_wide_string<KeyType> || TOML_ENABLE_WINDOWS_COMPAT,
						  "Emplacement using wide-character keys is only supported on Windows with "
						  "TOML_ENABLE_WINDOWS_COMPAT enabled.");

			using raw_value_type = impl::remove_cvref<ValueType>;
			using value_type	 = std::
				conditional_t<std::is_void_v<raw_value_type>, impl::emplaced_type_of<ValueArgs&&...>, raw_value_type>;

			if constexpr (impl::is_wide_string<KeyType>)
			{
#if TOML_ENABLE_WINDOWS_COMPAT
				return emplace<value_type>(impl::narrow(static_cast<KeyType&&>(key)),
										   static_cast<ValueArgs&&>(args)...);
#else
				static_assert(impl::dependent_false<KeyType>, "Evaluated unreachable branch!");
#endif
			}
			else
			{
				using unwrapped_type = impl::remove_cvref<impl::unwrap_node<value_type>>;
				static_assert((impl::is_native<unwrapped_type> || impl::is_one_of<unwrapped_type, table, array>),
							  "ValueType argument of table::emplace() must be one "
							  "of:" TOML_SA_UNWRAPPED_NODE_TYPE_LIST);

				const auto key_view = std::string_view{ key };
				auto ipos			= get_lower_bound(key_view);
				if (ipos == map_.end() || ipos->first != key_view)
				{
					ipos = insert_with_hint(
						const_iterator{ ipos },
						toml::key{ static_cast<KeyType&&>(key) },
						impl::node_ptr{ new impl::wrap_node<unwrapped_type>{ static_cast<ValueArgs&&>(args)... } });
					return { iterator{ ipos }, true };
				}
				return { iterator{ ipos }, false };
			}
		}

		using node::operator[];  

		TOML_NODISCARD
		node_view<node> operator[](std::string_view key) noexcept
		{
			return node_view<node>{ get(key) };
		}

		TOML_NODISCARD
		node_view<const node> operator[](std::string_view key) const noexcept
		{
			return node_view<const node>{ get(key) };
		}

#if TOML_ENABLE_WINDOWS_COMPAT

		TOML_NODISCARD
		node_view<node> operator[](std::wstring_view key)
		{
			return node_view<node>{ get(key) };
		}

		TOML_NODISCARD
		node_view<const node> operator[](std::wstring_view key) const
		{
			return node_view<const node>{ get(key) };
		}

#endif  

	  private:

		TOML_PURE_GETTER
		TOML_EXPORTED_STATIC_FUNCTION
		static bool TOML_CALLCONV equal(const table&, const table&) noexcept;

	  public:

		TOML_NODISCARD
		friend bool operator==(const table& lhs, const table& rhs) noexcept
		{
			return equal(lhs, rhs);
		}

		TOML_NODISCARD
		friend bool operator!=(const table& lhs, const table& rhs) noexcept
		{
			return !equal(lhs, rhs);
		}

#if TOML_ENABLE_FORMATTERS

		friend std::ostream& operator<<(std::ostream& lhs, const table& rhs)
		{
			impl::print_to_stream(lhs, rhs);
			return lhs;
		}

#endif
	};
}
TOML_NAMESPACE_END;

#include "header_end.h"
