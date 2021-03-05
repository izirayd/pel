#pragma once

#include "words.hpp"

namespace parser {
	class words_parser_t
	{
	public:
		void process_parse_word(const std::string &code, words_t &words)
		{
			std::flag16_t    parse_flag;
			std::flag16_t    parse_event_change_state;  // ¬ случае изменени€, получи флаг, который был изменен, что бы обработчик мог его обработать

			std::clear_flag(parse_flag);
			std::clear_flag(parse_event_change_state);

			words_base_t word;
			words_base_t word_for_one_symbol;

			uint32_t number_line     = 1;
			uint32_t number_position = 1;
			uint32_t start_position  = 1;

			std::size_t it       = 0;
			char next_symbol     = 0x00;
			char previous_symbol = 0x00;

			for (const auto& symbol : code)
			{
				if ((it + 1) < code.size())
				{
					next_symbol = code[it + 1];
				}
				else
				{
					next_symbol = 0x00;
				}

				if (it > 0)
				{
					previous_symbol = code[it - 1];
				}
				else
				{
					previous_symbol = 0x00;
				}

				it++;

				// ignore '\r'
				if (symbol == '\r')
				{
					continue;
				}

				parse_event_change_state = parse_flag;

				if (it_space_or_tab(symbol)) {
					std::clear_flag(parse_flag);
					std::add_flag(parse_flag, parse_flag_t::space_tab);
				}
				else if (it_new_line(symbol)) {

					// ≈сли нова€ лини€, то запретит стакнутьс€ с самим собой
					if (std::check_flag(parse_flag, parse_flag_t::new_line)) {

						word.number_line    = number_line - 1;
						word.start_position = start_position;
						word.end_position   = number_position;

						words.push(word);
						word.clear();

						start_position = number_position;
					}

					std::clear_flag(parse_flag);
					std::add_flag(parse_flag, parse_flag_t::new_line);

				}
				else if (it_word_symbol(symbol)) {
					std::clear_flag(parse_flag);
					std::add_flag(parse_flag, parse_flag_t::word);
				}
				else if (it_keyword_symbol(symbol)) {
					std::clear_flag(parse_flag);
					std::add_flag(parse_flag, parse_flag_t::symbol);
				}
				else
				{
					std::clear_flag(parse_flag);
					std::add_flag(parse_flag, parse_flag_t::unknow);
				}

				// ѕроизошло изменени€ буквы, теперь это что-то новое, но мы обработаем старое
				if (((parse_event_change_state != parse_flag) && ((std::flag16_t) parse_event_change_state > 0)) && !std::check_flag(parse_event_change_state, parse_flag_t::symbol)) {
					
					if (std::check_flag(parse_event_change_state, parse_flag_t::new_line)) {
						word.number_line = number_line - 1;
					}
					else
					{
						word.number_line = number_line;
					}

					word.start_position = start_position;
					word.end_position   = number_position;

					words.push(word);
					word.clear();

					start_position = number_position;
				}

				if ((std::flag16_t) parse_flag > 0) {

					if (std::check_flag(parse_flag, parse_flag_t::symbol))
					{
						word_for_one_symbol.type  = parse_flag;
						word_for_one_symbol.data += symbol;

						word_for_one_symbol.number_line    = number_line;
						word_for_one_symbol.start_position = start_position;
						word_for_one_symbol.end_position   = number_position;

						words.push(word_for_one_symbol);

						word_for_one_symbol.clear();

						start_position = number_position;

						/*	std::clear_flag(word.type);
							std::add_flag(word.type, parse_flag_t::group_symbol);
							word.data += symbol;*/
					}
					else
					{
						if (previous_symbol == '\r')
						{
							word.data += '\r';
						}

						word.type = parse_flag;
						word.data += symbol;
					}
				}

				if (it_new_line(symbol)) {

					number_line++;
					start_position = 1;
					number_position = 1;					
				}


				if (!std::check_flag(parse_flag, parse_flag_t::new_line))
				number_position++;
			}

			if (!word.data.empty())
			{
				// ≈сли нова€ лини€, то запретит стакнутьс€ с самим собой
				if (std::check_flag(parse_flag, parse_flag_t::new_line)) {

					word.number_line = number_line - 1;
					word.start_position = start_position;
					word.end_position = number_position;

					words.push(word);
					word.clear();

					start_position = number_position;
				}
				else
				{
					word.number_line = number_line;
					word.start_position = start_position;
					word.end_position = number_position;

					words.push(word);
					word.clear();

					start_position = number_position;
				}
			}
		}

	};
}