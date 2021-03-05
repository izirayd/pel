#pragma once

#include "..\executive\cmd\groups_logic\groups.hpp"

#include "..\executive\cmd\main_logic\cmd_make.hpp"
#include "..\executive\cmd\groups_logic\cmd_groups.hpp"
#include "..\executive\cmd\groups_logic\cmd_groups_make.hpp"
#include "..\executive\cmd\main_logic\cmd_build.hpp"

#include "..\..\block_parser.hpp"


#include "..\..\obj.hpp"

#include "../executive/error_context.hpp"
#include "../executive/executive_parser.hpp"

#include "cpp_parser/cpp_parser.hpp"

#include "../../association_flags.hpp"

#include <stack>

/* PEL from cplusplus */
namespace pel
{
	using namespace fmt;

	// TODO: add
	class limits_t
	{
	public:
		std::size_t max_depth_recursuin;
		std::size_t max_chain_len;

		void set_default_values() {
			max_depth_recursuin = 128;
			max_chain_len = 512;
		}
		
	};

	// TODO: add
	class statistic_t
	{
	public:
		std::size_t count_operation;
	};

	class pel_parser_t;

	bool build(
		pel::pel_parser_t& pel_lang,
		parser::executive::global_gcmd_t*               global_gcmd,
		parser::executive::recursion_gcmd_t*            recursion_gcmd,
		parser::executive::repeat_gcmd_t*               repeat_gcmd,
		parser::executive::groups::global_gcmd_group_t* global_gcmd_group
	);

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

		parser::executive::parser_core_t* get_core() { return &parser_core; }

		bool is_render_code_in_console = false;
	
		groups::groups_t groups;

		parser::executive::parser_core_t parser_core;

		std::string version;

		std::size_t counter_autoblock = 0;

		void delete_alloc()
		{
			tree.delete_tree();
			words.delete_alloc();
			code_render.delete_alloc();

			std::clear(all_types);
			std::clear(all_tests);
			std::clear(all_groups);		
		}

		void process_parse_word()
		{
			parser::words_parser_t::process_parse_word(code, words);
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
			parser_core.delete_alloc();
			parser_core.code.clear();
			parser_core.delete_global_gcmd();
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

		// TODO:
		void get_ast()
		{

		}

		void push_text(const std::string& text) {

			auto obj = new obj_t;
			obj->name = text;
			all_tests.push_back(obj);
		}

		void run()
		{
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
							parser_core.code = sub_test.name;

							std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

							parser_core.process_parse_pel_to_words();

							std::intptr_t new_position = -1;
							std::vector<std::pair<std::string, std::string>> array_symbols;

							array_symbols.push_back({ "{", "}" });
							array_symbols.push_back({ "\"", "\"" });

							parser_core.block_parse(&parser_core.tree, 0, parser_core.words.words.size(), new_position, array_symbols, &parser_core.words);

							std::chrono::high_resolution_clock::time_point timer_parse_tree_start = std::chrono::high_resolution_clock::now();

							parser_core.parse_tree();

							std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();

							std::chrono::duration<double, std::micro> result = end - start;
							std::chrono::duration<double, std::micro> result_timer_parse_tre = end - timer_parse_tree_start;

							print(fg(fmt::color::azure), "\nProcess parse tree end: ");
							print(fg(fmt::color::coral), "{}", result_timer_parse_tre.count());
							print(fg(fmt::color::azure), "us\n");

							print(fg(fmt::color::azure), "\nProcess parse all steps end: ");
							print(fg(fmt::color::coral), "{}", result.count());
							print(fg(fmt::color::azure), "us\n");

							parser_core.delete_alloc();
						}
					}
				}
			}
			else
			{
				parser_core.group_init();

				pel::groups::group_result_t group_result;		
				pel::groups::array_words_t  array_words;
				std::vector<pel::groups::group_t*> result;

				//TODO: can we get information about total count groups?
				group_result.groups.reserve(32);
				array_words.data.reserve(32);
				result.reserve(32);

				bool last_glut = false, last_split = false;

				if (parser_core.is_render_group)
				{
					print(fg(fmt::color::green_yellow), "Debug groups to base symbols\n\n");
				}

				for (size_t i = 0; i < all_tests.size(); i++)
				{
					if (all_tests[i])
					{
						for (auto sub_test : all_tests[i]->values)
						{
							parser_core.code = sub_test.name;

							fmt::print("pel <- {}\nList signature: \n", parser_core.code);

							std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

							for (size_t w = 0; w < parser_core.code.length(); w++)
							{
								// copy iteration ? sound sad
								groups::group_element_t element(parser_core.code[w]);

								parser_core.group_parse(element, group_result);

								if (parser_core.is_render_group) {
									print(fg(fmt::color::thistle), "\"{0}\": ", (char) element.element);

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
										parser_core.process_executive_array_words(array_words, level);

										array_words.clear();

										// здесь начать разделение
										if (group_result.groups[0]->is_ignore)
										{
											// we ignore
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

													array_words.data[new_position].words.push_back(object);
												}
											}

											for (auto& it : result)
											{
												it->last_position += 1;
											}

											parser_core.process_executive_array_words(array_words, level);
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
												
												array_words.data[new_position].words.push_back(object);
											}																						
										}

										for (auto& it : result)
										{
											it->last_position += 1;
										}
									}
								}
															
								if (parser_core.is_render_group) {

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
							parser_core.process_executive_array_words(array_words, level);
							array_words.clear();

							std::chrono::high_resolution_clock::time_point timer_parse_tree_start = std::chrono::high_resolution_clock::now();
				
							std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();

							std::chrono::duration<double, std::micro> result_timer = end - start;
							std::chrono::duration<double, std::micro> result_timer_parse_tree = end - timer_parse_tree_start;

							print(fg(fmt::color::azure), "Process parse all steps end: ");
							print(fg(fmt::color::coral), "{}", result_timer.count());
							print(fg(fmt::color::azure), "us\n");
						
							parser_core.delete_alloc();
						}
					}					
				}
			}
		}

		// cpp prototype version 2
		bool compile()
		{
			// just parsing
			process_parse_word();

			pel::cpp_parser::cpp_parser_pel_t cpp_parser_pel;

			core_data_manager_t core_data_manager;

			core_data_manager.parser_core = get_core();

			core_data_manager.all_types   = &all_types;
			core_data_manager.all_tests   = &all_tests;
			core_data_manager.all_groups  = &all_groups;

			cpp_parser_pel.process_parse(&words, &core_data_manager);

			return build(*this, &parser_core.global_gcmd, &parser_core.recursion_gcmd, &parser_core.repeat_gcmd, &parser_core.global_gcmd_group);
		}
	};

	void get_property(parser::executive::gcmd_t* gcmd, const pel::obj_t* obj, const pel::obj_t *original_obj = nullptr, bool is_in_chain = false)
	{
		parser::executive::cmd_t* cmd		 = &gcmd->get_value();
		parser::executive::cmd_t* parent_cmd = &gcmd->parent->get_value();

		cmd->value = obj->name;

		bool is_or = false, is_xor = false, is_and = false;

		association_flags_for_cmd(*obj, cmd);

		if (is_in_chain && original_obj)
		{
			association_flags_for_cmd(*original_obj, cmd);
		}

		gcmd->flush_value();
	}

	struct names_list_t
	{
		bool is_ex = false;
		std::string name;
		std::vector<pel::obj_t*> data;
	};

	struct multinames_list_t
	{
		std::vector<names_list_t> data;

		void push(pel::obj_t* obj) {

			bool is_find = false;
			std::size_t i = 0;

			names_list_t* nl = nullptr;

			for (const auto& it : data)
			{
				if (it.name == obj->name)
				{
					nl = &data[i];
					is_find = true;
					break;
				}

				i++;
			}

			if (is_find && nl)
			{
				is_find = false;

				for (const auto& it : nl->data)
				{
					if (it == obj)
					{
						is_find = true;
						break;
					}
				}

				if (!is_find)
				{
					if (obj->is_flag(obj_flag_t::obj_execute))
						nl->is_ex = true;

					nl->data.push_back(obj);
				}
			}
			else
			{
				names_list_t new_nl;
				new_nl.name = obj->name;
				new_nl.data.push_back(obj);

				if (obj->is_flag(obj_flag_t::obj_execute))
					new_nl.is_ex = true;

				data.push_back(new_nl);
			}
		}
	};

	void multi_name(pel::pel_parser_t& pel_lang, names_list_t* names_list, parser::executive::gcmd_t* main, unsigned& counter_tmp_or, multinames_list_t* multinames_list);

	void no_ex(
		pel::pel_parser_t& pel_lang,
		parser::executive::gcmd_t* parent,
		const words_base_t& word,
		int&  level,
		bool& is_stop,
		const obj_t &original_obj, // это объект очереди и его свойства не копируются
		multinames_list_t *multinames_list,
		unsigned& counter_tmp_or
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

		if (std::check_flag(original_obj.flag, obj_flag_t::obj_auto))
		{
			is_find = true;
			get_property(parent, &original_obj);

			for (const auto& sub_obj : original_obj.values)
			{
				parser::executive::gcmd_t* gcmd = parent->push({});

				if (std::check_flag(sub_obj.flag, obj_flag_t::obj_type))
				{
					no_ex(pel_lang, gcmd, sub_obj.word, level, is_stop, sub_obj, multinames_list, counter_tmp_or);

					level--;
				}
				else
				{
					get_property(gcmd, &sub_obj, &original_obj);
				}

			}
		}
		else
		{
			for (auto& list_mutinames : multinames_list->data)
			{
				if (list_mutinames.name == word.data)
				{
					if (list_mutinames.data.size() == 1)
					{
						is_find = true;

						if (original_obj.name == word.data)
						{
							get_property(parent, list_mutinames.data[0], &original_obj, true);
						}
						else
						{
							get_property(parent, list_mutinames.data[0]);
						}
					
						for (const auto& sub_obj : list_mutinames.data[0]->values)
						{
							parser::executive::gcmd_t* gcmd = parent->push({});

							if (std::check_flag(sub_obj.flag, obj_flag_t::obj_type) && !std::check_flag(sub_obj.flag, obj_flag_t::obj_recursion))
							{
								no_ex(pel_lang, gcmd, sub_obj.word, level, is_stop, sub_obj, multinames_list, counter_tmp_or);

								level--;
							}
							else
							{
								get_property(gcmd, &sub_obj, &original_obj);
							}

						}

						break;
					}
					else
					{
						is_find = true;
						multi_name(pel_lang, &list_mutinames, parent, counter_tmp_or, multinames_list);
						break;
					}

				}
			}
		}

		// find group
		if (!is_find)
		{	
			is_find = false;

			for (auto &it : pel_lang.all_groups)
			{
				if (it->name == word.data)
				{
					get_property(parent, it);
						
					for (size_t i = 0; i < pel_lang.parser_core.global_gcmd_group.size(); i++)
					{
						if (pel_lang.parser_core.global_gcmd_group[i].gcmd->get_value().group.name == word.data)
						{
							parent->get_value().group = &pel_lang.parser_core.global_gcmd_group[i].gcmd->get_value().group;
							is_find = true;
							break;
						}
					}

					if (is_find)
						break;
				}
			}
			
		}

		if (!is_find)
		{
			pel_lang.error_context.push(format("Type or group: \"{}\" is not declared!", word.data), "", word.number_line, word.start_position, word.end_position);
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
			pel_lang.error_context.push(format("Type: \"{}\" has no way out of recursion!", word.data), "", word.number_line, word.start_position, word.end_position);

			is_stop = true;
			return;
		}

		for (const auto &obj : pel_lang.all_groups)
		{
			if (std::check_flag(obj->flag, obj_flag_t::obj_type) && obj->name == word.data)
			{
				is_find = true;

				parser::executive::groups::get_property(parent, obj);

				/*
				   Копирование свойств в цепочке
				*/

				if (std::check_flag(original_obj.flag, obj_flag_t::obj_not))
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

					if (std::check_flag(sub_obj.flag, obj_flag_t::obj_type))
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

	void multi_name(pel::pel_parser_t& pel_lang, names_list_t* names_list, parser::executive::gcmd_t* main, unsigned &counter_tmp_or, multinames_list_t* multinames_list)
	{
		auto cmd_main = &main->get_value();

		cmd_main->value = names_list->name;

		std::add_flag(cmd_main->flag, parser::executive::quantum_or);
		//std::add_flag(cmd_main->flag, parser::executive::quantum_ex); // ????? TODO:
		std::add_flag(cmd_main->flag, parser::executive::quantum_type);

		for (auto &it : names_list->data)
		{
			parser::executive::gcmd_t* tmp_or = main->push({});

			auto cmd = &tmp_or->get_value();

			get_property(tmp_or, it);

			cmd->value = format("or{}", counter_tmp_or);
			counter_tmp_or++;

			for (auto main_cmd : it->values)
			{
				parser::executive::gcmd_t* sub_main = tmp_or->push({});

				int  level = 0;
				bool is_stop = false;
			
				if (std::check_flag(main_cmd.flag, obj_flag_t::obj_type))
				{
					no_ex(pel_lang, sub_main, main_cmd.word, level, is_stop, main_cmd, multinames_list, counter_tmp_or);
				}
				else
				{
					get_property(sub_main, &main_cmd);
				}
			}
		}

		main->flush_value();
	}

	void multi_names_array(pel::pel_parser_t& pel_lang, parser::executive::global_gcmd_t* global_gcmd, parser::executive::groups::global_gcmd_group_t* global_gcmd_group, multinames_list_t *multinames_list)
	{
		for (const auto &obj : pel_lang.all_types)
		{
			multinames_list->push(obj);

			for (const auto &it : pel_lang.all_types)
			{
				if (it == obj)
					continue;

				if (obj->name == it->name)
				{
					multinames_list->push(it);
				}
			}
		}
	}

	bool build(
		pel::pel_parser_t& pel_lang, 
		parser::executive::global_gcmd_t    *global_gcmd,
		parser::executive::recursion_gcmd_t *recursion_gcmd, 
		parser::executive::repeat_gcmd_t    *repeat_gcmd,
		parser::executive::groups::global_gcmd_group_t *global_gcmd_group
	)
	{
		if (!global_gcmd || !recursion_gcmd || !repeat_gcmd || !global_gcmd_group)
			return false;

		for (const auto obj_ex : pel_lang.all_groups)
		{
			if (obj_ex) {
				if (std::check_flag(obj_ex->flag, obj_flag_t::obj_execute))
				{
					auto main = new parser::executive::groups::gcmd_group_t;

					parser::executive::groups::get_property(main, obj_ex);

					for (auto main_cmd : obj_ex->values)
					{
						auto sub_main = main->push({});

						int  level = 0;
						bool is_stop = false;

						if (std::check_flag(main_cmd.flag, obj_flag_t::obj_type))
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

		multinames_list_t multinames_list;
		multi_names_array(pel_lang, global_gcmd, global_gcmd_group, &multinames_list);

		unsigned counter_tmp_or = 0;

		for (auto &it : multinames_list.data)
		{
			if (it.data.size() == 1)
			{
				// solo type
				if (std::check_flag(it.data[0]->flag, obj_flag_t::obj_execute))
				{
					parser::executive::gcmd_t* main = new parser::executive::gcmd_t;

					get_property(main, it.data[0]);

					for (auto main_cmd : it.data[0]->values)
					{
						parser::executive::gcmd_t* sub_main = main->push({});

						int  level = 0;
						bool is_stop = false;

						if (std::check_flag(main_cmd.flag, obj_flag_t::obj_type) && !std::check_flag(main_cmd.flag, obj_flag_t::obj_recursion))
						{
							no_ex(pel_lang, sub_main, main_cmd.word, level, is_stop, main_cmd, &multinames_list, counter_tmp_or);
						}
						else
						{
							get_property(sub_main, &main_cmd);
						}
					}

					// TODO: check status error
					global_gcmd->push_back(main);
				}			
			}
			else
			{
				if (it.is_ex) 
				{
					parser::executive::gcmd_t* main = new parser::executive::gcmd_t;
					multi_name(pel_lang, &it, main, counter_tmp_or, &multinames_list);
					global_gcmd->push_back(main);
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
			parser::executive::make_commands(global_gcmd, recursion_gcmd, repeat_gcmd, pel_lang.parser_core.is_render_tree);
			parser::executive::groups::make_commands(global_gcmd_group, pel_lang.parser_core.is_render_tree && pel_lang.parser_core.is_render_group);
		}
		else
		{
			return false;
		}

		return true;
	}
}