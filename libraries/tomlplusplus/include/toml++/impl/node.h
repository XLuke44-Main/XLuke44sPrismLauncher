//# This file is a part of toml++ and is subject to the the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT
#pragma once

#include "std_utility.h"
#include "forward_declarations.h"
#include "source_region.h"
#include "header_start.h"

TOML_NAMESPACE_START
{

	class TOML_ABSTRACT_INTERFACE TOML_EXPORTED_CLASS node
	{
	  private:

		friend class TOML_PARSER_TYPENAME;
		source_region source_{};

		template <typename T>
		TOML_NODISCARD
		decltype(auto) get_value_exact() const noexcept(impl::value_retrieval_is_nothrow<T>);

		template <typename T, typename N>
		using ref_type_ = std::conditional_t<													 
			std::is_reference_v<T>,																 
			impl::copy_ref<impl::copy_cv<impl::unwrap_node<T>, std::remove_reference_t<N>>, T>,  
			impl::copy_cvref<impl::unwrap_node<T>, N>											 
			>;

		template <typename T, typename N>
		using ref_type = std::conditional_t<			  
			std::is_reference_v<N>,						  
			ref_type_<T, N>,							  
			ref_type_<T, std::add_lvalue_reference_t<N>>  
			>;

		template <typename T, typename N>
		TOML_PURE_GETTER
		static ref_type<T, N&&> do_ref(N&& n) noexcept
		{
			using unwrapped_type = impl::unwrap_node<T>;
			static_assert(toml::is_value<unwrapped_type> || toml::is_container<unwrapped_type>,
						  "The template type argument of node::ref() must be one of:" TOML_SA_UNWRAPPED_NODE_TYPE_LIST);

			TOML_ASSERT_ASSUME(
				n.template is<unwrapped_type>()
				&& "template type argument provided to toml::node::ref() didn't match the node's actual type");

			using node_ref = std::remove_volatile_t<std::remove_reference_t<N>>&;
			using val_type = std::remove_volatile_t<unwrapped_type>;
			using out_ref  = ref_type<T, N&&>;
			static_assert(std::is_reference_v<out_ref>);

			if constexpr (toml::is_value<unwrapped_type>)
				return static_cast<out_ref>(const_cast<node_ref>(n).template ref_cast<val_type>().get());
			else
				return static_cast<out_ref>(const_cast<node_ref>(n).template ref_cast<val_type>());
		}

	  protected:
		TOML_EXPORTED_MEMBER_FUNCTION
		node() noexcept;

		TOML_EXPORTED_MEMBER_FUNCTION
		node(const node&) noexcept;

		TOML_EXPORTED_MEMBER_FUNCTION
		node(node&&) noexcept;

		TOML_EXPORTED_MEMBER_FUNCTION
		node& operator=(const node&) noexcept;

		TOML_EXPORTED_MEMBER_FUNCTION
		node& operator=(node&&) noexcept;

		template <typename T, typename N>
		using ref_cast_type_ = std::conditional_t<											   
			std::is_reference_v<T>,															   
			impl::copy_ref<impl::copy_cv<impl::wrap_node<T>, std::remove_reference_t<N>>, T>,  
			impl::copy_cvref<impl::wrap_node<T>, N>											   
			>;

		template <typename T, typename N>
		using ref_cast_type = std::conditional_t<			   
			std::is_reference_v<N>,							   
			ref_cast_type_<T, N>,							   
			ref_cast_type_<T, std::add_lvalue_reference_t<N>>  
			>;

		template <typename T>
		TOML_PURE_INLINE_GETTER
		ref_cast_type<T, node&> ref_cast() & noexcept
		{
			using out_ref  = ref_cast_type<T, node&>;
			using out_type = std::remove_reference_t<out_ref>;
			return static_cast<out_ref>(*reinterpret_cast<out_type*>(this));
		}

		template <typename T>
		TOML_PURE_INLINE_GETTER
		ref_cast_type<T, node&&> ref_cast() && noexcept
		{
			using out_ref  = ref_cast_type<T, node&&>;
			using out_type = std::remove_reference_t<out_ref>;
			return static_cast<out_ref>(*reinterpret_cast<out_type*>(this));
		}

		template <typename T>
		TOML_PURE_INLINE_GETTER
		ref_cast_type<T, const node&> ref_cast() const& noexcept
		{
			using out_ref  = ref_cast_type<T, const node&>;
			using out_type = std::remove_reference_t<out_ref>;
			return static_cast<out_ref>(*reinterpret_cast<out_type*>(this));
		}

		template <typename T>
		TOML_PURE_INLINE_GETTER
		ref_cast_type<T, const node&&> ref_cast() const&& noexcept
		{
			using out_ref  = ref_cast_type<T, const node&&>;
			using out_type = std::remove_reference_t<out_ref>;
			return static_cast<out_ref>(*reinterpret_cast<out_type*>(this));
		}

	  public:
		TOML_EXPORTED_MEMBER_FUNCTION
		virtual ~node() noexcept;

		TOML_PURE_GETTER
		virtual bool is_homogeneous(node_type ntype, node*& first_nonmatch) noexcept = 0;

		TOML_PURE_GETTER
		virtual bool is_homogeneous(node_type ntype, const node*& first_nonmatch) const noexcept = 0;

		TOML_PURE_GETTER
		virtual bool is_homogeneous(node_type ntype) const noexcept = 0;

		template <typename ElemType = void>
		TOML_PURE_GETTER
		bool is_homogeneous() const noexcept
		{
			using type = impl::remove_cvref<impl::unwrap_node<ElemType>>;
			static_assert(std::is_void_v<type> || toml::is_value<type> || toml::is_container<type>,
						  "The template type argument of node::is_homogeneous() must be void or one "
						  "of:" TOML_SA_UNWRAPPED_NODE_TYPE_LIST);

			return is_homogeneous(impl::node_type_of<type>);
		}

		TOML_PURE_GETTER
		virtual node_type type() const noexcept = 0;

		TOML_PURE_GETTER
		virtual bool is_table() const noexcept = 0;

		TOML_PURE_GETTER
		virtual bool is_array() const noexcept = 0;

		TOML_PURE_GETTER
		virtual bool is_array_of_tables() const noexcept = 0;

		TOML_PURE_GETTER
		virtual bool is_value() const noexcept = 0;

		TOML_PURE_GETTER
		virtual bool is_string() const noexcept = 0;

		TOML_PURE_GETTER
		virtual bool is_integer() const noexcept = 0;

		TOML_PURE_GETTER
		virtual bool is_floating_point() const noexcept = 0;

		TOML_PURE_GETTER
		virtual bool is_number() const noexcept = 0;

		TOML_PURE_GETTER
		virtual bool is_boolean() const noexcept = 0;

		TOML_PURE_GETTER
		virtual bool is_date() const noexcept = 0;

		TOML_PURE_GETTER
		virtual bool is_time() const noexcept = 0;

		TOML_PURE_GETTER
		virtual bool is_date_time() const noexcept = 0;

		template <typename T>
		TOML_PURE_INLINE_GETTER
		bool is() const noexcept
		{
			using type = impl::remove_cvref<impl::unwrap_node<T>>;
			static_assert(toml::is_value<type> || toml::is_container<type>,
						  "The template type argument of node::is() must be one of:" TOML_SA_UNWRAPPED_NODE_TYPE_LIST);

			if constexpr (std::is_same_v<type, table>)
				return is_table();
			else if constexpr (std::is_same_v<type, array>)
				return is_array();
			else if constexpr (std::is_same_v<type, std::string>)
				return is_string();
			else if constexpr (std::is_same_v<type, int64_t>)
				return is_integer();
			else if constexpr (std::is_same_v<type, double>)
				return is_floating_point();
			else if constexpr (std::is_same_v<type, bool>)
				return is_boolean();
			else if constexpr (std::is_same_v<type, date>)
				return is_date();
			else if constexpr (std::is_same_v<type, time>)
				return is_time();
			else if constexpr (std::is_same_v<type, date_time>)
				return is_date_time();
		}

		TOML_PURE_GETTER
		virtual table* as_table() noexcept = 0;

		TOML_PURE_GETTER
		virtual array* as_array() noexcept = 0;

		TOML_PURE_GETTER
		virtual toml::value<std::string>* as_string() noexcept = 0;

		TOML_PURE_GETTER
		virtual toml::value<int64_t>* as_integer() noexcept = 0;

		TOML_PURE_GETTER
		virtual toml::value<double>* as_floating_point() noexcept = 0;

		TOML_PURE_GETTER
		virtual toml::value<bool>* as_boolean() noexcept = 0;

		TOML_PURE_GETTER
		virtual toml::value<date>* as_date() noexcept = 0;

		TOML_PURE_GETTER
		virtual toml::value<time>* as_time() noexcept = 0;

		TOML_PURE_GETTER
		virtual toml::value<date_time>* as_date_time() noexcept = 0;

		TOML_PURE_GETTER
		virtual const table* as_table() const noexcept = 0;

		TOML_PURE_GETTER
		virtual const array* as_array() const noexcept = 0;

		TOML_PURE_GETTER
		virtual const toml::value<std::string>* as_string() const noexcept = 0;

		TOML_PURE_GETTER
		virtual const toml::value<int64_t>* as_integer() const noexcept = 0;

		TOML_PURE_GETTER
		virtual const toml::value<double>* as_floating_point() const noexcept = 0;

		TOML_PURE_GETTER
		virtual const toml::value<bool>* as_boolean() const noexcept = 0;

		TOML_PURE_GETTER
		virtual const toml::value<date>* as_date() const noexcept = 0;

		TOML_PURE_GETTER
		virtual const toml::value<time>* as_time() const noexcept = 0;

		TOML_PURE_GETTER
		virtual const toml::value<date_time>* as_date_time() const noexcept = 0;

		template <typename T>
		TOML_PURE_INLINE_GETTER
		impl::wrap_node<T>* as() noexcept
		{
			using unwrapped_type = impl::unwrap_node<impl::remove_cvref<T>>;
			static_assert(toml::is_value<unwrapped_type> || toml::is_container<unwrapped_type>,
						  "The template type argument of node::as() must be one of:" TOML_SA_UNWRAPPED_NODE_TYPE_LIST);

			if constexpr (std::is_same_v<unwrapped_type, table>)
				return as_table();
			else if constexpr (std::is_same_v<unwrapped_type, array>)
				return as_array();
			else if constexpr (std::is_same_v<unwrapped_type, std::string>)
				return as_string();
			else if constexpr (std::is_same_v<unwrapped_type, int64_t>)
				return as_integer();
			else if constexpr (std::is_same_v<unwrapped_type, double>)
				return as_floating_point();
			else if constexpr (std::is_same_v<unwrapped_type, bool>)
				return as_boolean();
			else if constexpr (std::is_same_v<unwrapped_type, date>)
				return as_date();
			else if constexpr (std::is_same_v<unwrapped_type, time>)
				return as_time();
			else if constexpr (std::is_same_v<unwrapped_type, date_time>)
				return as_date_time();
		}

		template <typename T>
		TOML_PURE_INLINE_GETTER
		const impl::wrap_node<T>* as() const noexcept
		{
			using unwrapped_type = impl::unwrap_node<impl::remove_cvref<T>>;
			static_assert(toml::is_value<unwrapped_type> || toml::is_container<unwrapped_type>,
						  "The template type argument of node::as() must be one of:" TOML_SA_UNWRAPPED_NODE_TYPE_LIST);

			if constexpr (std::is_same_v<unwrapped_type, table>)
				return as_table();
			else if constexpr (std::is_same_v<unwrapped_type, array>)
				return as_array();
			else if constexpr (std::is_same_v<unwrapped_type, std::string>)
				return as_string();
			else if constexpr (std::is_same_v<unwrapped_type, int64_t>)
				return as_integer();
			else if constexpr (std::is_same_v<unwrapped_type, double>)
				return as_floating_point();
			else if constexpr (std::is_same_v<unwrapped_type, bool>)
				return as_boolean();
			else if constexpr (std::is_same_v<unwrapped_type, date>)
				return as_date();
			else if constexpr (std::is_same_v<unwrapped_type, time>)
				return as_time();
			else if constexpr (std::is_same_v<unwrapped_type, date_time>)
				return as_date_time();
		}

		template <typename T>
		TOML_NODISCARD
		optional<T> value_exact() const noexcept(impl::value_retrieval_is_nothrow<T>);

		template <typename T>
		TOML_NODISCARD
		optional<T> value() const noexcept(impl::value_retrieval_is_nothrow<T>);

		template <typename T>
		TOML_NODISCARD
		auto value_or(T&& default_value) const noexcept(impl::value_retrieval_is_nothrow<T>);

		template <typename T>
		TOML_PURE_GETTER
		decltype(auto) ref() & noexcept
		{
			return do_ref<T>(*this);
		}

		template <typename T>
		TOML_PURE_GETTER
		decltype(auto) ref() && noexcept
		{
			return do_ref<T>(std::move(*this));
		}

		template <typename T>
		TOML_PURE_GETTER
		decltype(auto) ref() const& noexcept
		{
			return do_ref<T>(*this);
		}

		template <typename T>
		TOML_PURE_GETTER
		decltype(auto) ref() const&& noexcept
		{
			return do_ref<T>(std::move(*this));
		}

		TOML_PURE_INLINE_GETTER
		const source_region& source() const noexcept
		{
			return source_;
		}

	  private:

		template <typename Func, typename Node, typename T>
		static constexpr bool can_visit = std::is_invocable_v<Func, ref_cast_type<T, Node>>;

		template <typename Func, typename Node, typename T>
		static constexpr bool can_visit_nothrow = std::is_nothrow_invocable_v<Func, ref_cast_type<T, Node>>;

		template <typename Func, typename Node>
		static constexpr bool can_visit_any = can_visit<Func, Node, table>		  
										   || can_visit<Func, Node, array>		  
										   || can_visit<Func, Node, std::string>  
										   || can_visit<Func, Node, int64_t>	  
										   || can_visit<Func, Node, double>		  
										   || can_visit<Func, Node, bool>		  
										   || can_visit<Func, Node, date>		  
										   || can_visit<Func, Node, time>		  
										   || can_visit<Func, Node, date_time>;

		 

		template <typename Func, typename Node>
		static constexpr bool can_visit_all = can_visit<Func, Node, table>		  
										   && can_visit<Func, Node, array>		  
										   && can_visit<Func, Node, std::string>  
										   && can_visit<Func, Node, int64_t>	  
										   && can_visit<Func, Node, double>		  
										   && can_visit<Func, Node, bool>		  
										   && can_visit<Func, Node, date>		  
										   && can_visit<Func, Node, time>		  
										   && can_visit<Func, Node, date_time>;

		template <typename Func, typename Node, typename T>
		static constexpr bool visit_is_nothrow_one = !can_visit<Func, Node, T> || can_visit_nothrow<Func, Node, T>;

		template <typename Func, typename Node>
		static constexpr bool visit_is_nothrow = visit_is_nothrow_one<Func, Node, table>	    
											  && visit_is_nothrow_one<Func, Node, array>	    
											  && visit_is_nothrow_one<Func, Node, std::string>  
											  && visit_is_nothrow_one<Func, Node, int64_t>	    
											  && visit_is_nothrow_one<Func, Node, double>	    
											  && visit_is_nothrow_one<Func, Node, bool>		    
											  && visit_is_nothrow_one<Func, Node, date>		    
											  && visit_is_nothrow_one<Func, Node, time>		    
											  && visit_is_nothrow_one<Func, Node, date_time>;

		 

		template <typename Func, typename Node, typename T, bool = can_visit<Func, Node, T>>
		struct visit_return_type_
		{
			using type = decltype(std::declval<Func>()(std::declval<ref_cast_type<T, Node>>()));
		};
		template <typename Func, typename Node, typename T>
		struct visit_return_type_<Func, Node, T, false>
		{
			using type = void;
		};

		template <typename Func, typename Node, typename T>
		using visit_return_type = typename visit_return_type_<Func, Node, T>::type;

		template <typename A, typename B>
		using nonvoid = std::conditional_t<std::is_void_v<A>, B, A>;

		template <typename Func, typename Node>
		static decltype(auto) do_visit(Func&& visitor, Node&& n) noexcept(visit_is_nothrow<Func&&, Node&&>)
		{
			static_assert(can_visit_any<Func&&, Node&&>,
						  "TOML node visitors must be invocable for at least one of the toml::node "
						  "specializations:" TOML_SA_NODE_TYPE_LIST);

			switch (n.type())
			{
				case node_type::table:
					if constexpr (can_visit<Func&&, Node&&, table>)
						return static_cast<Func&&>(visitor)(static_cast<Node&&>(n).template ref_cast<table>());
					break;

				case node_type::array:
					if constexpr (can_visit<Func&&, Node&&, array>)
						return static_cast<Func&&>(visitor)(static_cast<Node&&>(n).template ref_cast<array>());
					break;

				case node_type::string:
					if constexpr (can_visit<Func&&, Node&&, std::string>)
						return static_cast<Func&&>(visitor)(static_cast<Node&&>(n).template ref_cast<std::string>());
					break;

				case node_type::integer:
					if constexpr (can_visit<Func&&, Node&&, int64_t>)
						return static_cast<Func&&>(visitor)(static_cast<Node&&>(n).template ref_cast<int64_t>());
					break;

				case node_type::floating_point:
					if constexpr (can_visit<Func&&, Node&&, double>)
						return static_cast<Func&&>(visitor)(static_cast<Node&&>(n).template ref_cast<double>());
					break;

				case node_type::boolean:
					if constexpr (can_visit<Func&&, Node&&, bool>)
						return static_cast<Func&&>(visitor)(static_cast<Node&&>(n).template ref_cast<bool>());
					break;

				case node_type::date:
					if constexpr (can_visit<Func&&, Node&&, date>)
						return static_cast<Func&&>(visitor)(static_cast<Node&&>(n).template ref_cast<date>());
					break;

				case node_type::time:
					if constexpr (can_visit<Func&&, Node&&, time>)
						return static_cast<Func&&>(visitor)(static_cast<Node&&>(n).template ref_cast<time>());
					break;

				case node_type::date_time:
					if constexpr (can_visit<Func&&, Node&&, date_time>)
						return static_cast<Func&&>(visitor)(static_cast<Node&&>(n).template ref_cast<date_time>());
					break;

				case node_type::none: TOML_UNREACHABLE;
				default: TOML_UNREACHABLE;
			}

			if constexpr (!can_visit_all<Func&&, Node&&>)
			{
				 

				using return_type =
					nonvoid<visit_return_type<Func&&, Node&&, table>,
					nonvoid<visit_return_type<Func&&, Node&&, array>,
					nonvoid<visit_return_type<Func&&, Node&&, std::string>,
					nonvoid<visit_return_type<Func&&, Node&&, int64_t>,
					nonvoid<visit_return_type<Func&&, Node&&, double>,
					nonvoid<visit_return_type<Func&&, Node&&, bool>,
					nonvoid<visit_return_type<Func&&, Node&&, date>,
					nonvoid<visit_return_type<Func&&, Node&&, time>,
							visit_return_type<Func&&, Node&&, date_time>
				>>>>>>>>;

				 

				if constexpr (!std::is_void_v<return_type>)
				{
					static_assert(std::is_default_constructible_v<return_type>,
								  "Non-exhaustive visitors must return a default-constructible type, or void");
					return return_type{};
				}
			}
		}

	  public:

		template <typename Func>
		decltype(auto) visit(Func&& visitor) & noexcept(visit_is_nothrow<Func&&, node&>)
		{
			return do_visit(static_cast<Func&&>(visitor), *this);
		}

		template <typename Func>
		decltype(auto) visit(Func&& visitor) && noexcept(visit_is_nothrow<Func&&, node&&>)
		{
			return do_visit(static_cast<Func&&>(visitor), static_cast<node&&>(*this));
		}

		template <typename Func>
		decltype(auto) visit(Func&& visitor) const& noexcept(visit_is_nothrow<Func&&, const node&>)
		{
			return do_visit(static_cast<Func&&>(visitor), *this);
		}

		template <typename Func>
		decltype(auto) visit(Func&& visitor) const&& noexcept(visit_is_nothrow<Func&&, const node&&>)
		{
			return do_visit(static_cast<Func&&>(visitor), static_cast<const node&&>(*this));
		}

		TOML_NODISCARD
		explicit operator node_view<node>() noexcept;

		TOML_NODISCARD
		explicit operator node_view<const node>() const noexcept;

		TOML_NODISCARD
		TOML_EXPORTED_MEMBER_FUNCTION
		node_view<node> at_path(std::string_view path) noexcept;

		TOML_NODISCARD
		TOML_EXPORTED_MEMBER_FUNCTION
		node_view<const node> at_path(std::string_view path) const noexcept;

		TOML_NODISCARD
		TOML_EXPORTED_MEMBER_FUNCTION
		node_view<node> at_path(const toml::path& path) noexcept;

		TOML_NODISCARD
		TOML_EXPORTED_MEMBER_FUNCTION
		node_view<const node> at_path(const toml::path& path) const noexcept;

#if TOML_ENABLE_WINDOWS_COMPAT

		TOML_NODISCARD
		TOML_EXPORTED_MEMBER_FUNCTION
		node_view<node> at_path(std::wstring_view path);

		TOML_NODISCARD
		TOML_EXPORTED_MEMBER_FUNCTION
		node_view<const node> at_path(std::wstring_view path) const;

#endif  

		TOML_NODISCARD
		TOML_EXPORTED_MEMBER_FUNCTION
		node_view<node> operator[](const toml::path& path) noexcept;

		TOML_NODISCARD
		TOML_EXPORTED_MEMBER_FUNCTION
		node_view<const node> operator[](const toml::path& path) const noexcept;

	};
}
TOML_NAMESPACE_END;

TOML_IMPL_NAMESPACE_START
{
	TOML_PURE_GETTER
	TOML_EXPORTED_FREE_FUNCTION
	bool TOML_CALLCONV node_deep_equality(const node*, const node*) noexcept;
}
TOML_IMPL_NAMESPACE_END;

#include "header_end.h"
