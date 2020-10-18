#pragma once

#include "..\..\..\..\..\code\parser\executive\cmd\main_logic\cmd.hpp"

#define show_logs   if (is_render_tree)
#define show_result if (is_render_tree)
#define show_tree   if (is_render_tree)

namespace parser
{
	namespace executive
	{
		void print_space_cmd(const std::size_t& count, bool is_elements, cmd_t* cmd)
		{
			if (is_elements && count == 0)
			{
				fmt::print(fg(fmt::color::coral), "{}", (char)254);
				return;
			}

			std::size_t tmp_count = count;

			for (size_t i = 0; i < tmp_count; i++)
			{
				fmt::print("{}", (char)221);
			}

			if (is_elements)
			{
				if (cmd->is_type()) {
					print(fg(fmt::color::aquamarine), "{}", (char)254); 
				}
				else
				{
					print(fg(fmt::color::deep_sky_blue), "{}", (char)254);
				}
			}
			else
			{
				if (cmd->is_type()) {
					print(fg(fmt::color::aquamarine), "{}", (char)254);
				}
				else
				{
					print(fg(fmt::color::deep_sky_blue), "{}", (char)254);
				}
			}
		}

		bool is_last_graph(gcmd_t* current_graph, gcmd_t* child_graph, gcmd_t* final_graph)
		{
			if (current_graph->is_root)
				return false; // ?

			if (child_graph)
			{
				if (child_graph->is_root)
					return true;
			}

			if (current_graph == final_graph)
			{
				if (child_graph)
				{
					if (child_graph == final_graph)
						return true;

					if (child_graph->is_last())
						return true;

					return false;
				}
				else
				{
					return false;
				}
			}

			return is_last_graph(current_graph->parent, current_graph, final_graph);
		}

		/*
			функция завершающего обхода вершины графа. Когда последний элемент дочерней вершины графа завершается, то
			её вершина-родитель вызывается, где обрабатывается её позиции. Данный подход является чем-то похожим на вертикальный перебор
			графа, но к сожалению нам требуется имено рекурсивный перебор, где по последнему вызывается родитель, несмотря на то, что родитель
			уже вызываелся при рекурсивном переборе.

			В данной функции происходит расчет позиций вершины, решающий задачу OR и AND для вершин значения/типов через счетчик. Что насчет лицензии на этот алгоритм? Я его выводил очень долго :D
		*/
		void calc_position_in_graph_for_parrent(gcmd_t* command_graph, gcmd_t* first_child_graph, gcmd_t* last_child_graph, std::size_t& count_base_signature, bool is_render_tree)
		{
			cmd_t* cmd		   = &command_graph->get_value();
			cmd_t* parrent_cmd = &command_graph->parent->get_value();
			 
			if (cmd->is_type())
			{
				if (first_child_graph->get_value().min_position < cmd->min_position)
					cmd->min_position = first_child_graph->get_value().min_position;

				std::size_t max_position = 0;

				if (cmd->is_or())
					cmd->min_counter = SIZE_MAX;

				for (std::size_t i = 0; i < command_graph->tree.size(); i++)
				{
					if (command_graph->tree[i]->get_value().max_position > max_position)
						max_position = command_graph->tree[i]->get_value().max_position;

					if (cmd->is_or()) {

						if (command_graph->tree[i]->get_value().min_counter < cmd->min_counter)
						{
							cmd->min_counter = command_graph->tree[i]->get_value().min_counter;
						}

						if (command_graph->tree[i]->get_value().max_counter > cmd->max_counter)
						{
							cmd->max_counter = command_graph->tree[i]->get_value().max_counter;
						}
					}
				}

				if (max_position > cmd->max_position)
					cmd->max_position = max_position;
			}

			 if (cmd->is_type())
			 {
				 if (!parrent_cmd->is_or()) {

					 if (cmd->is_or()) {
						 parrent_cmd->max_counter = cmd->max_counter;
						 parrent_cmd->min_counter = cmd->min_counter;
					 }

					 if (cmd->is_and() || cmd->is_empty_operation()) {			
						 parrent_cmd->max_counter = cmd->max_counter;
						 parrent_cmd->min_counter = cmd->min_counter;	 
					 }
				 }
			 }
		}

		void calc_position_in_graph(gcmd_t* command_graph, std::size_t& count_base_signature, bool is_render_tree)
		{
			cmd_t* cmd		   = &command_graph->get_value();
			cmd_t* parrent_cmd = &command_graph->parent->get_value();

			if (command_graph->is_root)
			{
				command_graph->get_value().min_position = 0;
			}

			if (cmd->is_type())
			{
				cmd->min_counter = parrent_cmd->min_counter;
				cmd->max_counter = parrent_cmd->max_counter;

				// empty vertex graph
				if (command_graph->size() == 0)
				{
					// for render graph
					cmd->min_position = parrent_cmd->min_counter;
					cmd->max_position = parrent_cmd->max_counter;
				}
			} 

			if (cmd->is_group() || cmd->is_value())
			{
				cmd->min_position = parrent_cmd->min_counter;
				cmd->max_position = parrent_cmd->max_counter;

				cmd->min_counter = parrent_cmd->min_counter;
				cmd->max_counter = parrent_cmd->max_counter;
	
				if (!parrent_cmd->is_or()) {
					parrent_cmd->min_counter++;
					parrent_cmd->max_counter++;
				}

				if (parrent_cmd->is_or()) {
					cmd->min_counter++;
					cmd->max_counter++;
				}
			}
		}

		void recursion_tree_traversals_positions(gcmd_t* command_graph, std::size_t level)
		{
			if (!command_graph->root->is_process)
				return;

			if (!command_graph)
				return;

			command_graph->level = level;

			std::size_t need_remove = 0;

			if (command_graph->is_value) 
				calc_position_in_graph(command_graph, need_remove, false);
			
			for (size_t i = 0; i < command_graph->tree.size(); i++)
			{
				if (command_graph->tree[i])
				{
					level++;

					recursion_tree_traversals_positions(command_graph->tree[i], level);

					if (command_graph->is_value)
					{
						if (command_graph->tree[i]->is_last())
						{
							calc_position_in_graph_for_parrent(command_graph, command_graph->tree[0], command_graph->tree[i], need_remove, false);
						
						}
					}

					level--;
				}
			}
		}

		void recalc_position_in_graph(gcmd_t* command_graph)
		{
			recursion_tree_traversals_positions(command_graph, 0);
		}

		void get_position_from_parent_to_root(gcmd_t* command_graph, std::size_t& position)
		{
			if (command_graph->is_root) {
				position = 0;
				return;
			}

			if (command_graph->parent->is_root)
				position = command_graph->position;
			else
				return get_position_from_parent_to_root(command_graph->parent, position);
		}

		void recalc_position_in_graph_from_position(gcmd_t* command_graph, const std::size_t &position)
		{
			if (!command_graph->root->is_process)
				return;

			if (!command_graph)
				return;

			std::size_t level = command_graph->level;

			std::size_t need_remove = 0;

		//	if (command_graph->is_value)
		//		calc_position_in_graph(command_graph, need_remove, false);

			for (size_t i = position; i < command_graph->tree.size(); i++)
			{
				if (command_graph->tree[i])
				{
					level++;

					recursion_tree_traversals_positions(command_graph->tree[i], level);

					if (command_graph->is_value)
					{
						if (command_graph->tree[i]->is_last())
						{
							calc_position_in_graph_for_parrent(command_graph, command_graph->tree[0], command_graph->tree[i], need_remove, false);
						}
					}

					level--;
				}
			}
		}

		void print_graph_gcmd(gcmd_t* command_graph, std::size_t& count_base_signature, bool is_render_tree)
		{
			cmd_t* cmd = &command_graph->get_value();
			cmd_t* parrent_cmd = &command_graph->parent->get_value();

			show_tree print_space_cmd(command_graph->level, command_graph->is_have_sub_elemets(), cmd);
		
			bool is_or = false;
			bool is_and = false;
			bool is_xor = false;
			bool is_not = false;
			bool is_value = false;
			bool is_position = false;

			if (!cmd->value.empty())
			{
				if (command_graph->is_root)
				{
					show_tree fmt::print(fg(fmt::color::coral), " {}", cmd->value);
				}
				else
				{
					if (cmd->is_type())
					{
						show_tree fmt::print(fg(fmt::color::blanched_almond), " {}", cmd->value);
					}

					if (cmd->is_value())
					{
						show_tree fmt::print(fg(fmt::color::thistle), " \"{}\"", cmd->value);
					}

					if (cmd->is_group())
					{
						show_tree fmt::print(fg(fmt::color::khaki), " {}", cmd->value);
					}
				}
			}

			if (cmd->is_group())
			{
				is_position = true;
				is_value = true;


				show_tree fmt::print(fg(fmt::color::deep_sky_blue), " [group]");

			}

			if (cmd->is_value())
			{
				is_position = true;
				is_value    = true;

				show_tree fmt::print(fg(fmt::color::deep_sky_blue), " [value]");					
			}

			if (cmd->is_or()) {

				is_or = true;
				is_position = true;

				show_tree  fmt::print(" [or]");
			}

			if (cmd->is_and()) {

				is_and = true;
				is_position = true;

				show_tree  fmt::print(" [and]");
			}

			if (cmd->is_empty_operation()) {

				is_and = true;
				is_position = true;

				show_tree  fmt::print(" [empty_operation]");
			}

			if (cmd->is_recursion()) {

				show_tree  fmt::print(fg(fmt::color::hot_pink), " [recursion]");
			}


			if (cmd->is_maybe()) {

				show_tree  fmt::print(fg(fmt::color::bisque), " [maybe]");
			}

			if (cmd->is_exit()) {

				show_tree  fmt::print(fg(fmt::color::bisque), " [exit]");
			}

			if (cmd->is_return()) {

				show_tree  fmt::print(fg(fmt::color::bisque), " [return]");
			}

			if (cmd->is_xor()) {

				is_xor = true;
				is_position = true;

				show_tree  fmt::print(" [xor]");
			}

			if (cmd->is_not()) {

				is_not = true;
				is_position = true;

				show_tree  fmt::print(" [not]");
			}

			if (cmd->is_type()) {

				is_position = true;

				show_tree fmt::print(fg(fmt::color::aquamarine), " [type]");
			}

			if (cmd->is_execute())
				show_tree fmt::print(fg(fmt::color::red), " [execute]");

			show_tree fmt::print(" [");
			show_tree fmt::print(fg(fmt::color::coral), "{0}", cmd->min_position);
			show_tree fmt::print("-");
			show_tree fmt::print(fg(fmt::color::coral), "{0}", cmd->max_position);
			show_tree fmt::print("]");
			show_tree fmt::print("\n");
		}

		void print_graph(gcmd_t* command_graph)
		{
			if (!command_graph->root->is_process)
				return;

			if (!command_graph)
				return;

			std::size_t need_remove = 0;

			if (command_graph->is_value)
				print_graph_gcmd(command_graph, need_remove, true);

			for (size_t i = 0; i < command_graph->tree.size(); i++)
			{
				if (command_graph->tree[i])
				{
					print_graph(command_graph->tree[i]);
				}
			}
		}

		struct data_block_global_gcmd_t
		{
			bool is_use = true;
		};

		struct element_gcmd_t
		{
			element_gcmd_t() {}
			element_gcmd_t(gcmd_t* g) { gcmd = g; }

			gcmd_t* gcmd = nullptr;
			//bool    is_use = true;
			std::size_t     count_signaturs = 0;

			block_depth_t<data_block_global_gcmd_t> block_depth;
		};

		using global_gcmd_t    = std::vector<element_gcmd_t>;
		using recursion_gcmd_t = std::vector<element_gcmd_t>;
		
		void copy_process_gcmd(const gcmd_t* gcmd, gcmd_t* data)
		{
			data->set_value(gcmd->get_value());

			for (const auto& tree_element : gcmd->tree)
			{
				gcmd_t* sub_data = data->push(tree_element->get_value());

				if (tree_element->size() > 0)
					copy_process_gcmd(tree_element, sub_data);
			}
		}

		void copy_global_cmd(global_gcmd_t* from_gcmd, global_gcmd_t* to_gcmd)
		{
			if (to_gcmd && from_gcmd)
			{
				for (auto& it : *to_gcmd)
				{
					it.gcmd->delete_tree();
					delete it.gcmd;

					it.gcmd = nullptr;
				}

				for (auto& it : *from_gcmd)
				{
					gcmd_t* data = new gcmd_t;

					data->set_value(it.gcmd->get_value());

					for (auto& tree_element : it.gcmd->tree)
					{
						gcmd_t* sub_data = data->push(tree_element->get_value());

						if (tree_element->size() > 0)
							copy_process_gcmd(tree_element, sub_data);
					}

					element_gcmd_t element_gcmd;

					element_gcmd.count_signaturs = it.count_signaturs;
					element_gcmd.gcmd = data;

					to_gcmd->push_back(element_gcmd);
				}
			}
		}

		gcmd_t* find_vertex_recursion(gcmd_t* command_graph, const std::string &name) {

			cmd_t* cmd = &command_graph->get_value();

			if (cmd->value == name)
			{
				return command_graph;
			}

			if (command_graph->is_root)
				return nullptr;

			return find_vertex_recursion(command_graph->parent, name);
		}

		void calc_recursion(gcmd_t* command_graph, recursion_gcmd_t* &recursion_gcmd, std::size_t& count_base_signature, bool is_render_tree)
		{
			cmd_t* cmd = &command_graph->get_value();
			cmd_t* parrent_cmd = &command_graph->parent->get_value();

			if (cmd->is_recursion())
			{
				element_gcmd_t recursion_element;

				recursion_element.gcmd = new gcmd_t;

				gcmd_t* graph = find_vertex_recursion(command_graph->parent, cmd->value);
				
				if (graph) {
					copy_process_gcmd(graph, recursion_element.gcmd);

					recursion_gcmd->push_back(recursion_element);

					cmd->recursion_element = recursion_element.gcmd;
					recursion_element.gcmd->get_value().recursion_element = recursion_element.gcmd;
				}
				else {
					// TODO: Critical error!
				}
			}	
		}

		void calc_recursion_for_recursion_gcmd(gcmd_t* command_graph, recursion_gcmd_t*& recursion_gcmd, std::size_t& count_base_signature, bool is_render_tree)
		{
			cmd_t* cmd = &command_graph->get_value();
			cmd_t* parrent_cmd = &command_graph->parent->get_value();

			if (cmd->is_recursion())
			{
				if (!cmd->recursion_element)
				{
					for (const auto &it : *recursion_gcmd)
					{					
						if (it.gcmd->get_value().value == cmd->value)
						{
								cmd->recursion_element = it.gcmd;
								break;
						}					
					}
				}		
			}
		}

		void find_last(gcmd_t* command_graph, bool is_render_tree)
		{
			cmd_t* cmd = &command_graph->get_value();

			if (command_graph->size() == 0)
			{
				//show_logs fmt::print("Last element: {}\n", cmd->value);
				cmd->is_last = true;
				return;
			}
			else
			{
				if (cmd->is_or() || cmd->is_xor())
				{
					cmd->is_last = true;
					return;
				}
				else
				{
					find_last(command_graph->tree.back(), is_render_tree);
				}

				return;
			}

			return;
		}


		void make_commands(global_gcmd_t* global_gcmd, recursion_gcmd_t *recursion_gcmd, bool is_render_tree)
		{		
			for (auto& it : *global_gcmd)
			{
				it.gcmd->is_process = true;

				recalc_position_in_graph(it.gcmd);

				it.gcmd->stop_process();

				/*it.gcmd->process_function["base"]         = detail::bind_function(&calc_position_in_graph, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
				it.gcmd->process_function["last_parrent"] = detail::bind_function(&calc_position_in_graph_for_parrent, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);

				it.gcmd->start_process(it.count_signaturs, is_render_tree);

				it.gcmd->process_function.function_list.clear();*/

				it.gcmd->process_function["base"] = detail::bind_function(&print_graph_gcmd, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
				it.gcmd->start_process(it.count_signaturs, is_render_tree);

				it.gcmd->process_function.function_list.clear();

				gcmd_t* tree = nullptr;

				if (it.gcmd->tree.size() > 0)
					tree = it.gcmd->tree.back();

				if (tree)
				{
					find_last(tree, is_render_tree);
				}
				else
				{
					// root last element
					it.gcmd->get_value().is_last = true;
				}

				it.gcmd->process_function["base"] = detail::bind_function(&calc_recursion, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
				it.gcmd->start_process(recursion_gcmd, it.count_signaturs, is_render_tree);

				it.gcmd->process_function.function_list.clear();
			}

			for (auto& it : *recursion_gcmd)
			{
				it.gcmd->process_function["base"] = detail::bind_function(&calc_recursion_for_recursion_gcmd, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
				it.gcmd->start_process(recursion_gcmd, it.count_signaturs, is_render_tree);

				it.gcmd->process_function.function_list.clear();
			}
		}
	}
}