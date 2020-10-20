#pragma once
#include "cmd_make.hpp"

namespace parser
{
	namespace executive
	{
		/*
			In this space, there are algorithms that use real recursion, which loads the stack,
			but designing with real recursion is very nice. The alternative is to emulate recursion,
			which will get rid of all the disadvantages of recursion.
		*/
		namespace real_recursion {

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

			void copy_process_gcmd_with_reinit(const gcmd_t* gcmd, gcmd_t* data)
			{
				data->set_value(gcmd->get_value());

				// reinit min_position
				data->get_value().reinit();

				for (const auto& tree_element : gcmd->tree)
				{
					gcmd_t* sub_data = data->push(tree_element->get_value());

					if (tree_element->size() > 0)
						copy_process_gcmd_with_reinit(tree_element, sub_data);
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

						element_gcmd.count_signatures = it.count_signatures;
						element_gcmd.gcmd = data;

						to_gcmd->push_back(element_gcmd);
					}
				}
			}

			gcmd_t* find_vertex_recursion(gcmd_t* command_graph, const std::string& name) {

				cmd_t* cmd = &command_graph->get_value();

				if (cmd->value == name)
				{
					return command_graph;
				}

				if (command_graph->is_root)
					return nullptr;

				return find_vertex_recursion(command_graph->parent, name);
			}

			void calc_recursion(gcmd_t* command_graph, recursion_gcmd_t*& recursion_gcmd, std::size_t& count_base_signature, bool is_render_tree)
			{
				cmd_t* cmd = &command_graph->get_value();
				cmd_t* parent_cmd = &command_graph->parent->get_value();

				if (cmd->is_recursion())
				{
					element_gcmd_t recursion_element;

					recursion_element.gcmd = new gcmd_t;

					//	gcmd_t* graph = find_vertex_recursion(command_graph->parent, cmd->value);
					gcmd_t* graph = find_vertex_recursion(command_graph->parent, cmd->value);

					if (graph) {

						//copy_process_gcmd_with_reinit(graph, recursion_element.gcmd);
						copy_process_gcmd_with_reinit(graph, recursion_element.gcmd);

						recursion_gcmd->push_back(recursion_element);

						cmd->recursion_element = recursion_element.gcmd;
						recursion_element.gcmd->get_value().recursion_element = recursion_element.gcmd;
					}
					else {
						// TODO: Critical error!
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

			void calc_recursion_for_recursion_gcmd(gcmd_t* command_graph, recursion_gcmd_t*& recursion_gcmd, std::size_t& count_base_signature, bool is_render_tree)
			{
				cmd_t* cmd = &command_graph->get_value();
				cmd_t* parent_cmd = &command_graph->parent->get_value();

				if (cmd->is_recursion())
				{
					if (!cmd->recursion_element)
					{
						for (const auto& it : *recursion_gcmd)
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
		}
	}
}