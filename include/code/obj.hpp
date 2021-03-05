#pragma once

#include "words_parser.hpp"
#include "block_parser.hpp"

#include "obj_flags.hpp"

namespace pel
{
	class obj_t
	{
	  
	  public:

		std::string name;

		obj_flag_base_t flag = 0;
		
		inline bool is_flag(const obj_flag_t& c_flag) {
			return std::check_flag(flag, c_flag);
		}

		void add_inversion_flag(obj_t& obj, const obj_flag_t& c_flag) {
		
			if (!obj.is_flag(c_flag))
			{
				add_flag(c_flag);
			}

		}

		void del_flag(const obj_flag_t& plus_flag) {
			std::del_flag(flag, plus_flag);
		}

		void add_flag(const obj_flag_t& plus_flag)
		{
			std::add_flag(flag, plus_flag);
		}

		bool add_flag_from_string_to_chain(const std::string& str) {

			for (auto& it : chain_flags)
			{
				for (auto& str_it : it.second)
				{
					if (str_it == str)
					{
						add_flag(it.first);
						return true;
					}
				}

			}

			return false;
		}

		bool add_flag_from_string_to_property(const std::string& str) {

			for (auto& it : property_flags)
			{
				for (auto& str_it : it.second)
				{
					if (str_it == str)
					{
						add_flag(it.first);
						return true;
					}
				}
				
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
}