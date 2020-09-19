#pragma once

#include "words_parser.hpp"

namespace parser {

	class block_parser_t
	{
	public:
		bool block_parse(
			tree_words_t*  mother_tree,
			std::size_t    position_start,
			std::size_t    position_end,
			std::intptr_t& change_position,
			const std::vector<std::pair<std::string, std::string>>& array_symbols,
			words_t* base_words
		)
		{
			if (!mother_tree || !base_words)
				return false;

			std::size_t max_size_words = position_end;// lol base_words->words.size();

			bool is_last_word = false;
			words_base_t* next_word = nullptr;
			bool is_comment_line = false;
			bool is_global_comments = false;
			bool is_ignore_next = false;

			for (std::size_t i = position_start; i < position_end; i++)
			{
				if (max_size_words == (i + 1))
					is_last_word = true;

				if (!is_last_word)
					next_word = &base_words->words[i + 1];
				else next_word = nullptr;

				if (max_size_words <= i)
					return true;

				if (base_words->words[i].data == "/" && next_word && !is_comment_line && !is_global_comments)
				{
					if (next_word->data == "/")
					{
						is_comment_line = true;
					}
				}

				if (base_words->words[i].data == "/" && next_word && !is_comment_line && !is_global_comments)
				{
					if (next_word->data == "*")
					{
						is_global_comments = true;
					}
				}

				if (!is_comment_line && !is_global_comments)
					if (!is_ignore_next)
						if (base_words->words[i].is_symbol() && !base_words->words[i].is_group_symbol())
						{

							for (std::size_t k = 0; k < array_symbols.size(); k++)
							{
								if (base_words->words[i].data == array_symbols[k].first)
								{
									tree_words_t* new_tree = new tree_words_t;

									mother_tree->sync(new_tree);

									std::intptr_t new_position = -1;

									base_words->words[i].start_index = i;
									base_words->words[i].index_pair = k;

									new_tree->set_value(&base_words->words[i]);

									if (block_parse(new_tree, i + 1, position_end, new_position, array_symbols, base_words))
									{
										if (new_position > -1) {
											i = new_position;
											k = 0;
										}

									}
									else break;//return false;
								}

								if (max_size_words <= i)
									return true;

								if (mother_tree->parent && mother_tree->get_value())
								{
									if ((mother_tree->get_value()->data == "<" && base_words->words[i].data == ";"))
									{
										delete mother_tree;

										change_position = i + 1;
										return false;
									}

									if ((base_words->words[i].data == array_symbols[k].second))
									{
										if (array_symbols[mother_tree->get_value()->index_pair].second == base_words->words[i].data) {

											tree_words_t* new_tree = new tree_words_t;

											mother_tree->sync(new_tree);

											base_words->words[i].start_index = mother_tree->get_value()->start_index;
											base_words->words[i].end_index = i;

											new_tree->set_value(&base_words->words[i]);

											mother_tree->get_value()->end_index = i;

											new_tree->get_value()->my_pair = mother_tree;
											mother_tree->get_value()->my_pair = new_tree;

											mother_tree->parent->push(mother_tree);
											mother_tree->parent->push(new_tree);


											change_position = i + 1;
											return true;

										}

									}
								}

							}
						}

				if (is_ignore_next)
				{
					is_ignore_next = false;
				}

				if (base_words->words[i].is_new_line() && is_comment_line) {
					is_comment_line = false;
					//is_ignore_next  = true;
				}

				if (base_words->words[i].data == "*" && next_word && is_global_comments)
				{
					if (next_word->data == "/")
					{
						is_global_comments = false;
						is_ignore_next = true;
					}
				}

				mother_tree->push(&base_words->words[i]);
			}

			return true;

		}
	};
}