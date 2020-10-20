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

					if (current_graph->first_chield) {
						current_graph = current_graph->first_chield;
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
									current_data = current_data->parent->push(current_graph->get_value());
									break;
								}
								else {
									parent_iterator = parent_iterator->parent;
									current_data = current_data->parent;
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

					if (current_graph->first_chield) {
						current_graph = current_graph->first_chield;
						current_data = current_data->push(current_graph->get_value());

						current_data->get_value().reinit();
					}
					else
						if (current_graph->next) {
							current_graph = current_graph->next;
							current_data = current_data->parent->push(current_graph->get_value());

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
									current_data = current_data->parent->push(current_graph->get_value());

									current_data->get_value().reinit();

									break;
								}
								else {
									parent_iterator = parent_iterator->parent;
									current_data = current_data->parent;
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

					if (current_graph->first_chield) {
						current_graph = current_graph->first_chield;

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


		}
	}
}