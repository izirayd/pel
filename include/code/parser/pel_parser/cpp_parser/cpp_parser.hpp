#include "..\..\..\block_parser.hpp"
#include "..\..\..\obj.hpp"
#include "..\..\executive\executive_parser.hpp"

namespace pel {

	struct core_data_manager_t
	{
		std::vector<obj_t*>		    *all_types  = nullptr;
		std::vector<obj_t*>		    *all_tests  = nullptr;
		std::vector<obj_t*>		    *all_groups = nullptr;

		parser::executive::parser_core_t* parser_core = nullptr;
	};

	namespace cpp_parser {

		struct obj_base_t
		{
			words_base_t  words_base;

			inline const std::string& c_str() { return words_base.data; };

			bool is_read_string_end = false;
			bool is_read_property   = false;
			bool is_multi_property  = false;
			bool is_conditional_expressions = false;

			obj_t		  obj;

			obj_t* conditional_expressions = nullptr;

			tree_t<obj_base_t*>* my_pair = nullptr;
		};

		using tree_obj_base_t = tree_t<obj_base_t*>;

		class spec_obj_base_t
		{
		public:
			std::vector<obj_base_t> words;
			inline void push(const obj_base_t& data) { words.push_back(data); }
			inline void clear() { words.clear(); }
			inline void delete_alloc() { std::clear(words); }
		};

		struct pel_keywords_element_t
		{
			std::string name;

			bool is_have_body = false;
			bool is_have_property = false;
			bool is_have_name = false;
			bool is_have_string_value = false;
			bool is_have_instuction = false;
			bool is_end_symbol = false;
			bool is_newline_symbol = false;

			bool is_read_object = false;

			bool is_was_read_body = false;
			bool is_was_read_property = false;
			bool is_was_read_name = false;
			bool is_was_read_string_value = false;
			bool is_was_read_instuction = false;
			bool is_was_read_symbol = false;
			bool is_was_read_newline_symbol = false;

			obj_t obj;
			obj_base_t tree_obj_base;

			void reset()
			{
				is_was_read_body = false;
				is_was_read_property = false;
				is_was_read_name = false;
				is_was_read_string_value = false;
				is_was_read_instuction = false;
				is_was_read_symbol = false;
				is_was_read_newline_symbol = false;

				if (is_read_object)
					obj.clear();

				tree_obj_base.obj.clear();
			}
		};

		using pel_keywords_t = std::vector<pel_keywords_element_t>;

		pel_keywords_t pel_keywords;	
		pel_keywords_element_t* current_keyword = nullptr;

		bool is_have_keyword = false;
		bool is_read_string  = false;

		void get_next_with_ignore_space_tab_newline(tree_obj_base_t* tree_words, tree_obj_base_t*& next_tree_words) {

			if (!tree_words) {
				next_tree_words = nullptr;
				return;
			}

			auto word = tree_words->get_value();

			if (word)
			{
				if (word->words_base.is_new_line() || word->words_base.is_space_tab())
				{
					get_next_with_ignore_space_tab_newline(tree_words->next, next_tree_words);
				}
				else
				{
					next_tree_words = tree_words;
				}
			}
		}

		void get_left_propery_block(tree_obj_base_t* tree_words, tree_obj_base_t*& result, bool& is_next_read, const std::string& read_symbol) {

			if (tree_words == nullptr) {

				fmt::print(fmt::fg(fmt::color::red), "Error, no left value for property\n");
				return;
			}

			auto word = tree_words->get_value();

			if (word)
			{
				if (word->words_base.is_new_line() || word->words_base.is_space_tab())
				{
					return get_left_propery_block(tree_words->previous, result, is_next_read, read_symbol);
				}
			}
			if (is_next_read)
			{
				if (!word)
				{
					result = nullptr;
					return;
				}

				result = tree_words;

				if (result)
				{
					if (result->get_value()->words_base.data == "}")
					{
						result = result->get_value()->my_pair;
					}

					if (result->get_value()->words_base.data == "\"" || result->get_value()->words_base.data == "'")
					{
						result = result->get_value()->my_pair;
					}
				}

				return;
			}

			if (word)
			{
				if (word->words_base.data == read_symbol)
				{
					is_next_read = true;
				}
			}
			else
			{
				result = nullptr;
				return;
			}

			return get_left_propery_block(tree_words->previous, result, is_next_read, read_symbol);
		}

		bool block_parse_obj(
			tree_obj_base_t* mother_tree,
			std::size_t       position_start,
			std::size_t       position_end,
			std::intptr_t& change_position,
			const std::vector<std::pair<std::string, std::string>>& array_symbols,
			spec_obj_base_t* base_words
		)
		{
			if (!mother_tree || !base_words)
				return false;

			std::size_t max_size_words = position_end;// lol base_words->words.size();

			bool is_last_word = false;
			obj_base_t* next_word = nullptr;
			bool is_comment_line = false;
			bool is_global_comments = false;
			bool is_ignore_next = false;

			bool is_ignore_push = false;
			bool is_ignore_next_push = false;

			bool is_no_recursion = false;
			std::string no_recursion_str = "";
			tree_obj_base_t* range_tree = nullptr;

			bool is_skip = false;

			for (std::size_t i = position_start; i < position_end; i++)
			{
				if (is_ignore_next_push)
				{
					is_ignore_next_push = false;
					is_ignore_push = true;
				}

				is_skip = false;

				if (max_size_words == (i + 1))
					is_last_word = true;

				if (!is_last_word)
					next_word = &base_words->words[i + 1];
				else next_word = nullptr;

				if (max_size_words <= i)
					return true;

				if (base_words->words[i].words_base.data == "/" && next_word && !is_comment_line && !is_global_comments)
				{
					if (next_word->words_base.data == "/")
					{
						is_comment_line = true;
					}
					else if (next_word->words_base.data == "*")
					{
						is_global_comments = true;
					}
				}

				/* stop range */
				if (is_no_recursion && no_recursion_str == base_words->words[i].words_base.data)
				{
					is_no_recursion = false;
					no_recursion_str.clear();

					mother_tree->push(&base_words->words[i]);

					std::size_t mother_tree_size = mother_tree->size();

					if (mother_tree_size > 1)
					{
						mother_tree->tree[mother_tree_size - 2]->get_value()->my_pair = mother_tree->tree[mother_tree_size - 1];
						mother_tree->tree[mother_tree_size - 1]->get_value()->my_pair = mother_tree->tree[mother_tree_size - 2];
					}

					range_tree = nullptr;

					continue;
				}

				if (is_no_recursion)
				{
					range_tree->push(&base_words->words[i]);
				}
				else
				{
					if (!is_comment_line && !is_global_comments)
						if (!is_ignore_next)
							if (base_words->words[i].words_base.is_symbol() && !base_words->words[i].words_base.is_group_symbol())
							{
								for (std::size_t k = 0; k < array_symbols.size(); k++)
								{
									if (base_words->words[i].words_base.data == array_symbols[k].first)
									{
										if (array_symbols[k].first == array_symbols[k].second)
										{
											is_no_recursion = true;
											no_recursion_str = array_symbols[k].first;

											range_tree = mother_tree->push(&base_words->words[i]);

											is_skip = true; /* skip */
											break;
										}

										if (is_no_recursion)
										{

										}
										else {

											tree_obj_base_t* new_tree = new tree_obj_base_t;

											mother_tree->sync(new_tree);

											std::intptr_t new_position = -1;

											base_words->words[i].words_base.start_index = i;
											base_words->words[i].words_base.index_pair = k;

											new_tree->set_value(&base_words->words[i]);

											if (block_parse_obj(new_tree, i + 1, position_end, new_position, array_symbols, base_words))
											{
												if (new_position > -1) {
													i = new_position;
													k = 0;
												}

											}
											else break;//return false;
										}
									}

									if (max_size_words <= i)
										return true;

									if (mother_tree->parent && mother_tree->get_value())
									{
										if ((mother_tree->get_value()->words_base.data == "<" && base_words->words[i].words_base.data == ";"))
										{
											delete mother_tree;

											change_position = i + 1;
											return false;
										}

										if ((base_words->words[i].words_base.data == array_symbols[k].second))
										{
											if (array_symbols[mother_tree->get_value()->words_base.index_pair].second == base_words->words[i].words_base.data) {

												tree_obj_base_t* new_tree = new tree_obj_base_t;

												mother_tree->sync(new_tree);

												base_words->words[i].words_base.start_index = mother_tree->get_value()->words_base.start_index;
												base_words->words[i].words_base.end_index = i;

												new_tree->set_value(&base_words->words[i]);

												mother_tree->get_value()->words_base.end_index = i;

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

					if (is_skip)
						continue;

					if (is_ignore_next)
					{
						is_ignore_next = false;
					}

					if (base_words->words[i].words_base.is_new_line() && is_comment_line) {
						is_comment_line = false;
					}

					if (base_words->words[i].words_base.data == "*" && next_word && is_global_comments)
					{
						if (next_word->words_base.data == "/")
						{
							is_global_comments = false;
							is_ignore_next = true;
							is_ignore_push = true;
							is_ignore_next_push = true;
						}
					}

					if (!is_comment_line && !is_global_comments) {

						if (is_ignore_push)
						{
							is_ignore_push = false;

						}
						else {

							if (mother_tree->last_element())
							{
								auto element = *mother_tree->last_element();
								if (element->words_base.data == "-" && base_words->words[i].words_base.data == ">")
								{
									element->words_base.data = "->";
									element->words_base.end_position++;
									continue;
								}
							}

							mother_tree->push(&base_words->words[i]);
						}

					}
				}
			}

			return true;
		}

		class cpp_parser_pel_t
		{
		    public:
				bool process_parse(words_t *words, core_data_manager_t* core_data_manager) {

					tree_obj_base_t* tree_words = new tree_obj_base_t;
					spec_obj_base_t spec_words;

					for (size_t i = 0; i < words->words.size(); i++)
					{
						obj_base_t base;
						base.words_base = words->words[i];
						spec_words.push(base);
					}

					std::intptr_t new_position = -1;
					std::vector<std::pair<std::string, std::string>> array_symbols;

					array_symbols.push_back({ "{", "}" });
					array_symbols.push_back({ "\"", "\"" });

					block_parse_obj(tree_words, 0, spec_words.words.size(), new_position, array_symbols, &spec_words);

					pel_keywords_element_t type_keyword;

					type_keyword.name = "type";

					type_keyword.is_end_symbol = true;
					type_keyword.is_have_body = true;
					type_keyword.is_have_instuction = true;
					type_keyword.is_have_property = true;
					type_keyword.is_have_name = true;
					type_keyword.is_have_string_value = true;
					type_keyword.is_newline_symbol = false;
					type_keyword.is_read_object = true;

					pel_keywords.push_back(type_keyword);

					type_keyword.name = "group";

					type_keyword.is_end_symbol = true;
					type_keyword.is_have_body = true;
					type_keyword.is_have_instuction = true;
					type_keyword.is_have_property = true;
					type_keyword.is_have_name = true;
					type_keyword.is_have_string_value = true;
					type_keyword.is_newline_symbol = false;
					type_keyword.is_read_object = true;

					pel_keywords.push_back(type_keyword);

					type_keyword.name = "test";

					type_keyword.is_end_symbol = true;
					type_keyword.is_have_body = true;
					type_keyword.is_have_instuction = true;
					type_keyword.is_have_property = true;
					type_keyword.is_have_name = true;
					type_keyword.is_have_string_value = true;
					type_keyword.is_newline_symbol = false;
					type_keyword.is_read_object = true;

					pel_keywords.push_back(type_keyword);

					type_keyword.name = "#";

					type_keyword.is_end_symbol = true;
					type_keyword.is_have_body = true;
					type_keyword.is_have_instuction = true;
					type_keyword.is_have_property = true;
					type_keyword.is_have_name = true;
					type_keyword.is_have_string_value = true;
					type_keyword.is_newline_symbol = false;
					type_keyword.is_read_object = true;

					pel_keywords.push_back(type_keyword);

					tree_words->process_function["base"] = detail::bind_function(&cpp_parser_pel_t::process_parse_tree, this, std::placeholders::_1, std::placeholders::_2);
					tree_words->process_function["last_parent"] = detail::bind_function(&cpp_parser_pel_t::last_process_parse_tree, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);

					last_position = 0;
					tree_words->start_process(core_data_manager);

					pel_keywords.clear();
					spec_words.clear();

					tree_words->delete_tree();
					delete tree_words;
					tree_words = nullptr;

					return true;
				}

		    private:
				std::size_t counter_autoblock = 0;
				std::size_t last_position = 0;

				struct conditional_element_t {

					struct base_element_t {

						base_element_t() = default;

						base_element_t(const obj_t& new_value) {
							set(new_value);
						}

						base_element_t(conditional_element_t* new_value) {
							set(new_value);
						}

						obj_t value;
						conditional_element_t* full_value = nullptr;

						bool is_used = false;

						void set(const obj_t& new_value) {
							value = new_value;
							is_used = true;
						}

						void set(conditional_element_t* new_value) {
							full_value = new_value;
							is_used = true;
						}
					};

					struct element_t
					{
						base_element_t condition;
						base_element_t first;
						base_element_t second;
					};

					element_t data;

					class memory_manager_t {
					public:
						void push(conditional_element_t* element) {
							condition_element.push_back(element);
						}

						void delete_alloc() {

							for (auto& it : condition_element)
								if (it) delete it;

							condition_element.clear();
						}
					private:
						std::vector<conditional_element_t*> condition_element;
					};

					class erase_manager_t {
					public:
						void push(const std::size_t& position) {
							if (!std::binary_search(data.begin(), data.end(), position)) {
								data.push_back(position);
							}
						}

						void clear() {
							data.clear();
						}

						std::vector<std::size_t> data;
					};

					static void read_condition(
						pel::obj_t* obj,
						std::size_t start_position,
						conditional_element_t* parent_condition,
						bool is_last_first,
						bool is_last_second,
						std::size_t& move_iterator,
						bool& is_skip_parent,
						conditional_element_t::memory_manager_t& memory_manager,
						erase_manager_t& erase_list) {

						if (!parent_condition)
							return;

						std::size_t tmp_i = start_position;
						bool is_skip = false;

						conditional_element_t* current_condition = parent_condition;

						for (size_t i = start_position; i < obj->values.size(); i++)
						{
							if (obj->values[i].is_condition_false)
							{
								erase_list.push(i);

								current_condition->data.second.set(obj->values[i]);

								if (obj->values.size() > i + 1) {

									if (obj->values[i + 1].is_condition_true) {

										current_condition->data.second.full_value = new conditional_element_t;
										memory_manager.push(current_condition->data.second.full_value);

										read_condition(obj, i + 1, current_condition->data.second.full_value, false, true, tmp_i, is_skip, memory_manager, erase_list);

										i = tmp_i;

										if (tmp_i >= obj->values.size())
											break;
									}
								}

								is_skip_parent = true;
							}

							if (obj->values[i].is_condition_true)
							{
								if (is_last_first)
								{
									parent_condition->data.first.full_value = new conditional_element_t;

									memory_manager.push(parent_condition->data.first.full_value);

									current_condition = parent_condition->data.first.full_value;
								}

								if (!is_last_first) {
									current_condition->data.condition.set(obj->values[i - 1]);
									erase_list.push(i - 1);
								}

								current_condition->data.first.set(obj->values[i]);
								erase_list.push(i);

								read_condition(obj, i + 1, current_condition, true, false, tmp_i, is_skip, memory_manager, erase_list);

								i = tmp_i;

								if (tmp_i >= obj->values.size())
									break;

								if (is_skip)
									break;
							}
							else
							{
								move_iterator = i;
								tmp_i = i;
								return;
							}
						}

						move_iterator = tmp_i;
					}
				};

				void condition_autogen_vertex(obj_t& obj) {

					obj.add_flag(obj_flag_t::obj_type);
					obj.add_flag(obj_flag_t::obj_and);
					obj.add_flag(obj_flag_t::obj_auto);

					obj.name = fmt::format("cond{}", counter_autoblock++);
				}

				void print_condition(conditional_element_t* condition)
				{
					conditional_element_t* current_condition = condition;

					if (current_condition->data.condition.is_used)
						fmt::print("{}", current_condition->data.condition.value.name);

					if (current_condition->data.first.is_used)
						fmt::print(" -> {}", current_condition->data.first.value.name);

					if (current_condition->data.second.is_used)
						fmt::print(" : {}", current_condition->data.second.value.name);

					fmt::print("\n");

					if (current_condition->data.first.is_used && current_condition->data.first.full_value)
					{
						print_condition(current_condition->data.first.full_value);
					}

					if (current_condition->data.second.is_used && current_condition->data.second.full_value)
					{
						print_condition(current_condition->data.second.full_value);
					}
				}

				void morph_condition(conditional_element_t* condition, obj_t& obj, bool is_last_first, bool is_last_second, obj_t* left_parent, bool& is_write_read_obj_parent)
				{
					if (!condition)
						return;

					conditional_element_t* current_condition = condition;

					bool is_and = false;
					bool is_or = false;
					bool is_empty_operation = false;

					if (current_condition->data.first.is_used && current_condition->data.second.is_used)
					{
						is_or = true;
						is_and = false;
						is_empty_operation = false;
					}
					else
					{
						if (current_condition->data.first.is_used) {
							is_and = true;
							is_or = false;
							is_empty_operation = false;
						}
						else {
							is_and = false;
							is_or = false;
							is_empty_operation = true;
						}
					}

					if (is_or)
					{
						condition_autogen_vertex(obj);

						obj.add_flag(obj_flag_t::obj_or);
						obj.del_flag(obj_flag_t::obj_and);

						obj_t obj_left, obj_right;

						condition_autogen_vertex(obj_left);
						condition_autogen_vertex(obj_right);

						if (is_last_first || is_last_second)
						{
							if (left_parent) {
								if (current_condition->data.condition.is_used) {
									fmt::print("Error, can`t have here condition lol!\n");
								}

								obj_left.values.push_back(*left_parent);

							}
							else
								if (current_condition->data.condition.is_used) {
									obj_left.values.push_back(current_condition->data.condition.value);
								}
						}
						else
						{
							if (current_condition->data.condition.is_used) {
								obj_left.values.push_back(current_condition->data.condition.value);
							}
						}

						// { {a, b} or {!a, c} }
						// { {a, {b, c} } or {!a, d} }
						// { a, {b, { c, { d }} } }
						if (current_condition->data.first.full_value)
						{
							obj_t read_obj;
							bool  is_write_read_obj = false;
							morph_condition(current_condition->data.first.full_value, read_obj, false, true, &current_condition->data.first.value, is_write_read_obj);
							obj_left.values.push_back(read_obj);
						}
						else
						{
							obj_left.values.push_back(current_condition->data.first.value);
						}

						obj.values.push_back(obj_left);

						if (is_last_first || is_last_second)
						{
							if (left_parent) {

								if (current_condition->data.condition.is_used) {
									fmt::print("Error, can`t have here condition lol!\n");
								}

								// inversion
								left_parent->add_inversion_flag(*left_parent, obj_flag_t::obj_not);

								obj_right.values.push_back(*left_parent);

							}
							else
								if (current_condition->data.condition.is_used) {
									current_condition->data.condition.value.add_inversion_flag(current_condition->data.condition.value, obj_flag_t::obj_not);
									obj_right.values.push_back(current_condition->data.condition.value);
								}
						}
						else
						{
							if (current_condition->data.condition.is_used) {
								current_condition->data.condition.value.add_inversion_flag(current_condition->data.condition.value, obj_flag_t::obj_not);
								obj_right.values.push_back(current_condition->data.condition.value);
							}
						}

						if (current_condition->data.second.full_value)
						{
							obj_t read_obj;
							bool  is_write_read_obj = false;
							morph_condition(current_condition->data.second.full_value, read_obj, false, true, &obj, is_write_read_obj);
							obj_right.values.push_back(read_obj);
						}
						else
						{
							obj_right.values.push_back(current_condition->data.second.value);
						}

						obj.values.push_back(obj_right);

						if ((is_last_first || is_last_second) && left_parent)
						{
							*left_parent = obj;
						}
						else
						{
							is_write_read_obj_parent = true;
						}
					}

					if (is_and)
					{
						condition_autogen_vertex(obj);

						obj.add_flag(obj_flag_t::obj_and);
						obj.del_flag(obj_flag_t::obj_or);

						if (current_condition->data.first.full_value)
						{
							obj_t read_obj;
							bool  is_write_read_obj = false;

							morph_condition(current_condition->data.first.full_value, read_obj, true, false, &current_condition->data.first.value, is_write_read_obj);

							if (current_condition->data.condition.is_used) {
								obj.values.push_back(current_condition->data.condition.value);
								obj.values.push_back(current_condition->data.first.value);

								if (is_write_read_obj)
									obj.values.push_back(read_obj);
							}
							else
							{
								obj.values.push_back(current_condition->data.first.value);

								if (is_write_read_obj)
									obj.values.push_back(read_obj);

								is_write_read_obj_parent = true;
							}
						}
						else
						{
							if (current_condition->data.condition.is_used) {
								obj.values.push_back(current_condition->data.condition.value);
								obj.values.push_back(current_condition->data.first.value);
							}
							else
							{
								obj = current_condition->data.first.value;
								is_write_read_obj_parent = true;
							}
						}
					}
				}

				void last_process_parse_tree(tree_obj_base_t* tree_words, tree_obj_base_t* first_child_graph, tree_obj_base_t* last_child_graph, core_data_manager_t* &core_data_manager)
				{
					auto word = tree_words->get_value();
					auto parent_word = tree_words->parent->get_value();

					if (word->words_base.data == "\"" || word->words_base.data == "'")
					{
						return;
					}

					if (word->words_base.data == "{")
					{
						obj_base_t* next_object = nullptr, * previous_object = nullptr;

						for (size_t i = 0; i < tree_words->tree.size(); i++)
						{
							auto it_word = tree_words->tree[i]->get_value();
							bool is_read_else = false;

							if ((it_word->words_base.is_new_line() || it_word->words_base.is_space_tab()) && !word->is_read_string_end)
							{
								continue;
							}

							for (size_t w = i + 1; w < tree_words->tree.size(); w++)
							{
								if (!tree_words->tree[w]->get_value()->words_base.is_new_line() &&
									!tree_words->tree[w]->get_value()->words_base.is_space_tab() &&
									!(tree_words->tree[w]->get_value()->words_base.data == "not" || tree_words->tree[w]->get_value()->words_base.data == "!")
									)
								{
									next_object = tree_words->tree[w]->get_value();
									break;
								}
							}

							if (i > 0) {

								for (size_t w = i - 1; w > 0; w--)
								{
									if (!tree_words->tree[w]->get_value()->words_base.is_new_line()
										&& !tree_words->tree[w]->get_value()->words_base.is_space_tab()
										&& !(tree_words->tree[w]->get_value()->words_base.data == "not" || tree_words->tree[w]->get_value()->words_base.data == "!")
										)
									{
										previous_object = tree_words->tree[w]->get_value();
										break;
									}
								}

							}

							if ((it_word->words_base.is_new_line() || it_word->words_base.is_space_tab()) && !word->is_read_string_end)
							{
								continue;
							}

							if (it_word->words_base.data == "{" && (!it_word->is_read_property || !it_word->is_multi_property))
							{
								// TODO: fmt not support UTF-16 and UTF-32 =/
								it_word->obj.name = fmt::format("block{}", counter_autoblock++);

								if (current_keyword) {

									if (current_keyword->name == "type") {
										it_word->obj.add_flag(obj_flag_t::obj_type);
									}

									if (current_keyword->name == "group") {
										it_word->obj.add_flag(obj_flag_t::obj_group);
									}

								}

								it_word->obj.add_flag(obj_flag_t::obj_auto);

								it_word->obj.word.data = it_word->obj.name;

								if (previous_object) {

									if (previous_object->words_base.data == "->") {
										it_word->obj.is_condition_true = true;
									}

									if (previous_object->words_base.data == ":") {
										it_word->obj.is_condition_false = true;
									}
								}

								word->obj.values.push_back(it_word->obj);
								continue;
							}

							if (it_word->words_base.data == "\"" || it_word->words_base.data == "'") {

								word->is_read_string_end = !word->is_read_string_end;

								if (word->is_read_string_end)
								{
									obj_t tmp_obj = it_word->obj;

									tmp_obj.name = "";
									tmp_obj.word.data = "";

									for (size_t w = 0; w < tree_words->tree[i]->tree.size(); w++)
									{
										auto w_word = tree_words->tree[i]->tree[w]->get_value();

										tmp_obj.name += w_word->obj.name;
										// TODO: len position
										tmp_obj.word.data += w_word->words_base.data;
										tmp_obj.word.end_position = w_word->words_base.end_position + 1;
									}

									tmp_obj.add_flag(obj_flag_t::obj_value);

									tmp_obj.word.start_position = it_word->words_base.start_position;

									tmp_obj.word.number_line = it_word->words_base.number_line;

									if (previous_object) {

										if (previous_object->words_base.data == "->") {
											tmp_obj.is_condition_true = true;
										}

										if (previous_object->words_base.data == ":") {
											tmp_obj.is_condition_false = true;
										}
									}


									word->obj.values.push_back(tmp_obj);
								}

								continue;
							}

							if (!it_word->is_read_property && !it_word->is_multi_property)
							if (
								it_word->words_base.data != "not" &&
								it_word->words_base.data != "!" &&
								it_word->words_base.data != "and" &&
								it_word->words_base.data != "or" &&
								it_word->words_base.data != "," &&
								it_word->words_base.data != "=" &&
								it_word->words_base.data != "exists" &&
								it_word->words_base.data != "glue" &&
								it_word->words_base.data != "split" &&
								it_word->words_base.data != "ignore" &&
								it_word->words_base.data != "maybe" &&
								it_word->words_base.data != "return" &&
								it_word->words_base.data != "exit" &&
								it_word->words_base.data != "recursion" &&
								it_word->words_base.data != "repeat" &&
								it_word->words_base.data != "repeat_end" &&
								it_word->words_base.data != "break" &&
								it_word->words_base.data != "breakpoint" &&
								it_word->words_base.data != "{" &&
								it_word->words_base.data != "}" &&
								it_word->words_base.data != "->" &&
								it_word->words_base.data != ":"
								)
							{
								if (!it_word->words_base.data.empty())
								{
									if (previous_object) {

										if (previous_object->words_base.data == "->") {
											it_word->obj.is_condition_true = true;
										}

										if (previous_object->words_base.data == ":") {
											it_word->obj.is_condition_false = true;
										}
									}

									it_word->obj.name = it_word->words_base.data;
									it_word->obj.word = it_word->words_base;

									/*
									  We need cast keyword true and false in value
									*/
									if (it_word->obj.name == "true") {

										it_word->obj.add_flag(obj_flag_t::obj_value);
										it_word->obj.add_flag(obj_flag_t::obj_true);
										it_word->obj.add_flag(obj_flag_t::obj_auto);

									}
									else if (it_word->obj.name == "false")
									{
										it_word->obj.add_flag(obj_flag_t::obj_value);
										it_word->obj.add_flag(obj_flag_t::obj_false);
										it_word->obj.add_flag(obj_flag_t::obj_auto);
									}
									else
									{
										it_word->obj.add_flag(obj_flag_t::obj_type);
									}

									word->obj.values.push_back(it_word->obj);
								}
							}
						}
					}
				}

				void process_parse_tree(tree_obj_base_t* tree_words, core_data_manager_t* &core_data_manager)
				{
					auto word        = tree_words->get_value();
					auto parent_word = tree_words->parent->get_value();

					if (!word)
						return;

					if (word->words_base.data == "\"" || word->words_base.data == "'")
					{
						is_read_string = !is_read_string;
						return;
					}

					if ((word->words_base.is_new_line() || word->words_base.is_space_tab()) && !is_read_string)
						return;

					if (parent_word) {

						if (is_read_string)
						{
							if (parent_word->words_base.data == "\"" || parent_word->words_base.data == "'")
							{
								word->obj.add_flag(obj_flag_t::obj_value);

								word->obj.name = word->words_base.data;
								word->obj.word = word->words_base;
							}
						}
						else
						{
							if (parent_word->is_read_property)
							{
								if (word->c_str() == "{") {

									word->is_read_property  = true;							
									word->is_multi_property = true;
									parent_word->is_multi_property = true;

								} else
								if (word->c_str() == "}") {
									parent_word->is_multi_property = false;
									parent_word->is_read_property  = false;
								} else								
								if (parent_word->is_multi_property) {

									tree_obj_base_t* object_for_property = nullptr;
									bool is_next_read = false;

									// tree_words->parent
									get_left_propery_block(tree_words->parent->previous, object_for_property, is_next_read, "=");

									if (object_for_property) {

										auto word_object_for_property = object_for_property->get_value();

										word->is_read_property = true;

										bool result = word_object_for_property->obj.add_flag_from_string_to_property(word->words_base.data);

										// no keyword
										if (!result)
										{
											if (word->words_base.data != ",")
											{
												fmt::print("No property: {}\n", word->words_base.data);
											}
										}
									}

								}
								  else {

								  tree_obj_base_t* object_for_property = nullptr;
								  bool is_next_read = false;

								  get_left_propery_block(tree_words->previous, object_for_property, is_next_read, "=");

								  if (object_for_property) {
 
								 	 auto word_object_for_property = object_for_property->get_value();

									 word->is_read_property = true;

									 bool result = word_object_for_property->obj.add_flag_from_string_to_property(word->words_base.data);

									// no keyword
									 if (!result)
									 { 
										if (word->words_base.data != ",")
										{
											fmt::print("No property: {}\n", word->words_base.data);
										}
									 }
								   }

								  word->is_read_property = true;
								  parent_word->is_read_property = false;
							   }
						
							}
							else {

								if (word->words_base.data == "=")
								{
									parent_word->is_read_property = true;
								}

								parent_word->obj.add_flag_from_string_to_chain(word->words_base.data);

								if (word->words_base.data == "not" || word->words_base.data == "!")
								{
									tree_obj_base_t* next_tree = nullptr;
									get_next_with_ignore_space_tab_newline(tree_words->next, next_tree);

									if (next_tree)
									{
										auto next_word = next_tree->get_value();
										auto this_word = tree_words->get_value();

										if (next_word)
										{
											// yeah, for double not
											next_word->obj.add_inversion_flag(this_word->obj, obj_flag_t::obj_not);
										}
									}
								}
							}
						}
					}

					// check pointer parent_word for nullptr status
					bool is_read_property = parent_word ? parent_word->is_read_property : false;

					if (!is_read_string && !is_read_property) {

						if (is_have_keyword && current_keyword) {

							if (current_keyword->is_have_name && !current_keyword->is_was_read_name)
							{
								current_keyword->is_was_read_name = true;

								if (current_keyword->is_read_object)
								{
									current_keyword->tree_obj_base.obj.name = word->words_base.data;
									current_keyword->tree_obj_base.obj.word = word->words_base;
								}
							}

							if (word->words_base.data == ";")
							{
								auto obj = new obj_t;

								for (size_t i = last_position; i < tree_words->position; i++)
								{
									if (tree_words->parent->tree[i]->get_value()->words_base.data == "{")
									{
										*obj = tree_words->parent->tree[i]->get_value()->obj;

										// TODO: NO CHECK SIZE!!!
										for (std::size_t w = 0; w < obj->values.size(); w++)
										{
											if (obj->values[w].is_condition_true)
											{
												conditional_element_t* condition_element = new conditional_element_t;
												conditional_element_t::memory_manager_t memory_manager;

												conditional_element_t::erase_manager_t erase_list;

												std::size_t insert_position = 0;
												bool is_can_insert = false;

												bool is_skip = false;

												conditional_element_t::read_condition(obj, w, condition_element, false, false, w, is_skip, memory_manager, erase_list);
												//print_condition(condition_element);

												if (!erase_list.data.empty()) {
													insert_position = erase_list.data[0];
													is_can_insert = true;
												}

												if (is_can_insert)
												{
													w = insert_position;

													for (size_t k = 0; k < erase_list.data.size(); k++)
													{
														obj->values.erase(obj->values.begin() + (erase_list.data[k] - k));
													}

													// here insert
													obj_t insert_obj;
													bool  is_write_read_obj = false;
													morph_condition(condition_element, insert_obj, false, false, nullptr, is_write_read_obj);
													obj->values.insert(obj->values.begin() + insert_position, insert_obj);
												}

												memory_manager.delete_alloc();
												erase_list.clear();
											}
										}
									}
								}

								last_position = tree_words->position;

								obj->name = current_keyword->tree_obj_base.obj.name;
								obj->word = current_keyword->tree_obj_base.obj.word;

								if (parent_word)
									parent_word->is_read_property = false;

								if (current_keyword->name == "type")
								{
									obj->add_flag(obj_flag_t::obj_type);

									core_data_manager->all_types->push_back(obj);
								}

								if (current_keyword->name == "group") {

									obj->add_flag(obj_flag_t::obj_group);
									obj->add_flag(obj_flag_t::obj_type);

									core_data_manager->all_groups->push_back(obj);
								}

								if (current_keyword->name == "test") {
									core_data_manager->all_tests->push_back(obj);
								}

								if (current_keyword->name == "#") {

									for (const auto& it : obj->values) {

										if (it.name == "group")
										{
											core_data_manager->parser_core->is_render_group = true;
										}

										if (it.name == "tree")
										{
											core_data_manager->parser_core->is_render_tree = true;
										}
									}

									delete obj;
								}

								current_keyword = nullptr;
								is_have_keyword = false;
							}
						}
						else {

							bool is_found = false;
							std::size_t i = 0;

							for (auto& pel_keyword : pel_keywords)
							{
								if (pel_keyword.name == word->words_base.data)
								{
									is_have_keyword = true;
									current_keyword = &pel_keywords[i];

									current_keyword->reset();

									tree_words->root->set_value(&current_keyword->tree_obj_base);

									is_found = true;
									break;
								}

								i++;
							}

							if (!is_found)
							{
								fmt::print("Error! What it is {} ?\n", word->words_base.data);
							}

						}
					}
				}
		};
	}
}