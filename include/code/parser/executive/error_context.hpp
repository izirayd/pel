#pragma once

#include "code_render.hpp"

namespace pel {

	class error_element_t
	{
	public:
		std::string text;
		std::string desc; // descriptor

		std::size_t number_line = 0;
		std::size_t start_position = 0;
		std::size_t end_position = 0;

		error_element_t() = default;

		error_element_t(const std::string& error_text)
		{
			text = error_text;
		}

		error_element_t(const std::string& error_text, const std::string& error_desc)
		{
			text = error_text;
			desc = error_desc;
		}

		error_element_t(const std::string& error_text,
			const std::string& error_desc,
			const uint32_t& _number_line,
			const uint32_t& _start_position,
			const uint32_t& _end_position
		)
		{
			text = error_text;
			desc = error_desc;

			number_line = _number_line;
			start_position = _start_position;
			end_position = _end_position;
		}
	};

	class error_context_t
	{
	public:

		std::vector<error_element_t> elements;

		bool is_error()
		{
			return !elements.empty();
		}

		void clear()
		{
			elements.clear();
		}

		void push(const std::string& error_text)
		{
			error_element_t element;

			element.text = error_text;

			elements.push_back(element);
		}

		void push(const std::string& error_text, const std::string& error_desc)
		{
			error_element_t element;

			element.text = error_text;
			element.desc = error_desc;

			elements.push_back(element);
		}

		void push(const std::string& error_text,
			const std::string& error_desc,
			const std::size_t& number_line,
			const std::size_t& start_position,
			const std::size_t& end_position
		)
		{
			error_element_t element;

			element.text = error_text;
			element.desc = error_desc;

			element.number_line = number_line;
			element.start_position = start_position;
			element.end_position = end_position;

			elements.push_back(element);
		}

		void push(const error_element_t& element)
		{
			elements.push_back(element);
		}

		void print_console(pel::code_render_t* cd = nullptr)
		{
			if (!elements.empty())
				print(fmt::fg(fmt::color::red), "{0} Errors list\n", (char)254);

			for (const auto& element : elements)
			{
				print(fmt::fg(fmt::color::red), "{}", (char)221);
				print(fmt::fg(fmt::color::indian_red), "[line {0}:{1}]:", element.number_line, element.start_position);
				print(fmt::fg(fmt::color::indian_red), " {}\n", element.text);

				if (cd)
				{
					cd->console_print(element.number_line, true, element.start_position, element.end_position);
				}
			}

			print("\n");
		}
	};
}