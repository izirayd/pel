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
		  public:
			
			group_t() = default;
			group_t(const std::string& name_group, const group_elements_t& ge)        { name = name_group; elements = ge; }
			group_t(const std::string& name_group, const std::string& string_element) { name = name_group; push_element(string_element); }

			std::string name;
			std::size_t last_position = 0;

			bool is_glue    = false;
			bool is_split   = false;
			bool is_ignored = false;

			bool is_belongs(const group_element_t& element) {

				if (is_sequential)
					return element.element >= min_sequential && element.element <= max_sequential;

				return std::binary_search(elements.begin(), elements.end(), element, [](const group_element_t& left_element, const group_element_t& right_element) { return (left_element.element < right_element.element); });
			}

			void sort_elements() {

				std::sort(elements.begin(), elements.end(), [](const group_element_t& left_element, const group_element_t& right_element) { return (left_element.element < right_element.element); });
				sequential();
			}

			bool    is_sequential = false;

			uint8_t min_sequential = UINT8_MAX;
			uint8_t max_sequential = 0;

			// функция проверяет лежат ли числа последовательно или нет
			void sequential() {

				if (elements.empty())
				{
					is_sequential = false; // or true??
					return;
				}

				bool is_first = false;
				uint8_t last = 0;

				for (auto& element : elements)
				{
					if (!is_first)
					{
						is_first = true;
						last = element.element;

						if (last < min_sequential)
							min_sequential = last;

						if (last > max_sequential)
							max_sequential = last;

						continue;
					}

					if ((last + 1) != element.element)
					{
						is_sequential = false;
						return;
					}

					last = element.element;;

					if (last < min_sequential)
						min_sequential = last;

					if (last > max_sequential)
						max_sequential = last;
				}

				is_sequential = true;
			}

			void push_element(const group_element_t& element) {
				elements.push_back(element);
			}

			void push_element(const std::string& str) {

				for (size_t i = 0; i < str.length(); i++)
				{
					push_element(str[i]);
				}
			}

			void clear() { elements.clear(); }

			std::size_t size()     const { return elements.size(); }
			group_elements_t get() const { return elements; }

		private:
			group_elements_t elements;

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
					data.clear();
				}

				inline void push_group(const position_element_t &group) {
					data.push_back(group);
				}
		};
	}
}