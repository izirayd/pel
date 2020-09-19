#pragma once

#include "..\executive\cmd\groups_logic\groups.hpp"

#include "..\executive\cmd\main_logic\cmd_make.hpp"
#include "..\executive\cmd\groups_logic\cmd_groups.hpp"
#include "..\executive\cmd\groups_logic\cmd_groups_make.hpp"

#include "..\..\block_parser.hpp"


#include "..\..\obj.hpp"

#include "../executive/error_context.hpp"
#include "../executive/executive_parser.hpp"


/* PEL from cplusplus */
namespace pel
{
	using namespace fmt;

	class pel_parser_t;

	void pel_compilation(pel::pel_parser_t& pel_lang, parser::executive::global_gcmd_t* global_gcmd, parser::executive::groups::global_gcmd_group_t *global_gcmd_group);

	class pel_parser_t: public parser::block_parser_t, public parser::words_parser_t
	{
	public:
		std::string                 code;
		words_t                     words;
		tree_words_t                tree;
		int32_t                     level = -1;
		error_context_t				error_context;
		code_render_t				code_render;

		std::vector<obj_t*>		    all_types;
		std::vector<obj_t*>		    all_tests;
		std::vector<obj_t*>		    all_groups;

		bool is_render_code_in_console  = false;
	
		groups::groups_t groups;

		parser::executive::parser_engine_t parser_engine;

		std::string version;

		void delete_alloc()
		{
			tree.delete_tree();
			words.delete_alloc();
			code_render.delete_alloc();

			std::clear(all_types);
			std::clear(all_tests);
			std::clear(all_groups);		
		}

		void process_parse()
		{
			parser::words_parser_t::process_parse(code, words);
		}

		void clear() {

			for (size_t i = 0; i < all_types.size(); i++)
			{
				if (all_types[i]) {
					all_types[i]->clear();	
					delete all_types[i]; 
				}
			}

			for (size_t i = 0; i < all_tests.size(); i++)
			{
				if (all_tests[i]) {
					all_tests[i]->clear();				
					delete all_tests[i];
				}
			}

			for (size_t i = 0; i < all_groups.size(); i++)
			{
				if (all_groups[i]) {
					all_groups[i]->clear();
					delete all_groups[i];
				}
			}

			all_types.clear();
			all_tests.clear();
			all_groups.clear();
			code_render.clear();
		}

		void clear_pe() {
			parser_engine.delete_alloc();
			parser_engine.code.clear();
			parser_engine.delete_global_gcmd();
		}

		void is_in_list(pel::groups::group_t *group, pel::groups::vector_objects_t &vector_objects, bool &is_result)
		{
			for (auto& current_words : vector_objects)
			{
				if (current_words.group == group) {
					is_result = true;
					break;
				}
			}

			is_result = false;
		}
		

		void is_no_group(const pel::groups::vector_objects_t& last_iteration_objects, const std::vector<pel::groups::group_t*> &groups, std::vector<pel::groups::group_t*> &result)
		{
			for (const auto &it: last_iteration_objects)
			{
				bool is_find = false;

				for (const auto& group : groups) // ?
				{
					if (it.group == group)
					{
						is_find = true;
					}
				}

				if (!is_find) {

					bool is_find_sub = false;

					for (const auto& sub: result)
					{
						if (sub == it.group)
						{
							is_find_sub = true;
							break;
						}

					}

					if (!is_find_sub)
						result.push_back(it.group);
				}
			}		
		}

		void run()
		{
			pel_compilation(*this, &parser_engine.global_gcmd, &parser_engine.global_gcmd_group);
		
			bool is_tree_model_words = false;

			if (is_tree_model_words)
			{
				// make tests
				for (size_t i = 0; i < all_tests.size(); i++)
				{
					if (all_tests[i])
					{
						for (auto sub_test : all_tests[i]->values)
						{
							parser_engine.code = sub_test.name;

							std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

							parser_engine.process_parse();

							std::intptr_t new_position = -1;
							std::vector<std::pair<std::string, std::string>> array_symbols;

							array_symbols.push_back({ "{", "}" });
							array_symbols.push_back({ "\"", "\"" });

							parser_engine.block_parse(&parser_engine.tree, 0, parser_engine.words.words.size(), new_position, array_symbols, &parser_engine.words);

							std::chrono::high_resolution_clock::time_point timer_parse_tree_start = std::chrono::high_resolution_clock::now();

							parser_engine.parse_tree();

							std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();

							std::chrono::duration<double, std::micro> result = end - start;
							std::chrono::duration<double, std::micro> result_timer_parse_tre = end - timer_parse_tree_start;

							print(fg(fmt::color::azure), "\nProcess parse tree end: ");
							print(fg(fmt::color::coral), "{}", result_timer_parse_tre.count());
							print(fg(fmt::color::azure), "us\n");

							print(fg(fmt::color::azure), "\nProcess parse all steps end: ");
							print(fg(fmt::color::coral), "{}", result.count());
							print(fg(fmt::color::azure), "us\n");

							parser_engine.delete_alloc();
						}
					}
				}
			}
			else
			{
				parser_engine.group_init();

				pel::groups::group_result_t group_result;		
				pel::groups::array_words_t  array_words;
				std::vector<pel::groups::group_t*> result;

				//TODO: can we get information about total count groups?
				group_result.groups.reserve(32);
				array_words.data.reserve(32);
				result.reserve(32);

				for (size_t i = 0; i < 32; i++)
				{
					array_words.data.push_back({});
				}

				for (auto &it: array_words.data)
				{
					it.words.reserve(32);
				}


				bool last_glut = false, last_split = false;

				if (parser_engine.is_render_group)
				{
					print(fg(fmt::color::green_yellow), "Debug groups to base symbols\n\n");
				}

				for (size_t i = 0; i < all_tests.size(); i++)
				{
					if (all_tests[i])
					{
						for (auto sub_test : all_tests[i]->values)
						{
							parser_engine.code = sub_test.name;

							std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

							for (size_t w = 0; w < parser_engine.code.length(); w++)
							{
								// copy iteration ? sound sad
								groups::group_element_t element(parser_engine.code[w]);

								parser_engine.group_parse(element, group_result);

								if (parser_engine.is_render_group) {
									print(fg(fmt::color::thistle), "\"{0}\": ", (char)element.element);

									std::size_t k = 0;

									for (auto& it : group_result.groups)
									{
										print(fg(fmt::color::blanched_almond), "{0}", it->name);
										print(fg(fmt::color::red), "[{0}]", it->last_position);

										if ((group_result.groups.size() - 1) != k) {
											
												print(fg(fmt::color::coral), " or ");
										}
										else
										{								
												print(";");
										}

										k++;
									}
								}
			
								if (!group_result.groups.empty())
								{
									if (group_result.groups[0]->is_split || group_result.groups[0]->is_ignore)
									{
										std::size_t level = 0;
										parser_engine.process_executive_array_words(array_words, level);

										array_words.clear();

										// здесь начать разделение
										if (group_result.groups[0]->is_ignore)
										{


										}

										if (group_result.groups[0]->is_split)
										{
											result.clear();

											for (auto& result_group : group_result.groups)
											{
												std::size_t position = result_group->last_position;

												if (array_words.data.size() < position + 1)
												{
													array_words.data.push_back({});
												}

												array_words.data[position].position = position;

												// its so bad T_T
												is_no_group(array_words.data[position].words, group_result.groups, result);

												bool is_find = false;

												for (auto& current_words : array_words.data[position].words)
												{
													if (current_words.group == result_group)
													{
														current_words.data += element.element;
														is_find = true;
														break;
													}
												}

												if (!is_find)
												{
													std::size_t new_position = position;

													for (auto& it : result)
													{
														new_position = it->last_position + 1;
														//it->last_position += 1; // +2? 
													}

													if (array_words.data.size() < new_position + 1)
													{
														array_words.data.push_back({});
													}

													array_words.data[new_position].position = new_position;

													pel::groups::object_t object;

													object.data += element.element;
													object.group = result_group;
													result_group->last_position = new_position;

													array_words.count_position++;
													array_words.data[new_position].words.push_back(object);
												}
											}

											for (auto& it : result)
											{
												it->last_position += 1;
											}

											parser_engine.process_executive_array_words(array_words, level);
											array_words.clear();
										}
									}
									else
									{								
										result.clear();

										for (auto& result_group : group_result.groups)
										{		
											std::size_t position = result_group->last_position;

											if (array_words.data.size() < position + 1)
											{
												array_words.data.push_back({});
											}

											array_words.data[position].position = position;

											// its so bad T_T
											is_no_group(array_words.data[position].words, group_result.groups, result);

											bool is_find = false;

											for (auto &current_words: array_words.data[position].words)
											{																				
												if (current_words.group == result_group)
												{
													current_words.data += element.element;
													is_find     = true;
													break;
												}	
											}

											if (!is_find)
											{
												std::size_t new_position = position;

												for (auto &it : result)
												{
													new_position = it->last_position + 1;
													//it->last_position += 1; // +2? 
												}

												if (array_words.data.size() < new_position + 1)
												{
													array_words.data.push_back({});
												}

												array_words.data[new_position].position = new_position;

												pel::groups::object_t object;

												object.data += element.element;
												object.group = result_group;
												result_group->last_position = new_position;

												array_words.count_position++;
												array_words.data[new_position].words.push_back(object);
											}																						
										}

										for (auto& it : result)
										{
											it->last_position += 1;
										}
									}
								}
															
								if (parser_engine.is_render_group) {

									if (!group_result.groups.empty())
									{
										print("\n");
									}

									if (group_result.groups.empty())
									{
										print(fg(fmt::color::red), "null\n");
									}
								}

								group_result.clear();
							}
							
							std::size_t level = 0;
							parser_engine.process_executive_array_words(array_words, level);
							array_words.clear();


							std::chrono::high_resolution_clock::time_point timer_parse_tree_start = std::chrono::high_resolution_clock::now();
				
							std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();

							std::chrono::duration<double, std::micro> result = end - start;
							std::chrono::duration<double, std::micro> result_timer_parse_tree = end - timer_parse_tree_start;

						

							print(fg(fmt::color::azure), "\nProcess parse tree end: ");
							print(fg(fmt::color::coral), "{}", result_timer_parse_tree.count());
							print(fg(fmt::color::azure), "us\n");

							print(fg(fmt::color::azure), "Process parse all steps end: ");
							print(fg(fmt::color::coral), "{}", result.count());
							print(fg(fmt::color::azure), "us\n");

						
							parser_engine.delete_alloc();
						}
					}					
				}
			}
		}

		// cpp prototype version
		void parse_words(bool is_render_code = false, bool is_render_line_number = false)
		{
			bool is_read_type_name = false;
			bool is_read_type_body = false;
			bool is_read_type_property = false;

			obj_t *obj = nullptr;
			obj_t body_obj;

			bool is_read_string = false;
			
			bool is_and = false;
			bool is_or  = false;
			bool is_xor = false;
			bool is_not = false;

			bool is_render_num	 = is_render_line_number;

			bool is_read_version = false;

			bool is_test  = false;
			bool is_type  = false;
			bool is_group  = false;

			bool is_run	  = false;
			bool is_clear = false;

			bool is_string_one = false;
			bool is_string_two = false;

			bool is_module = false;
			bool is_print_tree = false;
			bool is_global_property = false;
			bool is_debug = false;

			bool is_module_render_code  = false;
			bool is_module_render_group = false;

			bool is_ignore_next_iteration = false;

			std::size_t i = 0;

#define continue_with_inc i++; continue;

			for (auto& word : words.words)
			{	
				if (is_ignore_next_iteration) {
					is_ignore_next_iteration = false;
					continue_with_inc;
				}

				if (is_render_line_number)
				{
					if (is_render_num /*&& !word.is_new_line()*/)
					{
						is_render_num = false;
						
						int count_space = 1;

						if (word.number_line < 1000)
							count_space = 1;

						if (word.number_line < 100)
							count_space = 2;

						if (word.number_line < 10)
							count_space = 3;

					/*	print(fg(color::dark_cyan), "{}", (char)221);

						if (count_space == 3)
						 print(fg(color::lime_green), " {} ", word.number_line);

						if (count_space == 2)
							print(fg(color::lime_green), "{} ", word.number_line);

						if (count_space == 1)
							print(fg(color::lime_green), "{} ", word.number_line);*/
					}

					if (word.is_new_line())
					{
						is_render_num = true;

						if (is_render_code) {
							code_render.push({ word });
;						//	print("{}", word.data);
						}
					}
				}

				if (is_read_string)
				{ 
					if (
						(is_string_one && (word.data != "\"")) || 
						(is_string_two && (word.data != "'"))
					   )
					{

						words_base_t string_word;
						words_base_t real_word;

						bool is_find = false;

						string_word = word;
						real_word = word;

						if (word.data == "\\" && words.words.size() != (i + 1))
						{
							if (words.words[i + 1].data.size() >= 1) {
								if (words.words[i + 1].data[0] == 'n')
								{
									string_word.data = "\n";
									real_word.data += "n";

									is_find = true;
								}

								if (words.words[i + 1].data[0] == 'a')
								{
									string_word.data = "\a";
									real_word.data += "a";

									is_find = true;
								}

								if (words.words[i + 1].data[0] == 'b')
								{
									string_word.data = "\b";
									real_word.data += "b";

									is_find = true;
								}

								if (words.words[i + 1].data[0] == 't')
								{
									string_word.data = "\t";
									real_word.data += "t";

									is_find = true;
								}

								if (words.words[i + 1].data[0] == 'v')
								{
									string_word.data = "\v";
									real_word.data += "v";

									is_find = true;
								}

					/*			if (words.words[i + 1].data[0] == 'f')
								{
									string_word.data = "\f";
									real_word.data += "f";

									is_find = true;
								}*/

								if (words.words[i + 1].data[0] == 'r')
								{
									string_word.data = "\r";
									real_word.data += "r";

									is_find = true;
								}

								if (words.words[i + 1].data.size() > 1)
								{
									for (size_t w = 0; w < words.words[i + 1].data.size() - 1; w++)
									{
										words.words[i + 1].data[w] = words.words[i + 1].data[w + 1];
									}

									words.words[i + 1].data.resize(words.words[i + 1].data.size() - 1);			
								}
								else
								{
									if (is_find)
										is_ignore_next_iteration = true;
								}
							}
						}

					

						body_obj.name += string_word.data;

						//
						/*
							Its allocate so bad and it need only for we can got word end position
							may be delete it?!
						*/
					
						if (body_obj.word.start_position == 0) {
							body_obj.word = string_word;
						}
						else {
							body_obj.word.end_position = string_word.end_position;
						}

						if (!string_word.is_new_line())
							if (is_render_code) 
							{
								code_render.push({ fg(color::thistle), real_word });
							}

						continue_with_inc;
					}
				}

				if (!word.is_word() && !word.is_symbol())
				{
					if (is_render_code) {
						if (!word.is_new_line()) {
							code_render.push({ word });
							//print("{}", word.data);
						}
					}
				
					continue_with_inc;
				}

				if (word.data == ";" && !is_read_string)
				{
					if (is_type)
						all_types.push_back(obj);

					if (is_test)
						all_tests.push_back(obj);

					if (is_group) 
						all_groups.push_back(obj);
																	
					body_obj.clear();
					is_read_type_name = false;
					is_read_type_body = false;
					is_read_type_property = false;
					is_read_version = false;

					is_test  = false;
					is_type  = false;
					is_group = false;

					if (is_render_code)
					{
					//	print(fg(color::bisque), "{}", word.data);
						code_render.push({ fg(color::bisque), word });
					}

					if (is_run)
					{
						if (is_debug)
						{
							parser_engine.is_render_tree = is_print_tree;
							is_render_code_in_console = is_module_render_code;
							parser_engine.is_render_group = is_module_render_group;
						}
						else
						{
							parser_engine.is_render_tree = false;
							is_render_code_in_console = false;
						}

					//	print("\n");
							run();
					//	print("\n");
						is_run = false;
					}

					if (is_clear)
					{
						clear();
						clear_pe();
						is_clear = false;
					}

					continue_with_inc;
				}

				if (word.data == "version")
				{
					is_read_version = true;
					if (is_render_code) {
						code_render.push({ fg(color::coral), word });
						//print(fg(color::coral), "{}", word.data);
					}

					continue_with_inc;
				}

				if (is_read_version)
				{
					version += word.data;
					is_read_version = false;
					if (is_render_code)
					{
						//print(fg(color::linen), "{}", word.data);
						code_render.push({ fg(color::linen), word });
					}
				}

				if (word.data == "run")
				{
					is_run = true;		

					if (is_render_code)
					{
						code_render.push({ fg(color::dark_violet), word });
						//print(fg(color::dark_violet), "{}", word.data);
					}
				}				
				
				if (word.data == "clear")
				{
					is_clear = true;
					if (is_render_code)
					{
						code_render.push({ fg(color::dark_violet), word });
						//print(fg(color::dark_violet), "{}", word.data);
					}
				}
				
				if (word.data == "#")
				{
					is_global_property = true;

					if (is_render_code)
					{
						code_render.push({ fg(color::dark_khaki), word });
					}

					continue_with_inc;
				}

				if (is_global_property)
				{
					if (word.data == "module")
					{
						is_module = true;

						if (is_render_code)
						{
							code_render.push({ fg(color::dark_magenta), word });
						}
					}

					is_global_property = false;
					continue_with_inc;
				}

				if (is_module)
				{
					if (word.data == "debug")
					{
						is_debug = true;

						if (is_render_code)
							code_render.push({ fg(color::orange_red), word });

					}

					if (word.data == "print_tree")
					{
						is_print_tree = true;

						if (is_render_code)
							code_render.push({ fg(color::dark_khaki), word });
					}

					if (word.data == "print_code")
					{
						is_module_render_code = true;

						if (is_render_code)
							code_render.push({ fg(color::dark_khaki), word });
					}
					
					if (word.data == "print_group")
					{
						is_module_render_group = true;

						if (is_render_code)
							code_render.push({ fg(color::dark_khaki), word });
					}

					is_module = false;
					continue_with_inc;
				}

   				if (word.data == "type")
				{
					obj = new obj_t;
					is_read_type_name = true;
					obj->is_type = true;
					is_type = true;

					if (is_render_code)
					{
						code_render.push({ fg(color::aquamarine), word });
						//print(fg(color::aquamarine), "{}", word.data);
					}
					continue_with_inc;
				}

				if (word.data == "group")
				{
					obj = new obj_t;
					is_read_type_name = true;
					obj->is_type = true;
					is_group = true;

					if (is_render_code)
					{
						code_render.push({ fg(color::lime), word });
						//print(fg(color::aquamarine), "{}", word.data);
					}
					continue_with_inc;
				}

				if (word.data == "test")
				{
					obj = new obj_t;
					is_read_type_name = true;
					obj->is_type	  = true;
					is_test			  = true;

					if (is_render_code)
					{
						code_render.push({ fg(color::azure), word });
						//print(fg(color::azure), "{}", word.data);
					}
					continue_with_inc;
				}

				if (word.data == ":")
				{
					if (is_render_code)
					{
						code_render.push({ fg(color::red), word });
						//print(fg(color::red), "{}", word.data);
					}

					if (is_read_type_name)
						is_read_type_name = false;		
				
					continue_with_inc;
				}

				if (word.data == "{") {
					if (is_render_code)
					{
						code_render.push({ fg(color::beige), word });
						//print(fg(color::beige), "{}", word.data);
					}
					is_read_type_body = true;
					continue_with_inc;
				}

				if (word.data == "}") {
					is_read_type_body = false;

					body_obj.clear();

					if (is_render_code)
					{
						code_render.push({ fg(color::beige), word });
						//print(fg(color::beige), "{}", word.data);
					}

					continue_with_inc;
				}

				if (word.data == "," && is_read_type_property)
				{
					if (is_render_code)
					{
						code_render.push({ fg(color::azure), word });
					}

					continue_with_inc;
				}

				if (word.data == "=")
				{
					if (is_render_code)
					{
						code_render.push({ fg(color::azure), word });
						//print(fg(color::azure), "{}", word.data);
					}

					is_read_type_property = true;
					continue_with_inc;
				}

				if (is_read_type_property)
				{
					if (is_render_code)
					{
						code_render.push({ fg(color::red), word });
						//print(fg(color::red), "{}", word.data);
					}

					if (word.data == "ex")
					{		
						if (obj)
							obj->is_ex = true;
					}

					if (word.data == "glue")
					{
						if (obj)
							obj->is_glue = true;
					}

					if (word.data == "split")
					{
						if (obj)
							obj->is_split = true;
					}

					if (word.data == "ignore")
					{
						if (obj)
							obj->is_ignore = true;
					}

					if (word.data == "true")
					{
						body_obj.is_true = true;
					}

					if (word.data == "false")
					{
						body_obj.is_true = false;
					
					}
					continue_with_inc;
				}

				if (is_read_type_name)
				{
					if (obj) {
						obj->name = word.data;
						obj->word = word;
					}
					if (is_render_code)
					{
						code_render.push({ fg(color::blanched_almond), word });
						//print(fg(color::blanched_almond), "{}", word.data);
					}
				}

				if (is_read_type_body && !is_read_type_property)
				{
					bool is_bin_logic = false;

					// Дело в том, что флаги состояния бинарной операции
					// Остаются даже для следующей итерации
					// А вот is_iter_word_bin существует в рамках этой итерации
					bool is_iter_word_bin = false;
					bool is_ignore_print  = false;

					if (word.data == "and" || word.data == ",") {
						is_and = true;
						is_xor = false;
						is_or  = false;

						is_iter_word_bin = true;
					}
					
					if (word.data == "xor" || word.data == "^") {
						is_xor = true;
						is_and = false;
						is_or = false;

						is_iter_word_bin = true;
					}

					if (word.data == "or" || word.data == "|") {
						is_or  = true;
						is_and = false;
						is_xor = false;

						is_iter_word_bin = true;
					}

					if (word.data == "not" || word.data == "!") {
						is_not = true;

						is_iter_word_bin = true;
					}

					if (is_xor || is_or || is_and || is_not)
						is_bin_logic = true;

					if (word.data == "\"" || word.data == "'")
					{
						if (is_render_code)
						{
							code_render.push({ fg(color::azure), word });
							//print(fg(color::azure), "{}", word.data);
						}

						is_read_string = !is_read_string;

						if (is_read_string)
						  body_obj.is_value = true;
						
						if (word.data == "\"")
							 is_string_one = true;
	
						if (word.data == "'")
							 is_string_two = true;

						if (!is_read_string)
						{
							is_string_one = false;
							is_string_two = false;

						}

						if (body_obj.is_value && !is_read_string)
						{
							is_bin_logic = true; 
							is_ignore_print = true;
						}
						else
						{
							continue_with_inc;
						}
					}
					else
					{
						if (!is_read_string && !is_bin_logic)
							body_obj.is_type = true;
					}

					if (is_bin_logic && !is_read_string)
					{
						if (is_iter_word_bin) {
							if (!is_ignore_print)
								if (is_render_code)
								{
									code_render.push({ fg(color::coral), word });
									//print(fg(color::coral), "{}", word.data);
								}
						}
						else
						{
							if (!is_ignore_print)
								if (is_render_code)
								{
									if (body_obj.is_value)
									{
										code_render.push({ fg(color::thistle), word });
										//print(fg(color::thistle), "{}", word.data);
									}
								}
						}

						body_obj.is_not = is_not;
						body_obj.is_xor = is_xor;
						body_obj.is_or  = is_or;
						body_obj.is_and = is_and;

						if (!body_obj.name.empty()) {

							if (obj)
								obj->values.push_back(body_obj);

							body_obj.clear();

							is_not = false;
							is_xor = false;
							is_or  = false;
							is_and = false;

						} 
						else if (!is_iter_word_bin)
						{
							body_obj.word = word;
							body_obj.name = word.data;
							body_obj.is_type = true;

							if (body_obj.is_type) {
								code_render.push({ fg(color::blanched_almond), word });
								//print(fg(color::blanched_almond), "{}", word.data);
							}

							if (!body_obj.name.empty()) {

								if (obj)
									obj->values.push_back(body_obj);

								body_obj.clear();

								is_not = false;
								is_xor = false;
								is_or  = false;
								is_and = false;
							}
						}
	
						continue_with_inc;
					}
					else if (!is_read_string)
					{
						if (body_obj.is_type) {

							body_obj.word = word;
							body_obj.name = word.data;

							if (!body_obj.name.empty()) {

								if (obj)
									obj->values.push_back(body_obj);

								if (is_render_code)
								{
									code_render.push({ fg(color::blanched_almond), word });
									//print(fg(color::blanched_almond), "{}", body_obj.name);
								}

								body_obj.clear();

								is_not = false;
								is_xor = false;
								is_or  = false;
								is_and = false;
							}

							continue_with_inc;
						}

						if (body_obj.is_value) {
							if (is_render_code)
							{
								code_render.push({ fg(color::thistle), word });
								//print(fg(color::thistle), "{}", body_obj.name);
							}
						}
					}
				}

				i++;
			}

		
			//print("\n");
		}
	};

	void get_property(parser::executive::gcmd_t* gcmd, const pel::obj_t* obj, const pel::obj_t *original_obj = nullptr)
	{
		parser::executive::cmd_t* cmd		 = &gcmd->get_value();
		parser::executive::cmd_t* parent_cmd = &gcmd->parent->get_value();

		cmd->value = obj->name;

		bool is_or = false, is_xor = false, is_and = false;

		for (auto &it: obj->values)
		{	
			if (it.is_or)
				is_or = true;

			if (it.is_xor)
				is_xor = true;

			if (it.is_and)
				is_and = true;
		}

		if (obj->is_type)
			std::add_flag(cmd->flag, parser::executive::parser_type);

		if (obj->is_value)
			std::add_flag(cmd->flag, parser::executive::parser_value);

		if (obj->is_ex)
			std::add_flag(cmd->flag, parser::executive::parser_ex);

		if (obj->is_not)
			std::add_flag(cmd->flag, parser::executive::parser_not);

		if (is_or)
		{
			std::add_flag(cmd->flag, parser::executive::parser_or);
			std::del_flag(cmd->flag, parser::executive::parser_and);
		}

		if (is_xor)
		{
			std::add_flag(cmd->flag, parser::executive::parser_xor);
			std::del_flag(cmd->flag, parser::executive::parser_and);
		}

		if (is_and)
		{
			std::add_flag(cmd->flag, parser::executive::parser_and);
			std::del_flag(cmd->flag, parser::executive::parser_or);
			std::del_flag(cmd->flag, parser::executive::parser_xor);
		}

		if ((!cmd->is_or() && !cmd->is_xor() && !cmd->is_and()))
			std::add_flag(cmd->flag, parser::executive::empty_operation);
		

		if (obj->is_or) {
			std::add_flag(parent_cmd->flag, parser::executive::parser_or);
			std::del_flag(parent_cmd->flag, parser::executive::parser_and);
		}

		if (obj->is_xor) {
			std::add_flag(parent_cmd->flag, parser::executive::parser_xor);
			std::del_flag(parent_cmd->flag, parser::executive::parser_and);
		}

		//if (!obj->is_or && !obj->is_xor)
		//{
		//	if (!parent_cmd->is_or() && !parent_cmd->is_xor())
		//		if (!parent_cmd->is_and())
		//			std::add_flag(parent_cmd->flag, parser::executive::parser_and);
		//}

		gcmd->flush_value();
	}

	void no_ex(
		pel::pel_parser_t& pel_lang,
		parser::executive::gcmd_t* parent,
		const words_base_t& word,
		int& level,
		bool& is_stop,
		const obj_t &original_obj // это объект очереди и его свойства не копируются
	)
	{
		bool is_find = false;
		level++;

		if (is_stop)
			return;

		if (level > 128)
		{
			pel_lang.error_context.push(format("type {} has no way out of recursion!", word.data), "", word.number_line, word.start_position, word.end_position);
			is_stop = true;
			return;
		}

		for (const auto obj : pel_lang.all_types)
		{
			if (obj->is_type && obj->name == word.data)
			{
				is_find = true;

				get_property(parent, obj);

				for (const auto &sub_obj : obj->values)
				{
					parser::executive::gcmd_t* gcmd = parent->push({});

					if (sub_obj.is_type)
					{
						no_ex(pel_lang, gcmd, sub_obj.word, level, is_stop, sub_obj);

						level--;
					}
					else
					{
						get_property(gcmd, &sub_obj, &original_obj);
					}
				}

				break;
			}
		}

		if (!is_find)
		{
			pel_lang.error_context.push(format("type {} not declared!", word.data), "", word.number_line, word.start_position, word.end_position);
		}
	}

	void no_ex_groups(
		pel::pel_parser_t& pel_lang,
		parser::executive::groups::gcmd_group_t* parent,
		const words_base_t& word,
		int& level,
		bool& is_stop,
		const obj_t& original_obj // это объект очереди и его свойства не копируются
	)
	{
		bool is_find = false;
		level++;

		if (is_stop)
			return;

		// Блокирование рекурсии
		if (level > 128)
		{
			pel_lang.error_context.push(format("type {} has no way out of recursion!", word.data), "", word.number_line, word.start_position, word.end_position);

			is_stop = true;
			return;
		}

		for (const auto obj : pel_lang.all_groups)
		{
			if (obj->is_type && obj->name == word.data)
			{
				is_find = true;

				parser::executive::groups::get_property(parent, obj);

				/*
				   Копирование свойств в цепочке
				*/

				if (original_obj.is_not)
				{
					auto cmd = &parent->get_value();

					if (std::check_flag(cmd->flag, parser::executive::groups::group_flag_t::operation_not))
					{
						std::del_flag(cmd->flag, parser::executive::groups::group_flag_t::operation_not);
					}
					else
					{
						std::add_flag(cmd->flag, parser::executive::groups::group_flag_t::operation_not);
					}
					
				}


				for (const auto& sub_obj : obj->values)
				{
					parser::executive::groups::gcmd_group_t* gcmd = parent->push({});

					if (sub_obj.is_type)
					{
						no_ex_groups(pel_lang, gcmd, sub_obj.word, level, is_stop, sub_obj);

						level--;
					}
					else
					{
						parser::executive::groups::get_property(gcmd, &sub_obj, &original_obj);
					}
				}

				break;
			}
		}

		if (!is_find)
		{
			pel_lang.error_context.push(format("group {} not declared!", word.data), "", word.number_line, word.start_position, word.end_position);
		}
	}


	/*
	  Не решена проблема наследования свойств у типа!
	*/
	void pel_compilation(pel::pel_parser_t& pel_lang, parser::executive::global_gcmd_t* global_gcmd, parser::executive::groups::global_gcmd_group_t *global_gcmd_group)
	{
		// find ex
		for (const auto obj_ex : pel_lang.all_types)
		{
			if (obj_ex) {
				if (obj_ex->is_ex)
				{
					parser::executive::gcmd_t* main = new parser::executive::gcmd_t;

					get_property(main, obj_ex);

					for (auto main_cmd : obj_ex->values)
					{
						parser::executive::gcmd_t* sub_main = main->push({});

						int  level   = 0;
						bool is_stop = false;

						if (main_cmd.is_type)
						{
							no_ex(pel_lang, sub_main, main_cmd.word, level, is_stop, main_cmd);
						}
						else
						{
							get_property(sub_main, &main_cmd);
						}
					}

					global_gcmd->push_back(main);
				}
			}
		}

		for (const auto obj_ex : pel_lang.all_groups)
		{
			if (obj_ex) {
				if (obj_ex->is_ex)
				{
					auto main = new parser::executive::groups::gcmd_group_t;

					parser::executive::groups::get_property(main, obj_ex);

					for (auto main_cmd : obj_ex->values)
					{
						auto sub_main = main->push({});

						int  level = 0;
						bool is_stop = false;

						if (main_cmd.is_type)
						{
							no_ex_groups(pel_lang, sub_main, main_cmd.word, level, is_stop, main_cmd);
						}
						else		
						{
							parser::executive::groups::get_property(sub_main, &main_cmd);
						}
					}

					global_gcmd_group->push_back(main);
				}
			}
		}

		if (pel_lang.is_render_code_in_console) {
			print("\n");
			print("\n");
				pel_lang.code_render.console_print();
			print("\n");
		}
	
		pel_lang.error_context.print_console(&pel_lang.code_render);
		
		if (!pel_lang.error_context.is_error()) {
			parser::executive::make_commands(global_gcmd, pel_lang.parser_engine.is_render_tree);		
			parser::executive::groups::make_commands(global_gcmd_group, pel_lang.parser_engine.is_render_tree && pel_lang.parser_engine.is_render_group);
		}
	}
}