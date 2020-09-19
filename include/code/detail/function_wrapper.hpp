#pragma once

#include <cstddef>
#include <iostream>
#include <string.h>
#include <typeinfo>
#include <vector>
#include <functional>
#include <cstring>
#include <memory>
#include <unordered_map>

namespace detail {
	namespace snl
	{
		int count_allocate = 0;
		class buffer_allocator
		{
		protected:
			unsigned char* buffer = nullptr;
			std::size_t max_size = 0;

		public:
			std::size_t size() const { return max_size; }
			unsigned char* data() { return buffer; }
			unsigned char* data() const { return buffer; }

			void allocate(const std::size_t& size)
			{
				if (size > max_size && !buffer) {
					// printf("allocate: %d\n", count_allocate++);
					buffer = new unsigned char[size];
				}

				max_size = size;
			}

			void reallocate(const std::size_t& size)
			{
				if (size == max_size || size == 0)
					return;

				delallocate();
				allocate(size);
			}

			void delallocate()
			{
				if (buffer) {
					delete[] buffer;
					buffer = nullptr;
					// printf("delallocate: %d\n", --count_allocate);
				}

				max_size = 0;
			}
		};

		class var : public buffer_allocator
		{
		public:
			template<typename T>   using decay  = typename std::decay<T>::type;
			template<typename T>   using none_t = typename std::enable_if<!std::is_same<var, T>::value>::type;

			bool is_data_copy = false;
			bool is_move_copy = false;
			var(std::nullptr_t null = nullptr) { }

			var(const var& origin) {
				//printf("Copy constructur\n");
				reallocate(origin.size());
				//memcpy_s(buffer, size(), origin.data(), origin.size());
				std::memcpy(buffer, origin.data(), size());
			}

			var(var&& origin) noexcept {
				//printf("Move constructur\n");

				buffer = origin.buffer;
				max_size = origin.max_size;
				is_move_copy = true;

				//origin.buffer   = nullptr;
				//origin.max_size = 0;
			}

			template<typename base_data_t, typename = none_t<decay<base_data_t>>>
			var(base_data_t & base_data) {
				buffer = (unsigned char*) & base_data;
				max_size = sizeof(base_data_t);
				is_data_copy = true;
			}

			template<typename base_data_t, typename = none_t<decay<base_data_t>>>
			var(base_data_t && base_data) {
				//*this = base_data;
				*this = base_data;
			}

			var& operator=(var data) {
				std::swap(buffer, data.buffer);
				std::swap(max_size, data.max_size);
				return *this;
			}

			~var() {
				if (!is_move_copy && !is_data_copy)
					delallocate();
			}

			template<typename base_data_t, typename = none_t<decay<base_data_t>>>
			var & operator=(base_data_t && base_data) {

				std::size_t size_type = sizeof(base_data_t);

				if (size_type == 0)
					return *this;

				reallocate(size_type);

				std::memcpy(buffer, (const void*)& base_data, size());

				return *this;
			}

			inline operator const char* () const & { return (char*)(data()); }
			inline operator char* () const & { return (char*)(data()); }

			inline operator const unsigned char* () const & { return (unsigned char*)(data()); }
			inline operator unsigned char* () const & { return (unsigned char*)(data()); }

			template<typename data_t, typename = none_t<decay<data_t>>>
			inline operator data_t const& () const & { return *(data_t*)(data()); }

			template<typename data_t, typename = none_t<decay<data_t>>>
			inline operator data_t & () { return  *reinterpret_cast<data_t*>(data()); }

			template<typename data_t, typename = none_t<decay<data_t>>>
			inline operator data_t * () { return  reinterpret_cast<data_t*>(data()); }

			template<typename data_t>
			inline data_t     to_get()   const { return (data_t)* this; }
		};
	}

	class wrapper_base_t
	{
	public:
		virtual void call_function(
			  snl::var&& a1 = snl::var{ nullptr }
			, snl::var&& a2 = snl::var{ nullptr }
			, snl::var&& a3 = snl::var{ nullptr }
			, snl::var&& a4 = snl::var{ nullptr }
			, snl::var&& a5 = snl::var{ nullptr }
			, snl::var&& a6 = snl::var{ nullptr }
			, snl::var&& a7 = snl::var{ nullptr }
			, snl::var&& a8 = snl::var{ nullptr }
			, snl::var&& a9 = snl::var{ nullptr }
			, snl::var&& a10 = snl::var{ nullptr }
		) = 0;
	};

	template <typename... FunctionArguments>
	class function_element_t : public wrapper_base_t
	{
	public:
		function_element_t(void(*f)(FunctionArguments...)) : function(f) { count_arg = sizeof...(FunctionArguments); }

		void call_function(
			snl::var&& a1 = snl::var{ nullptr }
			, snl::var&& a2 = snl::var{ nullptr }
			, snl::var&& a3 = snl::var{ nullptr }
			, snl::var&& a4 = snl::var{ nullptr }
			, snl::var&& a5 = snl::var{ nullptr }
			, snl::var&& a6 = snl::var{ nullptr }
			, snl::var&& a7 = snl::var{ nullptr }
			, snl::var&& a8 = snl::var{ nullptr }
			, snl::var&& a9 = snl::var{ nullptr }
			, snl::var&& a10 = snl::var{ nullptr }
		) override
		{
			uint32_t count_call_arguments = 0;

			if (a1.data() != nullptr)	count_call_arguments++;
			if (a2.data() != nullptr)	count_call_arguments++;
			if (a3.data() != nullptr)	count_call_arguments++;
			if (a4.data() != nullptr)	count_call_arguments++;
			if (a5.data() != nullptr)	count_call_arguments++;
			if (a6.data() != nullptr)	count_call_arguments++;
			if (a7.data() != nullptr)	count_call_arguments++;
			if (a8.data() != nullptr)	count_call_arguments++;
			if (a9.data() != nullptr)	count_call_arguments++;
			if (a10.data() != nullptr)	count_call_arguments++;

			// protect count arguments
			if (count_arg == count_call_arguments)
				base_call(
					std::make_tuple(
						std::move(a1),
						std::move(a2),
						std::move(a3),
						std::move(a4),
						std::move(a5),
						std::move(a6),
						std::move(a7),
						std::move(a8),
						std::move(a9),
						std::move(a10)
					),
					std::make_index_sequence<sizeof...(FunctionArguments)>{});
		}

		template <typename Tuple, std::size_t... Indices>
		void base_call(Tuple&& t, const std::index_sequence<Indices...>& s)
		{
			function(std::get<Indices>(t)...);
		}

		uint32_t count_arg = 0;
		std::function<void(FunctionArguments...)> function;
	};

	template <typename... FunctionArguments>
	inline std::shared_ptr<wrapper_base_t> make_function(void(func)(FunctionArguments...))
	{
		return std::make_shared<function_element_t<FunctionArguments...>>(func);
	}

	// because stl not declares return type
	namespace datail {
		template <class type_t, class = void> struct weak_result_t {};

#pragma warning(push)
#pragma warning(disable : 4996) // was declared deprecated
		template <class type_t> struct weak_result_t<type_t, std::void_t<typename type_t::result_type>> {
			typedef typename type_t::result_type result_type;
		};
#pragma warning(pop)

		template <class return_t>
		struct forsed_result_t { typedef return_t result_type; };

		template <class type_t>
		struct is_function { using bool_t = std::false_type; };
		template <class type_t>
		struct is_memfunptr { using bool_t = std::false_type; };

		template <class type_t,
			class = void>
			struct weak_argument_t : weak_result_t<type_t> {
		};

#pragma warning(push)
#pragma warning(disable : 4996) 
		template <class type_t>
		struct weak_argument_t<type_t, std::void_t<typename type_t::argument_type>> : weak_result_t<type_t>
		{
			typedef typename type_t::argument_type argument_type;
		};
#pragma warning(pop)

		template <class type_t,
			class = void>
			struct weak_binary_args : weak_argument_t<type_t> {
		};

#pragma warning(push)
#pragma warning(disable : 4996) 
		template <class type_t>
		struct weak_binary_args<type_t, std::void_t<typename type_t::first_argument_type,
			typename type_t::second_argument_type>>
			: weak_argument_t<type_t> {
			typedef typename type_t::first_argument_type  first_argument_type;
			typedef typename type_t::second_argument_type second_argument_type;
		};
#pragma warning(pop)

		template <class type_t>
		struct weak_types {
			using is_f_or_pf  = is_function<std::remove_pointer_t<type_t>>;
			using is_pmf      = is_memfunptr<std::remove_cv_t<type_t>>;
			using type        = std::conditional_t<is_f_or_pf::bool_t::value, is_f_or_pf,
				std::conditional_t<is_pmf::bool_t::value, is_pmf, weak_binary_args<type_t>>>;
		};

		struct base_binder_t {};
		struct zero_variadic_args_t {};
		struct one_variadic_args_t {};

		template <class type1_t, class type2_t,
			bool = std::is_empty_v<type1_t> && !std::is_final_v<type1_t>>
			class compressed_pair_t final : private type1_t {
			private:
				type2_t value;
				using base_type = type1_t;

			public:
				template <class... other2_t>
				constexpr explicit compressed_pair_t(zero_variadic_args_t, other2_t&& ... value2)
					: type1_t(), value(std::forward<other2_t>(value2)...) {
				}

				template <class other1_t, class... other2_t>
				compressed_pair_t(one_variadic_args_t, other1_t&& value1, other2_t&& ... value2)
					: type1_t(std::forward<other1_t>(value1)),
					value(std::forward<other2_t>(value2)...) {
				}

				type1_t& get_first()               noexcept { return *this; }
				const type1_t& get_first()  const  noexcept { return *this; }
				type2_t& get_second()              noexcept { return value; }
				const type2_t& get_second() const  noexcept { return value; }
		};

		template <class type1_t, class type2_t>
		class compressed_pair_t<type1_t, type2_t, false> final {
		private:
			type1_t value1;
			type2_t value2;

		public:
			template <class... other2_t>
			constexpr explicit compressed_pair_t(zero_variadic_args_t, other2_t&& ... argument_value2)
				: value1(),
				value2(std::forward<other2_t>(argument_value2)...) {}

			template <class other1_t, class... other2_t>
			compressed_pair_t(one_variadic_args_t, other1_t&& argument_value1, other2_t&& ... argument_value2)
				: value1(std::forward<other1_t>(argument_value1)),
				value2(std::forward<other2_t>(argument_value2)...) {}

			type1_t& get_first()        noexcept { return value1; }
			const type1_t& get_first()  const noexcept { return value1; }
			type2_t& get_second()       noexcept { return value2; }
			const type2_t& get_second() const noexcept { return value2; }
		};

		template <class return_t, class function_t>
		struct wrapper_result_t {
			using fuinction_decay_t = std::decay_t<function_t>;
			using all_types_t       = typename weak_types<fuinction_decay_t>::type;
			using type              = std::conditional_t<std::is_same_v<return_t, base_binder_t>, weak_result_t<all_types_t>, forsed_result_t<return_t>>;
		};

		template <class type_t,
			bool = std::is_void_v<type_t>>
			struct invoker_return_t { };

		template <class cv_void_t>
		struct invoker_return_t<cv_void_t, true>
		{
			template <class... value_t>
			static void invoker_call(value_t&& ... values) { std::invoke(std::forward<value_t>(values)...); }
		};

		template <class type_t>
		struct invoker_return_t<type_t, false> {
			template <class... value_t>
			static type_t invoker_call(value_t&& ... values) {
				return std::invoke(std::forward<value_t>(values)...);
			}
		};

		template <>
		struct invoker_return_t<base_binder_t, false> {
			template <class... value_t>
			static auto invoker_call(value_t&& ... values)
				-> decltype(std::invoke(std::forward<value_t>(values)...)) {
				return std::invoke(std::forward<value_t>(values)...);
			}
		};

		template <class type_t, template <class...> class template_t>
		constexpr bool is_specialization_v = false;

		template <template <class...> class template_t, class... type_t>
		constexpr bool is_specialization_v<template_t<type_t...>, template_t> = true;

		template <class cv_tid_t, bool = is_specialization_v<std::remove_cv_t<cv_tid_t>, std::reference_wrapper>,
			bool = std::is_bind_expression_v<cv_tid_t>, int = std::is_placeholder_v<cv_tid_t>>
			struct select_fixer;

		template <class cv_tid_t>
		struct select_fixer<cv_tid_t, true, false, 0> {
			template <class untuple_t>
			static auto fix(cv_tid_t& cv_tid, untuple_t&&) -> typename cv_tid_t::type & {
				return cv_tid.get();
			}
		};

		template <class cv_tid_t>
		struct select_fixer<cv_tid_t, false, true, 0> {
#pragma warning(push)
#pragma warning(disable : 4100) 
			template <class untuple_t, size_t... jx_t>
			static auto apply(cv_tid_t& cv_tid, untuple_t&& untuple,
				std::index_sequence<jx_t...>) -> decltype(cv_tid(std::get<jx_t>(std::move(untuple))...)) {
				return cv_tid(std::get<jx_t>(std::move(untuple))...);
			}
#pragma warning(pop)

			template <class untuple_t>
			static auto fix(cv_tid_t& cv_tid, untuple_t&& untuple) -> decltype(apply(cv_tid, std::move(untuple),
				std::make_index_sequence<std::tuple_size_v<untuple_t>>())) {
				return apply(cv_tid, std::move(untuple), std::make_index_sequence<std::tuple_size_v<untuple_t>>());
			}
		};

		template <class cv_tid_t>
		struct select_fixer<cv_tid_t, false, false, 0> {
			template <class untuple_t>
			static cv_tid_t& fix(cv_tid_t& cv_tid, untuple_t&&) {
				return cv_tid;
			}
		};

		template <class cv_tid_t, int jx_t>
		struct select_fixer<cv_tid_t, false, false, jx_t> {
			static_assert(jx_t > 0, "invalid is_placeholder value");

			template <class untuple_t>
			static auto fix(cv_tid_t&, untuple_t&& untuple)
				-> decltype(std::get<jx_t - 1>(std::move(untuple)))
			{
				return std::get<jx_t - 1>(std::move(untuple));
			}
		};

		template <class cv_tid_t, class untuple_t>
		inline auto fix_arguments(cv_tid_t& cv_tid, untuple_t&& untuple)
			-> decltype(select_fixer<cv_tid_t>::fix(cv_tid, std::move(untuple))) {
			return select_fixer<cv_tid_t>::fix(cv_tid, std::move(untuple));
		}
	};

	template <class return_t, std::size_t... index_t, class cv_field_t, class cv_tuple_t, class untuple_t>
	auto call_wrapper_binder(
		datail::invoker_return_t<return_t>,
		std::index_sequence<index_t...>,
		cv_field_t& object,
		cv_tuple_t& cv_tuple,
		untuple_t&& untuple
	) -> decltype(
		datail::invoker_return_t<return_t>::invoker_call(
			object,
			datail::fix_arguments(std::get<index_t>(cv_tuple), std::move(untuple))...
		)
		)
	{
		return 	   datail::invoker_return_t<return_t>::invoker_call(
			object,
			datail::fix_arguments(std::get<index_t>(cv_tuple), std::move(untuple))...
		);
	}

	template <class return_t, class function_t, class... types_t>
	class wrapper_binder_t : public datail::wrapper_result_t<return_t, function_t>::type, public wrapper_base_t
	{
		using sequence_t = std::index_sequence_for<types_t...>;
		using first_t    = std::decay_t<function_t>;
		using second_t   = std::tuple<std::decay_t<types_t>...>;

		datail::compressed_pair_t<first_t, second_t> pair_element;

	public:
		explicit wrapper_binder_t(function_t&& function, types_t&& ... args)
			: pair_element(datail::one_variadic_args_t(),
				std::forward<function_t>(function),
				std::forward<types_t>(args)...) {
		}

		template <class... unbound_t>
		auto operator()(unbound_t&& ... unbound)
			->decltype(
				call_wrapper_binder(datail::invoker_return_t<return_t>(), sequence_t(), pair_element.get_first(), pair_element.get_second(), std::forward_as_tuple(std::forward<unbound_t>(unbound)...))
				)
		{
			return call_wrapper_binder(datail::invoker_return_t<return_t>(), sequence_t(), pair_element.get_first(), pair_element.get_second(), std::forward_as_tuple(std::forward<unbound_t>(unbound)...));
		}

		template <class... unbound_t>
		auto operator()(unbound_t&& ... unbound)
			const->decltype(
				call_wrapper_binder(datail::invoker_return_t<return_t>(), sequence_t(), pair_element.get_first(), pair_element.get_second(), std::forward_as_tuple(std::forward<unbound_t>(unbound)...))
				)
		{
			return call_wrapper_binder(datail::invoker_return_t<return_t>(), sequence_t(), pair_element.get_first(), pair_element.get_second(), std::forward_as_tuple(std::forward<unbound_t>(unbound)...));
		}

		void call_function(
			  snl::var&& a1  = snl::var{ nullptr }
			, snl::var&& a2  = snl::var{ nullptr }
			, snl::var&& a3  = snl::var{ nullptr }
			, snl::var&& a4  = snl::var{ nullptr }
			, snl::var&& a5  = snl::var{ nullptr }
			, snl::var&& a6  = snl::var{ nullptr }
			, snl::var&& a7  = snl::var{ nullptr }
			, snl::var&& a8  = snl::var{ nullptr }
			, snl::var&& a9  = snl::var{ nullptr }
			, snl::var&& a10 = snl::var{ nullptr }
		) override
		{
			 return base_call(
				std::move(a1),
				std::move(a2),
				std::move(a3),
				std::move(a4),
				std::move(a5),
				std::move(a6),
				std::move(a7),
				std::move(a8),
				std::move(a9),
				std::move(a10)
			);
		}

		template <class... unbound_t>
		auto base_call(unbound_t&& ... unbound)
			->decltype(
				call_wrapper_binder(datail::invoker_return_t<return_t>(), sequence_t(), pair_element.get_first(), pair_element.get_second(), std::forward_as_tuple(std::forward<unbound_t>(unbound)...))
				)
		{
			return call_wrapper_binder(datail::invoker_return_t<return_t>(), sequence_t(), pair_element.get_first(), pair_element.get_second(), std::forward_as_tuple(std::forward<unbound_t>(unbound)...));
		}
	};

	template <class function_t, class... types_t>
	inline wrapper_binder_t<datail::base_binder_t, function_t, types_t...>
		func_bind(
			function_t&& function, types_t&& ... args
		)
	{
		return wrapper_binder_t<datail::base_binder_t, function_t, types_t...>(std::forward<function_t>(function), std::forward<types_t>(args)...);
	}

	template <class function_t, class... types_t>
	inline std::shared_ptr<wrapper_base_t> bind_function(function_t&& function, types_t&& ... args)
	{
		return std::make_shared<wrapper_binder_t<datail::base_binder_t, function_t, types_t...>>(std::forward<function_t>(function), std::forward<types_t>(args)...);
	}

	class function_wrapper_t
	{
	public:
		template<typename function_t>
		void add_function(const std::string& name,
			const function_t& function
		)
		{
			function_list.erase(name);
			function_list.insert({ name, make_function(&function) });
		}

		template <class function_t, class... types_t>
		void add_function(const std::string& name, function_t&& function, types_t&& ... args)
		{
			function_list.erase(name);
			function_list.insert({ name, bind_function(std::forward<function_t>(function), std::forward<types_t>(args)...) });
		}

		template <typename... arguments>
		void call(const std::string& name, arguments&& ... args)
		{
			auto function = function_list.find(name);

			if (function == function_list.end()) {}
			else
				function->second->call_function(static_cast<arguments&&>(args)...);
		}

		function_wrapper_t& operator[] (const std::string& name_function)
		{
			name_tmp = name_function;
			return *this;
		}

		template <typename... arguments>
		function_wrapper_t& operator() (arguments&& ... args)
		{
			call(name_tmp, static_cast<arguments&&>(args)...);
			return *this;
		}

		template<typename function_t>
		function_wrapper_t& operator= (const function_t& function)
		{
			add_function(name_tmp, function);
			return *this;
		}

		function_wrapper_t& operator= (const std::shared_ptr<wrapper_base_t>& result_bind_function)
		{
			function_list.erase(name_tmp);
			function_list.insert({ name_tmp, result_bind_function });
			return *this;
		}

		void delete_function(const std::string& name_function)
		{
			function_list.erase(name_function);
		}

		std::string name_tmp;
		std::unordered_map<std::string, std::shared_ptr<wrapper_base_t>> function_list;
	};
}