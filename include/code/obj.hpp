#pragma once

#include "words_parser.hpp"
#include "block_parser.hpp"

namespace pel
{
	class obj_t
	{
	public:
		std::string name;
	
		// TODO: switch in flags?
		bool is_execute = false;
		bool is_type  = false;
		bool is_value = false;
		bool is_group = false;

		bool is_and = false;
		bool is_or  = false;
		bool is_xor = false;
		bool is_not = false;

		bool is_space    = false;
		bool is_tab      = false;
		bool is_new_line = false;

		bool is_true  = false; // lul
		bool is_false = false;
		
		// Group property
		bool is_glue   = false;
		bool is_split  = false;
		bool is_ignore = false;
		bool is_autogen_block = false;

		bool is_maybe = false;

		bool is_return = false;
		bool is_exit = false;

		bool is_recursion = false;

		bool is_breakpoint = false;

		words_base_t word;

		std::vector<obj_t> values;

		void clear()
		{
			// ??
			for (size_t i = 0; i < values.size(); i++)
			{
				values[i].clear();
			}

			name.clear();
			values.clear();

			is_group = false;
			is_execute = false;
			is_type  = false;
			is_value = false;

			is_and = false;
			is_or  = false;
			is_xor = false;
			is_not = false;

			is_space    = false;
			is_tab      = false;
			is_new_line = false;

			is_glue  = false;
			is_split = false;

			is_true  = false; // 
			is_false = false;

			is_ignore = false;
			is_autogen_block = false;

			is_maybe  = false;
			is_return = false;
			is_exit   = false;

			is_recursion = false;

			is_breakpoint = false;
		}
	};
}