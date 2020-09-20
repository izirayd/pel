#pragma once

#include "..\..\..\..\..\code\parser\executive\cmd\main_logic\cmd.hpp"

#define show_logs   if (is_render_tree)
#define show_result if (is_render_tree)
#define show_tree   if (is_render_tree)

namespace parser
{
	namespace executive
	{
		// TODO: Половина функций уже не актуальна, удалить?
		inline void find_or(const gcmd_t* command_graph, bool& is_result)
		{
			if (std::check_flag(command_graph->get_value().flag, parser_or))
				is_result = true;

			if (command_graph->is_root)
				return;

			find_or(command_graph->parent, is_result);
		}

		gcmd_t *get_up_or(gcmd_t* command_graph, bool& is_result)
		{
			if (command_graph->is_root)
			{
				if (command_graph->get_value().is_or())
				{
					is_result = true;
					return command_graph;
				}
				else
				{
					is_result = false;
					return nullptr;
				}
			}

			if (command_graph->get_value().is_or()) 
			{
				if (command_graph->parent->get_value().is_tree_or())
				{
					return get_up_or(command_graph->parent, is_result);
				}
				else
				{
					is_result = true;
					return command_graph;
				}
			}

			return get_up_or(command_graph->parent, is_result);
		}

		// Вернет первую вершину or, игнорируя любую or вершину
		gcmd_t* get_first_or(gcmd_t* command_graph, bool& is_result)
		{
			if (command_graph->is_root)
			{
				if (command_graph->get_value().is_or())
				{
					is_result = true;
					return command_graph;
				}
				else
				{
					is_result = false;
					return nullptr;
				}
			}

			if (command_graph->get_value().is_or())
			{
				is_result = true;
				return command_graph;
			}

			return get_first_or(command_graph->parent, is_result);
		}

		gcmd_t* get_and_child_or(gcmd_t* command_graph, gcmd_t* main_or_grahp, bool& is_result)
		{
			if (command_graph->is_root)
			{
				return command_graph;
			}

			if (command_graph->get_value().is_and())
			{
				if (command_graph->get_value().is_and() && command_graph->parent == main_or_grahp)
				{
					return command_graph;
				}
				else
				{
					return get_and_child_or(command_graph->parent, main_or_grahp, is_result);
				}
			}

			// for or
			return get_and_child_or(command_graph->parent, main_or_grahp, is_result);
		}

		inline void find_xor(const gcmd_t* command_graph, bool& is_result)
		{
			if (std::check_flag(command_graph->get_value().flag, parser_xor))
				is_result = true;

			if (command_graph->is_root)
				return;

			find_xor(command_graph->parent, is_result);
		}

	

		inline void set_max_position(gcmd_t* command_graph, const std::size_t& max_position)
		{
			command_graph->get_value().max_position = max_position;
			//command_graph->get_value().current_position = max_position;

			if (command_graph->is_root) 
				return;
			
			set_max_position(command_graph->parent, max_position);
		}

		inline void set_min_position(gcmd_t* command_graph, const std::size_t& min_position)
		{
			command_graph->get_value().min_position = min_position;
			//command_graph->get_value().current_position = min_position;

			if (command_graph->is_root)
				return;

			set_min_position(command_graph->parent, min_position);
		}

		inline void set_min_position_ignore_root(gcmd_t* command_graph, const std::size_t& min_position)
		{
			if (command_graph->is_root)
				return;

			command_graph->get_value().min_position = min_position;
			//command_graph->get_value().current_position = min_position;

			set_min_position_ignore_root(command_graph->parent, min_position);
		}

		inline void set_min_position_ignore_root_with_check_min(gcmd_t* command_graph, const std::size_t& min_position)
		{
			if (command_graph->is_root)
				return;

			command_graph->get_value().min_position = min_position;

			if (command_graph->get_value().min_position < min_position)
			{
				command_graph->get_value().min_position = min_position;
			//	command_graph->get_value().current_position = min_position;
			}

			set_min_position_ignore_root(command_graph->parent, min_position);
		}


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

		void set_max_position_for_graph(gcmd_t* command_graph, const std::size_t& max_position)
		{
			command_graph->get_value().max_position = max_position;
		}

		void set_min_max_position_for_graph(gcmd_t* command_graph, const std::size_t& max_position, const std::size_t& min_position)
		{
			command_graph->get_value().max_position = max_position;
			command_graph->get_value().min_position = min_position;
		}

		void sub_min_position_change(gcmd_t* command_graph, const std::size_t& min_position)
		{
			if (command_graph->is_root)
				return;

			if (command_graph->get_value().min_position > min_position)
				command_graph->get_value().min_position = min_position;

			sub_min_position_change(command_graph->parent, min_position);
		}

		void min_position_change(gcmd_t* command_graph)
		{
			if (command_graph->is_root)
				return;

			std::size_t min_value = SIZE_MAX;

			//if (command_graph->get_value().min_position == 0)
			{
				for (size_t i = 0; i < command_graph->tree.size(); i++)
				{
					if (command_graph->tree[i]->get_value().min_position < min_value)
						min_value = command_graph->tree[i]->get_value().min_position;
				}

				if (min_value != SIZE_MAX) {
					command_graph->get_value().min_position = min_value;
				}
			}

			sub_min_position_change(command_graph->parent, min_value);
		}

		void print_global_cmd(gcmd_t* command_graph, std::size_t& count_base_signature, bool);

		void min_position_change_all_or_iteration(gcmd_t* command_graph, std::size_t &min_value)
		{
			{
				for (size_t i = 0; i < command_graph->tree.size(); i++)
				{
					if (command_graph->tree[i]->get_value().min_position < min_value)
						min_value = command_graph->tree[i]->get_value().min_position;
				}

				if (min_value != SIZE_MAX)
				{
					for (size_t i = 0; i < command_graph->tree.size(); i++)
					{
						command_graph->tree[i]->get_value().min_position = min_value;
					}

					command_graph->get_value().min_position = min_value;
				}
			}
		}

		void min_position_change_all(gcmd_t* command_graph)
		{
			if (command_graph->is_root)
				return;

			std::size_t min_value = SIZE_MAX;

			{
				for (size_t i = 0; i < command_graph->tree.size(); i++)
				{
					if (command_graph->tree[i]->get_value().min_position < min_value)
						min_value = command_graph->tree[i]->get_value().min_position;
				}

				if (min_value != SIZE_MAX) {

					for (size_t i = 0; i < command_graph->tree.size(); i++)
					{
						command_graph->tree[i]->get_value().min_position = min_value;
					}

					command_graph->get_value().min_position = min_value;
				}
			}

			sub_min_position_change(command_graph->parent, min_value);
		}

		void sub_max_position_change(gcmd_t* command_graph, const std::size_t& max_position)
		{
			if (command_graph->is_root)
				return;

			if (command_graph->get_value().max_position < max_position)
				command_graph->get_value().max_position = max_position;

			sub_max_position_change(command_graph->parent, max_position);
		}

		void max_position_change(gcmd_t* command_graph)
		{
			if (command_graph->is_root)
				return;

			std::size_t max_value = 0;

			{
				for (size_t i = 0; i < command_graph->tree.size(); i++)
				{
					if (command_graph->tree[i]->get_value().max_position > max_value)
						max_value = command_graph->tree[i]->get_value().max_position;
				}

				command_graph->get_value().max_position = max_value;
			}

			sub_max_position_change(command_graph->parent, max_value);
		}

		void max_position_change_all(gcmd_t* command_graph)
		{
			if (command_graph->is_root)
				return;

			std::size_t max_value = 0;

			{
				for (size_t i = 0; i < command_graph->tree.size(); i++)
				{
					if (command_graph->tree[i]->get_value().max_position > max_value)
						max_value = command_graph->tree[i]->get_value().max_position;
				}

				for (size_t i = 0; i < command_graph->tree.size(); i++)
				{	
					command_graph->tree[i]->get_value().max_position = max_value;
				}

				command_graph->get_value().max_position = max_value;

			}

			sub_max_position_change(command_graph->parent, max_value);
		}

		void set_min_position_for_graph_with_check_min(gcmd_t* command_graph, const std::size_t& min_position)
		{
			if (command_graph->get_value().min_position == 0)
			{
				command_graph->get_value().min_position = min_position;
				return;
			}

			if (command_graph->get_value().min_position > min_position)
			{
				command_graph->get_value().min_position = min_position;
			}
		}

		// TODO : Вынести в свой собственный контекст, для мультипоточности
		// Не актуально, уничтожить
		static std::size_t last_min = 0;
		static std::size_t last_max = 0;
		static std::size_t next_and_position = 0;
		static std::size_t global_min_position  = 0;
		static std::size_t global_max_position  = 0;
		static std::size_t dynamic_max  = 0;
		static std::size_t dynamic_min  = 0;
		static std::size_t count_or  = 0;

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
		void last_global_cmd(gcmd_t* command_graph, gcmd_t* first_child_graph, gcmd_t* last_child_graph, std::size_t& count_base_signature, bool is_render_tree)
		{
			cmd_t* cmd		   = &command_graph->get_value();
			cmd_t* parrent_cmd = &command_graph->parent->get_value();

			if (cmd->is_type())
			{
				if (first_child_graph->get_value().min_position < cmd->min_position)
					cmd->min_position = first_child_graph->get_value().min_position;

				std::size_t max_position = 0;

				for (std::size_t i = 0; i < command_graph->tree.size(); i++)
				{
					if (command_graph->tree[i]->get_value().max_position > max_position)
						max_position = command_graph->tree[i]->get_value().max_position;
				}

				if (max_position > cmd->max_position)
					cmd->max_position = max_position;
			}

			 if (cmd->is_type())
			 {
				 if (!parrent_cmd->is_or()) {

					 if (cmd->is_or()) {
						 parrent_cmd->max_counter = cmd->max_position + 1;
						 parrent_cmd->min_counter = cmd->min_position + 1;
					 }

					 if (cmd->is_and() || cmd->is_empty_operation()) {			
						 parrent_cmd->max_counter = cmd->max_counter;
						 parrent_cmd->min_counter = cmd->min_counter;	 
					 }
				 }
			 }
		}

	//	#define debug_print_global_cmd

		void print_global_cmd(gcmd_t* command_graph, std::size_t& count_base_signature, bool is_render_tree)
		{
			cmd_t* cmd		   = &command_graph->get_value();
			cmd_t* parrent_cmd = &command_graph->parent->get_value();

			bool is_or       = false;
			bool is_and      = false;
			bool is_xor      = false;
			bool is_not      = false;
			bool is_value    = false;
			bool is_position = false;
		

#ifdef debug_print_global_cmd

			show_tree print_space_cmd(command_graph->level, command_graph->is_have_sub_elemets(), cmd);

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
					else
					{
						show_tree fmt::print(fg(fmt::color::thistle), " \"{}\"", cmd->value);
					}
				}
			}
#endif // debug_print_global_cmd


			if (cmd->is_value())
			{
				is_position = true;
				is_value    = true;

#ifdef debug_print_global_cmd
				show_tree fmt::print(fg(fmt::color::deep_sky_blue), " [value]");
#endif // debug_print_global_cmd

			}

			if (cmd->is_or()) {

				is_or = true;
				is_position = true;
#ifdef debug_print_global_cmd
				show_tree  fmt::print(" [or]");
#endif
			}

			if (cmd->is_and()) {

				is_and = true;
				is_position = true;

#ifdef debug_print_global_cmd
				show_tree  fmt::print(" [and]");
#endif
			}

			if (cmd->is_empty_operation()) {

				is_and = true;
				is_position = true;

#ifdef debug_print_global_cmd
				show_tree  fmt::print(" [empty_operation]");
#endif
			}

			if (cmd->is_xor()) {

				is_xor = true;
				is_position = true;

#ifdef debug_print_global_cmd
				show_tree  fmt::print(" [xor]");
#endif
			}

			if (cmd->is_not()) {


				is_not = true;
				is_position = true;

#ifdef debug_print_global_cmd
				show_tree  fmt::print(" [not]");
#endif
			}

			if (cmd->is_type()) {

				is_position = true;

#ifdef debug_print_global_cmd
				show_tree fmt::print(fg(fmt::color::aquamarine), " [type]");
#endif
			}

#ifdef debug_print_global_cmd
			if (cmd->is_ex())
				show_tree fmt::print(fg(fmt::color::red), " [ex]");
#endif


			if (command_graph->is_root)
			{
				command_graph->get_value().min_position = 0;
			}

			
			// TODO: remove?
			{
				bool is_result = false;

				find_or(command_graph, is_result);

				if (is_result)
				{
					std::add_flag(command_graph->get_value().flag, parser_flag_t::parser_tree_or);
#ifdef debug_print_global_cmd
					show_tree fmt::print(" [parser_tree_or]");
#endif
				}
			}
	
			if (cmd->is_type())
			{
				cmd->min_counter = parrent_cmd->min_counter;
				cmd->max_counter = parrent_cmd->max_counter;	
			}

			if (is_value)
			{
				cmd->min_position = parrent_cmd->min_counter;
				cmd->max_position = parrent_cmd->max_counter;
	
				if (!parrent_cmd->is_or()) {
					parrent_cmd->min_counter++;
					parrent_cmd->max_counter++;
				}
			}

#ifdef debug_print_global_cmd
			show_tree fmt::print("\n");
#endif
		}

		void print_global_cmd2(gcmd_t* command_graph, std::size_t& count_base_signature, bool is_render_tree)
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
					else
					{
						show_tree fmt::print(fg(fmt::color::thistle), " \"{}\"", cmd->value);
					}
				}
			}

			if (cmd->is_value())
			{
				is_position = true;
				is_value = true;

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

			if (cmd->is_ex())
				show_tree fmt::print(fg(fmt::color::red), " [ex]");


			if (is_value)
			{
				bool is_result = false;

				find_or(command_graph, is_result);

				if (is_result)
				{
					std::add_flag(command_graph->get_value().flag, parser_flag_t::parser_tree_or);

					show_tree fmt::print(" [parser_tree_or]");
				}
			}

			if (is_value)
			{
				bool is_result = false;

				find_xor(command_graph, is_result);

				if (is_result)
				{
					std::add_flag(command_graph->get_value().flag, parser_flag_t::parser_tree_xor);

					show_tree fmt::print(" [parser_tree_xor]");
				}
			}

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
			std::size_t     count_signaturs = 0;

			block_depth_t<data_block_global_gcmd_t> block_depth;
		};

		using global_gcmd_t = std::vector<element_gcmd_t>;

		void copy_process_gcmd(gcmd_t* gcmd, gcmd_t* data)
		{
			data->set_value(gcmd->get_value());

			for (auto& tree_element : gcmd->tree)
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


		void make_commands(global_gcmd_t* global_gcmd, bool is_render_tree)
		{		
			for (auto& it : *global_gcmd)
			{
				next_and_position = 0;

				last_min = 0;
				last_max = 0;

				next_and_position   = 0;
				global_min_position = 0;
				global_max_position = 0;

				dynamic_max = 0;
				dynamic_min = 0;
				count_or    = 0;

				it.gcmd->process_function["base"] = detail::bind_function(&print_global_cmd, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
				it.gcmd->process_function["last_parrent"] = detail::bind_function(&last_global_cmd, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);

				it.gcmd->start_process(it.count_signaturs, is_render_tree);

				//fmt::print("\n\n");

				it.gcmd->process_function["base"] = detail::bind_function(&print_global_cmd2, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
				it.gcmd->start_process(it.count_signaturs, is_render_tree);

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
			}
		}
	}
}