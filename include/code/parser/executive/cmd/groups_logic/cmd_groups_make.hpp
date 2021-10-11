#pragma once
#include "..\..\..\..\..\code\parser\executive\cmd\groups_logic\cmd_groups.hpp"

#include "..\..\..\..\..\code\detail\fmt\fmt\printf.h"
#include "..\..\..\..\..\code\detail\fmt\fmt\color.h"

namespace parser
{
	namespace executive
	{
		namespace groups
		{
			void print_space_cmd_group(const std::size_t& count, bool is_elements, cmd_group_t* cmd)
			{
				if (is_elements && count == 0)
				{
					fmt::print(fg(fmt::color::coral), "{}", (char)218);
					return;
				}

				std::size_t tmp_count = count;

				if (tmp_count == 1)
				{
					fmt::print("{}", (char)195);
				}
				else
				{
					for (size_t i = 0; i < tmp_count - 1; i++)
					{
						fmt::print("{}", (char)179);
					}

					fmt::print("{}", (char)195);
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

						if (cmd->is_last)
						{
							print(fg(fmt::color::aquamarine), "{}", (char)192);
						}
						else
						{
							print(fg(fmt::color::aquamarine), "{}", (char)191);
						}
						
					}
					else
					{
						print(fg(fmt::color::deep_sky_blue), "{}", (char)254);
					}
				}
			}

			void last_group_calculate(gcmd_group_t* command_graph, gcmd_group_t* first_child_graph, gcmd_group_t* last_child_graph, std::size_t& count_base_signature)
			{
				cmd_group_t* cmd = &command_graph->get_value();
				cmd_group_t* parent_cmd = &command_graph->parent->get_value();

			}
			
			void cmd_group_calculate(gcmd_group_t* command_graph)
			{
				cmd_group_t* cmd		 = &command_graph->get_value();
				cmd_group_t* parent_cmd = &command_graph->parent->get_value();

				bool is_value = false;

				if (command_graph->is_root)
				{
					cmd->min_position = 0;
				}

				cmd->min_position = 0;
			}

			void print_group_cmd(gcmd_group_t* command_graph, bool is_render_tree)
			{
				cmd_group_t* cmd = &command_graph->get_value();
				cmd_group_t* parent_cmd = &command_graph->parent->get_value();

				show_tree print_space_cmd_group(command_graph->level, command_graph->is_have_sub_elemets(), cmd);

				bool is_or       = false;
				bool is_and      = false;
				bool is_xor      = false;
				bool is_not      = false;
				bool is_value    = false;
				bool is_position = false;

				if (!cmd->group.name.empty() || cmd->group.size() > 0)
				{
					if (command_graph->is_root)
					{
						show_tree fmt::print(fg(fmt::color::coral), " {}", cmd->group.name);
					}
					else
					{
						if (cmd->is_type())
						{
							show_tree fmt::print(fg(fmt::color::blanched_almond), " {}", cmd->group.name);
						}
						else
						{   // cmd->group.get().
							//show_tree fmt::print(fg(fmt::color::thistle), " \"{}\"", "data group");

							show_tree fmt::print("{}", " {");

							if (cmd->group.get().size() > 0) {

								for (size_t i = 0; i < cmd->group.get().size() - 1; i++)
								{
									show_tree fmt::print(fg(fmt::color::thistle), "'{}'", (char)cmd->group.get()[i].element);
									show_tree fmt::print(", ");
								}

								show_tree fmt::print(fg(fmt::color::thistle), "'{}'", (char)cmd->group.get()[cmd->group.get().size() - 1].element);

								show_tree fmt::print("{}", "};");

							}
						}
					}
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

				if (cmd->is_not()) {

					is_not = true;
					is_position = true;

					show_tree  fmt::print(" [not]");
				}

				if (cmd->is_type()) {

					is_position = true;

					show_tree fmt::print(fg(fmt::color::gold), " [group]");
				}

				if (cmd->is_exists())
					show_tree fmt::print(fg(fmt::color::red), " [exists]");

				if (cmd->is_glue())
					show_tree fmt::print(fg(fmt::color::red), " [glue]");

				if (cmd->is_split())
					show_tree fmt::print(fg(fmt::color::red), " [split]");		
				
				if (cmd->is_ignore())
					show_tree fmt::print(fg(fmt::color::red), " [ignore]");

				show_tree fmt::print(" [");
				show_tree fmt::print(fg(fmt::color::coral), "{0}", cmd->min_position);
				show_tree fmt::print("-");
				show_tree fmt::print(fg(fmt::color::coral), "{0}", cmd->max_position);
				show_tree fmt::print("]");
				show_tree fmt::print("\n");
			}

			void find_last(gcmd_group_t* command_graph)
			{
				cmd_group_t* cmd = &command_graph->get_value();

				if (command_graph->size() == 0)
				{
				//	show_logs fmt::print("Last element: {}\n", cmd->group.name);
					cmd->is_last = true;
					return;
				}
				else
				{
					if (cmd->is_or())
					{
						cmd->is_last = true;
						return;
					}
					else
					{
						find_last(command_graph->tree.back());
					}

					return;
				}

				return;
			}

			void make_commands(global_gcmd_group_t* global_gcmd, bool is_render_tree)
			{
				for (auto& it : *global_gcmd)
				{
					it.gcmd->process_function["base"]		  = detail::bind_function(&cmd_group_calculate, std::placeholders::_1);
					it.gcmd->process_function["last_parent"]  = detail::bind_function(&last_group_calculate, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);

					it.gcmd->start_process();

					it.gcmd->process_function["base"] = detail::bind_function(&print_group_cmd, std::placeholders::_1, std::placeholders::_2);
					it.gcmd->start_process(is_render_tree);

					gcmd_group_t* tree = nullptr;

					if (it.gcmd->tree.size() > 0)
						tree = it.gcmd->tree.back();

					if (tree)
					{
						find_last(tree);
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
}