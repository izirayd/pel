#pragma once

#include <stdint.h>

#include <cstddef>
#include <iostream>
#include <vector>
#include <cstring>
#include <algorithm>

namespace pel {

	namespace groups {

		class group_element_t
		{
		public:
			group_element_t() = default;
			group_element_t(const uint8_t /*unsigned char*/& uchar_element) { element = uchar_element; }
			// Размер данного элемента отражает 
			uint8_t element = 0;
		};

		using group_elements_t = std::vector<group_element_t>;

		class group_t
		{

		private: 
			void init() {
				all_elements.resize(256 /* uint8_t */, 0);
			}

			std::vector<uint32_t> all_elements;
			group_elements_t elements;
		public:

			group_t() { init();  };
			group_t(const std::string& name_group, const group_elements_t& ge) { init();  name = name_group; elements = ge; }
			group_t(const std::string& name_group, const std::string& string_element) { init();  name = name_group; push_element(string_element); }

			std::string name;
			std::size_t last_position = 0;

			bool is_glue   = false;
			bool is_split  = false;
			bool is_ignore = false;

			// O (1)
			inline bool is_belongs(const group_element_t& element) {
				return all_elements[(uint32_t) element.element] > 0;
			}

			void sort_elements() {}

			// O (1)
			void push_element(const group_element_t& element) {
				elements.push_back(element);
				all_elements[(uint32_t)element.element]++;
			}


			void push_element(const std::string& str) {

				for (size_t i = 0; i < str.length(); i++)
					push_element(str[i]);
				
			}

			// O (n)
			void clear() { 

				elements.clear();
			
				for (size_t i = 0; i < all_elements.size(); i++)
				{
					all_elements[i] = 0;
				}
			}

			std::size_t size()     const { return elements.size(); }
			group_elements_t get() const { return elements; }
		};

		using groups_list_t = std::vector<group_t>;

		class group_result_t
		{
		public:
			std::vector<group_t*> groups;

			inline void clear() {
				groups.clear();
			}

			inline void push_group(group_t* group) {
				groups.push_back(group);
			}

		};

		class groups_t
		{
		public:
			groups_list_t groups;

			void push_group(const group_t& group) {
				groups.push_back(group);
			}

			group_result_t* get(const group_element_t& element) {

				result.clear();

				for (size_t i = 0; i < groups.size(); i++)
				{
					if (groups[i].is_belongs(element))
					{
						result.push_group(&groups[i]);
					}
				}

				return &result;
			};

		private:
			group_result_t result;
		};

		struct object_t
		{
			std::string data;

			std::size_t number_line    = 0;
			std::size_t start_position = 0;
			std::size_t end_position   = 0;

			inline void clear() {
				data.clear();
			}

			group_t *group;
		};

		using vector_objects_t = std::vector<object_t>;

		class position_element_t
		{
			public:
				std::size_t position = 0;
				vector_objects_t words;
		};

		class array_words_t
		{
			public:
				std::vector<position_element_t> data;

				inline void clear() {
					
					for (size_t i = 0; i < data.size(); i++)
					{
						for (auto& it : data[i].words)
						{
							it.group->last_position = 0;
						}
					}

					data.clear();
				}

				inline void push_group(const position_element_t &group) {
					data.push_back(group);	
				}
		};
	}
}