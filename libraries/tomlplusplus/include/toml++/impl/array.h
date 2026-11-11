//# This file is a part of toml++ and is subject to the the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT
#pragma once

#include "std_utility.h"
#include "std_vector.h"
#include "std_initializer_list.h"
#include "value.h"
#include "make_node.h"
#include "header_start.h"

TOML_IMPL_NAMESPACE_START
{
	template <bool IsConst>
	class TOML_TRIVIAL_ABI array_iterator
	{
	  private:
		template <bool>
		friend class array_iterator;

		using mutable_vector_iterator = std::vector<node_ptr>::iterator;
		using const_vector_iterator	  = std::vector<node_ptr>::const_iterator;
		using vector_iterator		  = std::conditional_t<IsConst, const_vector_iterator, mutable_vector_iterator>;

		mutable vector_iterator iter_;

	  public:
		using value_type		= std::conditional_t<IsConst, const node, node>;
		using reference			= value_type&;
		using pointer			= value_type*;
		using difference_type	= ptrdiff_t;
		using iterator_category = typename std::iterator_traits<vector_iterator>::iterator_category;

		TOML_NODISCARD_CTOR
		array_iterator() noexcept = default;

		TOML_NODISCARD_CTOR
		explicit array_iterator(mutable_vector_iterator iter) noexcept  
			: iter_{ iter }
		{}

		TOML_CONSTRAINED_TEMPLATE(C, bool C = IsConst)
		TOML_NODISCARD_CTOR
		explicit array_iterator(const_vector_iterator iter) noexcept  
			: iter_{ iter }
		{}

		TOML_CONSTRAINED_TEMPLATE(C, bool C = IsConst)
		TOML_NODISCARD_CTOR
		array_iterator(const array_iterator<false>& other) noexcept  
			: iter_{ other.iter_ }
		{}

		TOML_NODISCARD_CTOR
		array_iterator(const array_iterator&) noexcept = default;

		array_iterator& operator=(const array_iterator&) noexcept = default;

		array_iterator& operator++() noexcept  
		{
			++iter_;
			return *this;
		}

		array_iterator operator++(int) noexcept  
		{
			array_iterator out{ iter_ };
			++iter_;
			return out;
		}

		array_iterator& operator--() noexcept  
		{
			--iter_;
			return *this;
		}

		array_iterator operator--(int) noexcept  
		{
			array_iterator out{ iter_ };
			--iter_;
			return out;
		}

		TOML_PURE_INLINE_GETTER
		reference operator*() const noexcept
		{
			return *iter_->get();
		}

		TOML_PURE_INLINE_GETTER
		pointer operator->() const noexcept
		{
			return iter_->get();
		}

		TOML_PURE_INLINE_GETTER
		explicit operator const vector_iterator&() const noexcept
		{
			return iter_;
		}

		TOML_CONSTRAINED_TEMPLATE(!C, bool C = IsConst)
		TOML_PURE_INLINE_GETTER
		explicit operator const const_vector_iterator() const noexcept
		{
			return iter_;
		}

		array_iterator& operator+=(ptrdiff_t rhs) noexcept
		{
			iter_ += rhs;
			return *this;
		}

		array_iterator& operator-=(ptrdiff_t rhs) noexcept
		{
			iter_ -= rhs;
			return *this;
		}

		TOML_NODISCARD
		friend array_iterator operator+(const array_iterator& lhs, ptrdiff_t rhs) noexcept
		{
			return array_iterator{ lhs.iter_ + rhs };
		}

		TOML_NODISCARD
		friend array_iterator operator+(ptrdiff_t lhs, const array_iterator& rhs) noexcept
		{
			return array_iterator{ rhs.iter_ + lhs };
		}

		TOML_NODISCARD
		friend array_iterator operator-(const array_iterator& lhs, ptrdiff_t rhs) noexcept
		{
			return array_iterator{ lhs.iter_ - rhs };
		}

		TOML_PURE_INLINE_GETTER
		friend ptrdiff_t operator-(const array_iterator& lhs, const array_iterator& rhs) noexcept
		{
			return lhs.iter_ - rhs.iter_;
		}

		TOML_PURE_INLINE_GETTER
		friend bool operator==(const array_iterator& lhs, const array_iterator& rhs) noexcept
		{
			return lhs.iter_ == rhs.iter_;
		}

		TOML_PURE_INLINE_GETTER
		friend bool operator!=(const array_iterator& lhs, const array_iterator& rhs) noexcept
		{
			return lhs.iter_ != rhs.iter_;
		}

		TOML_PURE_INLINE_GETTER
		friend bool operator<(const array_iterator& lhs, const array_iterator& rhs) noexcept
		{
			return lhs.iter_ < rhs.iter_;
		}

		TOML_PURE_INLINE_GETTER
		friend bool operator<=(const array_iterator& lhs, const array_iterator& rhs) noexcept
		{
			return lhs.iter_ <= rhs.iter_;
		}

		TOML_PURE_INLINE_GETTER
		friend bool operator>(const array_iterator& lhs, const array_iterator& rhs) noexcept
		{
			return lhs.iter_ > rhs.iter_;
		}

		TOML_PURE_INLINE_GETTER
		friend bool operator>=(const array_iterator& lhs, const array_iterator& rhs) noexcept
		{
			return lhs.iter_ >= rhs.iter_;
		}

		TOML_PURE_INLINE_GETTER
		reference operator[](ptrdiff_t idx) const noexcept
		{
			return *(iter_ + idx)->get();
		}
	};

	struct array_init_elem
	{
		mutable node_ptr value;

		template <typename T>
		TOML_NODISCARD_CTOR
		array_init_elem(T&& val, value_flags flags = preserve_source_value_flags)  
			: value{ make_node(static_cast<T&&>(val), flags) }
		{}
	};
}
TOML_IMPL_NAMESPACE_END;

TOML_NAMESPACE_START
{

	using array_iterator = POXY_IMPLEMENTATION_DETAIL(impl::array_iterator<false>);

	using const_array_iterator = POXY_IMPLEMENTATION_DETAIL(impl::array_iterator<true>);

	class TOML_EXPORTED_CLASS array : public node
	{
	  private:

		using vector_type			= std::vector<impl::node_ptr>;
		using vector_iterator		= typename vector_type::iterator;
		using const_vector_iterator = typename vector_type::const_iterator;
		vector_type elems_;

		TOML_NODISCARD_CTOR
		TOML_EXPORTED_MEMBER_FUNCTION
		array(const impl::array_init_elem*, const impl::array_init_elem*);

		TOML_NODISCARD_CTOR
		array(std::false_type, std::initializer_list<impl::array_init_elem> elems)  
			: array{ elems.begin(), elems.end() }
		{}

		TOML_EXPORTED_MEMBER_FUNCTION
		void preinsertion_resize(size_t idx, size_t count);

		TOML_EXPORTED_MEMBER_FUNCTION
		void insert_at_back(impl::node_ptr&&);

		TOML_EXPORTED_MEMBER_FUNCTION
		vector_iterator insert_at(const_vector_iterator, impl::node_ptr&&);

		template <typename T>
		void emplace_back_if_not_empty_view(T&& val, value_flags flags)
		{
			if constexpr (is_node_view<T>)
			{
				if (!val)
					return;
			}
			insert_at_back(impl::make_node(static_cast<T&&>(val), flags));
		}

		TOML_NODISCARD
		TOML_EXPORTED_MEMBER_FUNCTION
		size_t total_leaf_count() const noexcept;

		TOML_EXPORTED_MEMBER_FUNCTION
		void flatten_child(array&& child, size_t& dest_index) noexcept;

	  public:
		using value_type	  = node;
		using size_type		  = size_t;
		using difference_type = ptrdiff_t;
		using reference		  = node&;
		using const_reference = const node&;

		TOML_NODISCARD_CTOR
		TOML_EXPORTED_MEMBER_FUNCTION
		array() noexcept;

		TOML_EXPORTED_MEMBER_FUNCTION
		~array() noexcept;

		TOML_NODISCARD_CTOR
		TOML_EXPORTED_MEMBER_FUNCTION
		array(const array&);

		TOML_NODISCARD_CTOR
		TOML_EXPORTED_MEMBER_FUNCTION
		array(array&& other) noexcept;

		TOML_CONSTRAINED_TEMPLATE((sizeof...(ElemTypes) > 0 || !std::is_same_v<impl::remove_cvref<ElemType>, array>),
								  typename ElemType,
								  typename... ElemTypes)
		TOML_NODISCARD_CTOR
		explicit array(ElemType&& val, ElemTypes&&... vals)
			: array{ std::false_type{},
					 std::initializer_list<impl::array_init_elem>{ static_cast<ElemType&&>(val),
																   static_cast<ElemTypes&&>(vals)... } }
		{}

		TOML_EXPORTED_MEMBER_FUNCTION
		array& operator=(const array&);

		TOML_EXPORTED_MEMBER_FUNCTION
		array& operator=(array&& rhs) noexcept;

		TOML_CONST_INLINE_GETTER
		node_type type() const noexcept final
		{
			return node_type::array;
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
						  "The template type argument of array::is_homogeneous() must be void or one "
						  "of:" TOML_SA_UNWRAPPED_NODE_TYPE_LIST);

			return is_homogeneous(impl::node_type_of<type>);
		}

		TOML_CONST_INLINE_GETTER
		bool is_table() const noexcept final
		{
			return false;
		}

		TOML_CONST_INLINE_GETTER
		bool is_array() const noexcept final
		{
			return true;
		}

		TOML_PURE_GETTER
		bool is_array_of_tables() const noexcept final
		{
			return is_homogeneous(node_type::table);
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
			return nullptr;
		}

		TOML_CONST_INLINE_GETTER
		array* as_array() noexcept final
		{
			return this;
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
			return nullptr;
		}

		TOML_CONST_INLINE_GETTER
		const array* as_array() const noexcept final
		{
			return this;
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
		node* get(size_t index) noexcept
		{
			return index < elems_.size() ? elems_[index].get() : nullptr;
		}

		TOML_PURE_INLINE_GETTER
		const node* get(size_t index) const noexcept
		{
			return const_cast<array&>(*this).get(index);
		}

		template <typename ElemType>
		TOML_NODISCARD
		impl::wrap_node<ElemType>* get_as(size_t index) noexcept
		{
			if (auto val = get(index))
				return val->template as<ElemType>();
			return nullptr;
		}

		template <typename ElemType>
		TOML_NODISCARD
		const impl::wrap_node<ElemType>* get_as(size_t index) const noexcept
		{
			return const_cast<array&>(*this).template get_as<ElemType>(index);
		}

		using node::operator[];  

		TOML_NODISCARD
		node& operator[](size_t index) noexcept
		{
			return *elems_[index];
		}

		TOML_NODISCARD
		const node& operator[](size_t index) const noexcept
		{
			return *elems_[index];
		}

		TOML_NODISCARD
		TOML_EXPORTED_MEMBER_FUNCTION
		node& at(size_t index);

		TOML_NODISCARD
		const node& at(size_t index) const
		{
			return const_cast<array&>(*this).at(index);
		}

		TOML_NODISCARD
		node& front() noexcept
		{
			return *elems_.front();
		}

		TOML_NODISCARD
		const node& front() const noexcept
		{
			return *elems_.front();
		}

		TOML_NODISCARD
		node& back() noexcept
		{
			return *elems_.back();
		}

		TOML_NODISCARD
		const node& back() const noexcept
		{
			return *elems_.back();
		}

		using iterator = array_iterator;

		using const_iterator = const_array_iterator;

		TOML_NODISCARD
		iterator begin() noexcept
		{
			return iterator{ elems_.begin() };
		}

		TOML_NODISCARD
		const_iterator begin() const noexcept
		{
			return const_iterator{ elems_.cbegin() };
		}

		TOML_NODISCARD
		const_iterator cbegin() const noexcept
		{
			return const_iterator{ elems_.cbegin() };
		}

		TOML_NODISCARD
		iterator end() noexcept
		{
			return iterator{ elems_.end() };
		}

		TOML_NODISCARD
		const_iterator end() const noexcept
		{
			return const_iterator{ elems_.cend() };
		}

		TOML_NODISCARD
		const_iterator cend() const noexcept
		{
			return const_iterator{ elems_.cend() };
		}

	  private:

		template <typename T, typename Array>
		using for_each_elem_ref = impl::copy_cvref<impl::wrap_node<impl::remove_cvref<impl::unwrap_node<T>>>, Array>;

		template <typename Func, typename Array, typename T>
		static constexpr bool can_for_each = std::is_invocable_v<Func, for_each_elem_ref<T, Array>, size_t>  
										  || std::is_invocable_v<Func, size_t, for_each_elem_ref<T, Array>>  
										  || std::is_invocable_v<Func, for_each_elem_ref<T, Array>>;

		template <typename Func, typename Array, typename T>
		static constexpr bool can_for_each_nothrow =
			std::is_nothrow_invocable_v<Func, for_each_elem_ref<T, Array>, size_t>	   
			|| std::is_nothrow_invocable_v<Func, size_t, for_each_elem_ref<T, Array>>  
			|| std::is_nothrow_invocable_v<Func, for_each_elem_ref<T, Array>>;

		template <typename Func, typename Array>
		static constexpr bool can_for_each_any = can_for_each<Func, Array, table>		 
											  || can_for_each<Func, Array, array>		 
											  || can_for_each<Func, Array, std::string>  
											  || can_for_each<Func, Array, int64_t>		 
											  || can_for_each<Func, Array, double>		 
											  || can_for_each<Func, Array, bool>		 
											  || can_for_each<Func, Array, date>		 
											  || can_for_each<Func, Array, time>		 
											  || can_for_each<Func, Array, date_time>;

		template <typename Func, typename Array, typename T>
		static constexpr bool for_each_is_nothrow_one = !can_for_each<Func, Array, T>  
													 || can_for_each_nothrow<Func, Array, T>;

		// clang-format off

		template <typename Func, typename Array>
		static constexpr bool for_each_is_nothrow = for_each_is_nothrow_one<Func, Array, table>		   
												 && for_each_is_nothrow_one<Func, Array, array>		   
												 && for_each_is_nothrow_one<Func, Array, std::string>  
												 && for_each_is_nothrow_one<Func, Array, int64_t>	   
												 && for_each_is_nothrow_one<Func, Array, double>	   
												 && for_each_is_nothrow_one<Func, Array, bool>		   
												 && for_each_is_nothrow_one<Func, Array, date>		   
												 && for_each_is_nothrow_one<Func, Array, time>		   
												 && for_each_is_nothrow_one<Func, Array, date_time>;

		// clang-format on

		template <typename Func, typename Array>
		static void do_for_each(Func&& visitor, Array&& arr) noexcept(for_each_is_nothrow<Func&&, Array&&>)
		{
			static_assert(can_for_each_any<Func&&, Array&&>,
						  "TOML array for_each visitors must be invocable for at least one of the toml::node "
						  "specializations:" TOML_SA_NODE_TYPE_LIST);

			for (size_t i = 0; i < arr.size(); i++)
			{
				using node_ref = impl::copy_cvref<toml::node, Array&&>;
				static_assert(std::is_reference_v<node_ref>);

				const auto keep_going =
					static_cast<node_ref>(static_cast<Array&&>(arr)[i])
						.visit(
							[&](auto&& elem)
#if !TOML_MSVC  
								noexcept(for_each_is_nothrow_one<Func&&, Array&&, decltype(elem)>)
#endif
							{
								using elem_ref = for_each_elem_ref<decltype(elem), Array&&>;
								static_assert(std::is_reference_v<elem_ref>);

								if constexpr (std::is_invocable_v<Func&&, elem_ref, size_t>)
								{
									using return_type =
										decltype(static_cast<Func&&>(visitor)(static_cast<elem_ref>(elem), i));

									if constexpr (impl::is_constructible_or_convertible<bool, return_type>)
									{
										return static_cast<bool>(
											static_cast<Func&&>(visitor)(static_cast<elem_ref>(elem), i));
									}
									else
									{
										static_cast<Func&&>(visitor)(static_cast<elem_ref>(elem), i);
										return true;
									}
								}

								else if constexpr (std::is_invocable_v<Func&&, size_t, elem_ref>)
								{
									using return_type =
										decltype(static_cast<Func&&>(visitor)(i, static_cast<elem_ref>(elem)));

									if constexpr (impl::is_constructible_or_convertible<bool, return_type>)
									{
										return static_cast<bool>(
											static_cast<Func&&>(visitor)(i, static_cast<elem_ref>(elem)));
									}
									else
									{
										static_cast<Func&&>(visitor)(i, static_cast<elem_ref>(elem));
										return true;
									}
								}

								else if constexpr (std::is_invocable_v<Func&&, elem_ref>)
								{
									using return_type =
										decltype(static_cast<Func&&>(visitor)(static_cast<elem_ref>(elem)));

									if constexpr (impl::is_constructible_or_convertible<bool, return_type>)
									{
										return static_cast<bool>(
											static_cast<Func&&>(visitor)(static_cast<elem_ref>(elem)));
									}
									else
									{
										static_cast<Func&&>(visitor)(static_cast<elem_ref>(elem));
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
		array& for_each(Func&& visitor) & noexcept(for_each_is_nothrow<Func&&, array&>)
		{
			do_for_each(static_cast<Func&&>(visitor), *this);
			return *this;
		}

		template <typename Func>
		array&& for_each(Func&& visitor) && noexcept(for_each_is_nothrow<Func&&, array&&>)
		{
			do_for_each(static_cast<Func&&>(visitor), static_cast<array&&>(*this));
			return static_cast<array&&>(*this);
		}

		template <typename Func>
		const array& for_each(Func&& visitor) const& noexcept(for_each_is_nothrow<Func&&, const array&>)
		{
			do_for_each(static_cast<Func&&>(visitor), *this);
			return *this;
		}

		template <typename Func>
		const array&& for_each(Func&& visitor) const&& noexcept(for_each_is_nothrow<Func&&, const array&&>)
		{
			do_for_each(static_cast<Func&&>(visitor), static_cast<const array&&>(*this));
			return static_cast<const array&&>(*this);
		}

		TOML_NODISCARD
		bool empty() const noexcept
		{
			return elems_.empty();
		}

		TOML_NODISCARD
		size_t size() const noexcept
		{
			return elems_.size();
		}

		TOML_NODISCARD
		size_t max_size() const noexcept
		{
			return elems_.max_size();
		}

		TOML_NODISCARD
		size_t capacity() const noexcept
		{
			return elems_.capacity();
		}

		TOML_EXPORTED_MEMBER_FUNCTION
		void reserve(size_t new_capacity);

		TOML_EXPORTED_MEMBER_FUNCTION
		void shrink_to_fit();

		TOML_EXPORTED_MEMBER_FUNCTION
		void truncate(size_t new_size);

		template <typename ElemType>
		void resize(size_t new_size,
					ElemType&& default_init_val,
					value_flags default_init_flags = preserve_source_value_flags)
		{
			static_assert(!is_node_view<ElemType>,
						  "The default element type argument to toml::array::resize may not be toml::node_view.");

			if (!new_size)
				clear();
			else if (new_size > elems_.size())
				insert(cend(), new_size - elems_.size(), static_cast<ElemType&&>(default_init_val), default_init_flags);
			else
				truncate(new_size);
		}

		TOML_EXPORTED_MEMBER_FUNCTION
		iterator erase(const_iterator pos) noexcept;

		TOML_EXPORTED_MEMBER_FUNCTION
		iterator erase(const_iterator first, const_iterator last) noexcept;

		TOML_EXPORTED_MEMBER_FUNCTION
		array& flatten() &;

		array&& flatten() &&
		{
			return static_cast<toml::array&&>(this->flatten());
		}

		TOML_EXPORTED_MEMBER_FUNCTION
		array& prune(bool recursive = true) & noexcept;

		array&& prune(bool recursive = true) && noexcept
		{
			return static_cast<toml::array&&>(this->prune(recursive));
		}

		TOML_EXPORTED_MEMBER_FUNCTION
		void pop_back() noexcept;

		TOML_EXPORTED_MEMBER_FUNCTION
		void clear() noexcept;

		template <typename ElemType>
		iterator insert(const_iterator pos, ElemType&& val, value_flags flags = preserve_source_value_flags)
		{
			if constexpr (is_node_view<ElemType>)
			{
				if (!val)
					return end();
			}
			return iterator{ insert_at(const_vector_iterator{ pos },
									   impl::make_node(static_cast<ElemType&&>(val), flags)) };
		}

		template <typename ElemType>
		iterator insert(const_iterator pos,
						size_t count,
						ElemType&& val,
						value_flags flags = preserve_source_value_flags)
		{
			if constexpr (is_node_view<ElemType>)
			{
				if (!val)
					return end();
			}
			switch (count)
			{
				case 0: return iterator{ elems_.begin() + (const_vector_iterator{ pos } - elems_.cbegin()) };
				case 1: return insert(pos, static_cast<ElemType&&>(val), flags);
				default:
				{
					const auto start_idx = static_cast<size_t>(const_vector_iterator{ pos } - elems_.cbegin());
					preinsertion_resize(start_idx, count);
					size_t i = start_idx;
					for (size_t e = start_idx + count - 1u; i < e; i++)
						elems_[i] = impl::make_node(val, flags);

					elems_[i] = impl::make_node(static_cast<ElemType&&>(val), flags);
					return iterator{ elems_.begin() + static_cast<ptrdiff_t>(start_idx) };
				}
			}
		}

		template <typename Iter>
		iterator insert(const_iterator pos, Iter first, Iter last, value_flags flags = preserve_source_value_flags)
		{
			const auto distance = std::distance(first, last);
			if (distance <= 0)
				return iterator{ elems_.begin() + (const_vector_iterator{ pos } - elems_.cbegin()) };
			else
			{
				auto count		 = distance;
				using deref_type = decltype(*first);
				if constexpr (is_node_view<deref_type>)
				{
					for (auto it = first; it != last; it++)
						if (!(*it))
							count--;
					if (!count)
						return iterator{ elems_.begin() + (const_vector_iterator{ pos } - elems_.cbegin()) };
				}
				const auto start_idx = static_cast<size_t>(const_vector_iterator{ pos } - elems_.cbegin());
				preinsertion_resize(start_idx, static_cast<size_t>(count));
				size_t i = start_idx;
				for (auto it = first; it != last; it++)
				{
					if constexpr (is_node_view<deref_type>)
					{
						if (!(*it))
							continue;
					}
					if constexpr (std::is_rvalue_reference_v<deref_type>)
						elems_[i++] = impl::make_node(std::move(*it), flags);
					else
						elems_[i++] = impl::make_node(*it, flags);
				}
				return iterator{ elems_.begin() + static_cast<ptrdiff_t>(start_idx) };
			}
		}

		template <typename ElemType>
		iterator insert(const_iterator pos,
						std::initializer_list<ElemType> ilist,
						value_flags flags = preserve_source_value_flags)
		{
			return insert(pos, ilist.begin(), ilist.end(), flags);
		}

		template <typename ElemType = void, typename... Args>
		iterator emplace(const_iterator pos, Args&&... args)
		{
			using raw_elem_type = impl::remove_cvref<ElemType>;
			using elem_type		= std::conditional_t<std::is_void_v<raw_elem_type>,  
												 impl::emplaced_type_of<Args&&...>,
												 raw_elem_type>;

			using type = impl::remove_cvref<impl::unwrap_node<elem_type>>;
			static_assert(impl::is_native<type> || impl::is_one_of<type, table, array>,
						  "Emplacement type parameter must be one of:" TOML_SA_UNWRAPPED_NODE_TYPE_LIST);

			return iterator{ insert_at(const_vector_iterator{ pos },
									   impl::node_ptr{ new impl::wrap_node<type>{ static_cast<Args&&>(args)... } }) };
		}

		template <typename ElemType>
		iterator replace(const_iterator pos, ElemType&& val, value_flags flags = preserve_source_value_flags)
		{
			TOML_ASSERT(pos >= cbegin() && pos < cend());

			if constexpr (is_node_view<ElemType>)
			{
				if (!val)
					return end();
			}

			const auto it = elems_.begin() + (const_vector_iterator{ pos } - elems_.cbegin());
			*it			  = impl::make_node(static_cast<ElemType&&>(val), flags);
			return iterator{ it };
		}

		template <typename ElemType>
		void push_back(ElemType&& val, value_flags flags = preserve_source_value_flags)
		{
			emplace_back_if_not_empty_view(static_cast<ElemType&&>(val), flags);
		}

		template <typename ElemType = void, typename... Args>
		decltype(auto) emplace_back(Args&&... args)
		{
			using raw_elem_type = impl::remove_cvref<ElemType>;
			using elem_type		= std::conditional_t<std::is_void_v<raw_elem_type>,  
												 impl::emplaced_type_of<Args&&...>,
												 raw_elem_type>;

			static constexpr auto moving_node_ptr = std::is_same_v<elem_type, impl::node_ptr>  
												 && sizeof...(Args) == 1u					   
												 && impl::first_is_same<impl::node_ptr&&, Args&&...>;

			using unwrapped_type = impl::remove_cvref<impl::unwrap_node<elem_type>>;

			static_assert(
				moving_node_ptr										   
					|| impl::is_native<unwrapped_type>				   
					|| impl::is_one_of<unwrapped_type, table, array>,  
				"ElemType argument of array::emplace_back() must be one of:" TOML_SA_UNWRAPPED_NODE_TYPE_LIST);

			if constexpr (moving_node_ptr)
			{
				insert_at_back(static_cast<Args&&>(args)...);
				return *elems_.back();
			}
			else
			{
				auto ptr = new impl::wrap_node<unwrapped_type>{ static_cast<Args&&>(args)... };
				insert_at_back(impl::node_ptr{ ptr });
				return *ptr;
			}
		}

	  private:

		TOML_NODISCARD
		TOML_EXPORTED_STATIC_FUNCTION
		static bool TOML_CALLCONV equal(const array&, const array&) noexcept;

		template <typename T>
		TOML_NODISCARD
		static bool equal_to_container(const array& lhs, const T& rhs) noexcept
		{
			using element_type = std::remove_const_t<typename T::value_type>;
			static_assert(impl::is_losslessly_convertible_to_native<element_type>,
						  "Container element type must be losslessly convertible one of the native TOML value types");

			if (lhs.size() != rhs.size())
				return false;
			if (rhs.size() == 0u)
				return true;

			size_t i{};
			for (auto& list_elem : rhs)
			{
				const auto elem = lhs.get_as<impl::native_type_of<element_type>>(i++);
				if (!elem || *elem != list_elem)
					return false;
			}

			return true;
		}

	  public:

		TOML_NODISCARD
		friend bool operator==(const array& lhs, const array& rhs) noexcept
		{
			return equal(lhs, rhs);
		}

		TOML_NODISCARD
		friend bool operator!=(const array& lhs, const array& rhs) noexcept
		{
			return !equal(lhs, rhs);
		}

		template <typename T>
		TOML_NODISCARD
		friend bool operator==(const array& lhs, const std::initializer_list<T>& rhs) noexcept
		{
			return equal_to_container(lhs, rhs);
		}
		TOML_ASYMMETRICAL_EQUALITY_OPS(const array&, const std::initializer_list<T>&, template <typename T>);

		template <typename T>
		TOML_NODISCARD
		friend bool operator==(const array& lhs, const std::vector<T>& rhs) noexcept
		{
			return equal_to_container(lhs, rhs);
		}
		TOML_ASYMMETRICAL_EQUALITY_OPS(const array&, const std::vector<T>&, template <typename T>);

#if TOML_ENABLE_FORMATTERS

		friend std::ostream& operator<<(std::ostream& lhs, const array& rhs)
		{
			impl::print_to_stream(lhs, rhs);
			return lhs;
		}

#endif
	};
}
TOML_NAMESPACE_END;

#include "header_end.h"
