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
		void calc_position_in_graph_for_parent(gcmd_t* command_graph, gcmd_t* first_child_graph, gcmd_t* last_child_graph, std::size_t& count_base_signature, bool is_render_tree)
		{
			cmd_t* cmd		   = &command_graph->get_value();
			cmd_t* parent_cmd  = &command_graph->parent->get_value();
			 
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
				 if (!parent_cmd->is_or()) {

					 if (cmd->is_or()) {
						 parent_cmd->max_counter = cmd->max_counter;
						 parent_cmd->min_counter = cmd->min_counter;
					 }

					 if (cmd->is_and() || cmd->is_empty_operation()) {			
						 parent_cmd->max_counter = cmd->max_counter;
						 parent_cmd->min_counter = cmd->min_counter;
					 }
				 }
			 }
		}

		void calc_position_in_graph(gcmd_t* command_graph, std::size_t& count_base_signature, bool is_render_tree)
		{
			cmd_t* cmd		   = &command_graph->get_value();
			cmd_t* parent_cmd  = &command_graph->parent->get_value();

			if (command_graph->is_root)
			{
				command_graph->get_value().min_position = 0;
			}

			if (cmd->is_type())
			{
				cmd->min_counter = parent_cmd->min_counter;
				cmd->max_counter = parent_cmd->max_counter;

				// empty vertex graph
				if (command_graph->size() == 0)
				{
					// for render graph
					cmd->min_position = parent_cmd->min_counter;
					cmd->max_position = parent_cmd->max_counter;
				}
			} 

			if (cmd->is_group() || cmd->is_value())
			{
				cmd->min_position = parent_cmd->min_counter;
				cmd->max_position = parent_cmd->max_counter;

				cmd->min_counter  = parent_cmd->min_counter;
				cmd->max_counter  = parent_cmd->max_counter;
	
				if (!parent_cmd->is_or()) {
					parent_cmd->min_counter++;
					parent_cmd->max_counter++;
				}

				//if (parent_cmd->is_or()) {
					cmd->min_counter++;
					cmd->max_counter++;
				//}
			}
		}

		void print_graph_gcmd(gcmd_t* command_graph, std::size_t& count_base_signature, bool is_render_tree)
		{
			cmd_t* cmd = &command_graph->get_value();
			cmd_t* parent_cmd = &command_graph->parent->get_value();

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
			std::size_t     count_signatures = 0;

			block_depth_t<data_block_global_gcmd_t> block_depth;
		};

		using global_gcmd_t    = std::vector<element_gcmd_t>;
		using recursion_gcmd_t = std::vector<element_gcmd_t>;
		using repeat_gcmd_t    = std::vector<element_gcmd_t>;
	}
}