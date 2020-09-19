#pragma once

#include "..\..\detail\fmt\fmt\printf.h"
#include "..\..\detail\fmt\fmt\color.h"
#include "..\..\block_parser.hpp"

/* PEL from cplusplus */
namespace pel
{
	using namespace fmt;

	class code_element_render_t
	{
	public:
		code_element_render_t() = default;

		code_element_render_t(const words_base_t &w) {
			word = w;
		}

		code_element_render_t(const fmt::text_style& _ts, const words_base_t &w) {
			word = w;
			ts   = _ts;
		}

		bool is_error = false;
		fmt::text_style ts;
		words_base_t word;
	};

	class code_render_t
	{
	public:
		std::vector<code_element_render_t> elements;
		inline void push(const code_element_render_t& data) { elements.push_back(data); }
		inline void clear() { elements.clear(); }
		inline void delete_alloc() { std::clear(elements); }

		void console_print()
		{
			bool is_render_line_number = true;
			bool is_render_num = true;
			bool is_render_code = true;

			for (auto& element : elements)
			{
				if (is_render_line_number)
				{
					if (is_render_num /*&& !word.is_new_line()*/)
					{
						is_render_num = false;

						int count_space = 1;

						if (element.word.number_line < 1000)
							count_space = 1;

						if (element.word.number_line < 100)
							count_space = 2;

						if (element.word.number_line < 10)
							count_space = 3;

						print(fg(color::dark_cyan), "{}", (char)221);

						if (count_space == 3)
							print(fg(color::lime_green), " {} ", element.word.number_line);

						if (count_space == 2)
							print(fg(color::lime_green), "{} ", element.word.number_line);

						if (count_space == 1)
							print(fg(color::lime_green), "{} ", element.word.number_line);
					}

					if (element.word.is_new_line())
					{
						is_render_num = true;

					}
				}

				if (is_render_code)
					print(element.ts, "{}", element.word.data);
			}
		}

		void console_print(std::size_t number_line, bool is_error = false, std::size_t start_position = 0, std::size_t end_position = 0)
		{
			bool is_render_line_number = true;
			bool is_render_num = true;
			bool is_render_code = true;

			for (auto& element : elements)
			{
				if (element.word.number_line == number_line)
				{
					if (is_render_line_number)
					{
						if (is_render_num /*&& !word.is_new_line()*/)
						{
							is_render_num = false;

							int count_space = 1;

							if (element.word.number_line < 1000)
								count_space = 1;

							if (element.word.number_line < 100)
								count_space = 2;

							if (element.word.number_line < 10)
								count_space = 3;

							print(fg(color::dark_cyan), "{}", (char)221);

							if (count_space == 3)
								print(fg(color::lime_green), " {} ", element.word.number_line);

							if (count_space == 2)
								print(fg(color::lime_green), "{} ", element.word.number_line);

							if (count_space == 1)
								print(fg(color::lime_green), "{} ", element.word.number_line);
						}

						if (element.word.is_new_line())
						{
							is_render_num = true;
						}
					}

					if (is_render_code) {

						if (is_error)
						{	
							if (element.word.start_position >= start_position && element.word.end_position <= end_position) {

								if (element.word.data != "\n" && element.word.data != "\r" && element.word.data != "\n\r" && element.word.data != "\r\n")
									print((fg(element.ts.get_foreground()), bg(color::dark_red)), "{}", element.word.data);
							}
							else
							{
								print(element.ts, "{}", element.word.data);
							}
						}
						else
						{
							print(element.ts, "{}", element.word.data);
						}			
					}
				}

				if (element.word.number_line > number_line)
					break;
			}
		}
	};
}