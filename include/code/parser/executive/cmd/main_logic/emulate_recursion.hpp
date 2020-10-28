#pragma once
#include "cmd_make.hpp"


namespace parser
{
	namespace executive
	{
		namespace emulate_recursion {

			void copy_process_gcmd(const gcmd_t* command_graph, gcmd_t* data)
			{
				const gcmd_t* current_graph = command_graph;

				gcmd_t* current_data = data;
				gcmd_t* parent_iterator = nullptr;

				bool is_exit_recursion = false;

				for (;;)
				{
					// body
					// TODO: check
					current_data->set_value(current_graph->get_value());

					if (current_graph->first_child) {
						current_graph = current_graph->first_child;
						current_data = current_data->push(current_graph->get_value());
					}
					else
						if (current_graph->next) {
							current_graph = current_graph->next;
							current_data = current_data->parent->push(current_graph->get_value());
						}
						else
						{
							parent_iterator = current_graph->parent;
							current_data = current_data->parent;

							for (;;)
							{
								if (parent_iterator->next)
								{
									current_graph = parent_iterator->next;
									current_data  = current_data->parent->push(current_graph->get_value());
									break;
								}
								else {
									parent_iterator = parent_iterator->parent;
									current_data    = current_data->parent;
								}

								if (parent_iterator->is_root || current_graph == command_graph) {
									is_exit_recursion = true;
									break;
								}
							}
						}

					if (is_exit_recursion)
						break;
				}
			}

			void copy_process_gcmd_with_reinit(const gcmd_t* command_graph, gcmd_t* data)
			{
				const gcmd_t* current_graph = command_graph;

				gcmd_t* current_data = data;
				gcmd_t* parent_iterator = nullptr;

				bool is_exit_recursion = false;

				for (;;)
				{
					// body
					// TODO: check
					current_data->set_value(current_graph->get_value());
					current_data->get_value().reinit();

					if (current_graph->first_child) {

						current_graph = current_graph->first_child;
						current_data  = current_data->push(current_graph->get_value());

						current_data->get_value().reinit();
					}
					else
						if (current_graph->next) {

							current_graph = current_graph->next;
							current_data  = current_data->parent->push(current_graph->get_value());

							current_data->get_value().reinit();
						}
						else
						{
							parent_iterator = current_graph->parent;
							current_data = current_data->parent;

							for (;;)
							{
								if (parent_iterator->next)
								{
									current_graph = parent_iterator->next;
									current_data  = current_data->parent->push(current_graph->get_value());

									current_data->get_value().reinit();

									break;
								}
								else {
									parent_iterator = parent_iterator->parent;
									current_data    = current_data->parent;
								}

								if (parent_iterator->is_root || current_graph == command_graph) {
									is_exit_recursion = true;
									break;
								}
							}
						}

					if (is_exit_recursion)
						break;
				}
			}

			gcmd_t* find_vertex_recursion(gcmd_t* command_graph, const std::string& name) {

				gcmd_t* current_graph   = command_graph;
				gcmd_t* parent_iterator = nullptr;

				bool is_exit_recursion  = false;

				for (;;)
				{
					// body
					// TODO: check

					if (current_graph->get_value().value == name)
						return current_graph;

					if (current_graph->is_root)
						return nullptr;

					if (current_graph->first_child) {
						current_graph = current_graph->first_child;

					}
					else
						if (current_graph->next) {
							current_graph = current_graph->next;

						}
						else
						{
							parent_iterator = current_graph->parent;

							for (;;)
							{
								if (parent_iterator->next)
								{
									current_graph = parent_iterator->next;
									break;
								}
								else {

									parent_iterator = parent_iterator->parent;

								}

								if (parent_iterator->is_root || current_graph == command_graph) {
									is_exit_recursion = true;
									break;
								}
							}
						}

					if (is_exit_recursion)
						break;
				}

				return nullptr;
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

			void print_graph(gcmd_t* command_graph)
			{
				if (!command_graph)
					return;

				std::size_t need_remove = 0;

				gcmd_t* current_graph   = command_graph;

				bool is_exit_recursion = false;

				for (;;)
				{
					if (current_graph->is_value)
						print_graph_gcmd(current_graph, need_remove, true);
	
					if (current_graph->real_first_child) {
						current_graph = current_graph->real_first_child;
					}
					else
						if (current_graph->next) {
							current_graph = current_graph->next;
						}
						else
						{
							current_graph = current_graph->parent;

							for (;;)
							{
								if (current_graph->next)
								{
									current_graph = current_graph->next;
									break;
								}
								else {
									current_graph = current_graph->parent;
								}

								if (current_graph->is_root || current_graph == command_graph) {
									is_exit_recursion = true;
									break;
								}
							}
						}

					if (is_exit_recursion)
						break;
				}
			}

			void recursion_tree_traversals_positions(gcmd_t* command_graph, std::size_t level)
			{
				if (!command_graph)
					return;

				std::size_t need_remove   = 0;
				std::size_t counter_level = level; 

				gcmd_t* current_graph = command_graph;

				bool is_exit_recursion = false;

				for (;;)
				{
					if (current_graph->is_value) {

						current_graph->level = counter_level;

						calc_position_in_graph(current_graph, need_remove, false);
					}

					if (current_graph->real_first_child) {
						counter_level++;
						current_graph = current_graph->real_first_child;
					}
					else
						if (current_graph->next) {
							current_graph = current_graph->next;
						}
						else
						{
							if (current_graph->is_value) {

								if (current_graph->is_last())
								{
									calc_position_in_graph_for_parent(current_graph->parent, current_graph->parent->size() > 0 ? current_graph->parent->tree[0] : nullptr, current_graph->parent->size() > 0 ? current_graph->parent->tree[current_graph->parent->size() - 1] : nullptr, need_remove, false);
								}
							}

							current_graph = current_graph->parent;

							for (;;)
							{
								if (current_graph->next)
								{
									current_graph = current_graph->next;
									counter_level--;
									break;
								}
								else {

									current_graph = current_graph->parent;

									if (current_graph->is_value) {	
										calc_position_in_graph_for_parent(current_graph, current_graph->size() > 0 ? current_graph->tree[0] : nullptr, current_graph->size() > 0 ? current_graph->tree[current_graph->size() - 1] : nullptr, need_remove, false);										
									}

								}

								if (current_graph->is_root || current_graph == command_graph) {
									is_exit_recursion = true;
									break;
								}
							}
						}

					if (is_exit_recursion) {
						break;
					}
				}	
			}

			bool sort_function(const gcmd_t *i, const gcmd_t *j) { return (i->tree.size() < j->tree.size()); }

			void sort_graph_in_or(gcmd_t* command_graph)
			{
				if (!command_graph)
					return;

				gcmd_t* current_graph = command_graph;

				bool is_exit_recursion = false;

				for (;;)
				{
					if (current_graph->is_value) {

						if (current_graph->get_value().is_or() && current_graph->tree.size() > 0) {
							
							std::sort(current_graph->tree.begin(), current_graph->tree.end(), sort_function);

							current_graph->first_child = current_graph->tree[0];
							current_graph->real_first_child = current_graph->first_child;

							for (size_t i = 0; i < current_graph->tree.size(); i++)
							{
								current_graph->tree[i]->position = i;
								current_graph->tree[i]->calculate_previous_next();
							}

						}
					
					}

					if (current_graph->real_first_child) {
					
						current_graph = current_graph->real_first_child;
					}
					else
						if (current_graph->next) {
							current_graph = current_graph->next;
						}
						else
						{

							current_graph = current_graph->parent;

							for (;;)
							{
								if (current_graph->next)
								{
									current_graph = current_graph->next;
							
									break;
								}
								else {

									current_graph = current_graph->parent;

								}

								if (current_graph->is_root || current_graph == command_graph) {
									is_exit_recursion = true;
									break;
								}
							}
						}

					if (is_exit_recursion) {
						break;
					}
				}
			}	

			void recalc_position_in_graph(gcmd_t* command_graph)
			{
				recursion_tree_traversals_positions(command_graph, 0);
			}

			void get_position_from_parent_to_root(gcmd_t* command_graph, std::size_t& position)
			{
				if (!command_graph)
					return;

				position = 0;

				gcmd_t* current_graph = command_graph;

				bool is_exit_recursion = false;

				for (;;)
				{
					if (current_graph->parent->is_root) {
						position = current_graph->position;
						return;
					}

					current_graph = current_graph->parent;
				}
			}	

			void recalc_position_in_graph_from_position(gcmd_t* command_graph, const std::size_t& position)
			{
				if (!command_graph->root->is_process)
					return;

				if (!command_graph)
					return;

				std::size_t level = command_graph->level;

				std::size_t need_remove = 0;

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
								calc_position_in_graph_for_parent(command_graph, command_graph->tree[0], command_graph->tree[i], need_remove, false);
							}
						}

						level--;
					}
				}

			}
		}
	}
}