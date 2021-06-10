#pragma once
/* Prototype */

#include "words_parser.hpp"
#include "block_parser.hpp"

namespace pel
{
	class ast_element_t
	{
		public:
			std::string element;
			std::string type_name;
			std::string print;
	};

	using ast_base_t = tree_t<ast_element_t>;

	namespace emulate_recursion {

		void get_print() {}
	}

	class ast_t 
	{
		public:
			ast_base_t ast_base;

			ast_base_t *get_print(const std::string& text)     {
			
				emulate_recursion::get_print();	

			}

			ast_base_t *get_type_name(const std::string& text) { }
	};
}