#pragma once

#include "words.hpp"
#include "block_parser.hpp"

#include "obj_flags.hpp"

#include "../code/detail/tree.hpp"

namespace pel
{
	class obj_t
	{
	  
	  public:

		std::string name;

		obj_flag_base_t flag = 0;
		
		inline bool is_flag(const obj_flag_t& c_flag) const {
			return std::check_flag(flag, c_flag);
		}

		inline void add_inversion_flag(obj_t& obj, const obj_flag_t& c_flag) {
		
			if (!obj.is_flag(c_flag))
				add_flag(c_flag);
		}

		inline void del_flag(const obj_flag_t& plus_flag) {
			std::del_flag(flag, plus_flag);
		}

		inline void add_flag(const obj_flag_t& plus_flag)	{
			std::add_flag(flag, plus_flag);
		}

		bool add_flag_from_string_to_chain(const std::string& str) {

		    const auto result = chain_flags.find(str);

			if (result != chain_flags.end())
			{				
				add_flag(result->second);
				return true;
			}

			return false;
		}

		// O (1)
		bool add_flag_from_string_to_property(const std::string& str) {

			const auto result = property_flags.find(str);

			if (result != property_flags.end())
			{				
				add_flag(result->second);
				return true;
			}

			return false;
		}

		bool is_space           = false;
		bool is_tab             = false;
		bool is_new_line        = false;
		bool is_condition_true  = false;
		bool is_condition_false = false;

		words_base_t word;

		std::vector<obj_t> values;

		void clear()
		{
			std::clear_flag(flag);

			// ??
			for (size_t i = 0; i < values.size(); i++)
			{
				values[i].clear();
			}

			name.clear();
			values.clear();

			is_space    = false;
			is_tab      = false;
			is_new_line = false;
			is_condition_true = false;
			is_condition_false = false;
		}
	};

	struct obj_base_t
	{
		words_base_t  words_base;

		inline const std::string& c_str() { return words_base.data; };

		bool is_read_string_end = false;
		bool is_read_property   = false;
		bool is_multi_property  = false;
		bool is_conditional_expressions = false;

		obj_t  obj;
		obj_t* conditional_expressions = nullptr;

		tree_t<obj_base_t*>* my_pair = nullptr;
	};

	using tree_obj_base_t = tree_t<obj_base_t*>;

	class spec_obj_base_t
	{
	public:
		std::vector<obj_base_t> words;

		inline void push(const obj_base_t& data) { words.push_back(data); }
		inline void push(const words_base_t& words_base) { 

			obj_base_t obj_base;

			obj_base.words_base = words_base;
			
			words.push_back(obj_base);
		}

		inline void clear() { words.clear(); }
		inline void delete_alloc() { std::clear(words); }
	};

}