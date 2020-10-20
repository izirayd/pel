#pragma once

#include "real_recursion.hpp"
#include "emulate_recursion.hpp"

namespace parser
{
	namespace executive
	{
		void make_commands(global_gcmd_t* global_gcmd, recursion_gcmd_t* recursion_gcmd, bool is_render_tree)
		{
			for (auto& it : *global_gcmd)
			{
				it.gcmd->is_process = true;

				recalc_position_in_graph(it.gcmd);

				it.gcmd->stop_process();

				/*it.gcmd->process_function["base"]         = detail::bind_function(&calc_position_in_graph, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
				it.gcmd->process_function["last_parent"] = detail::bind_function(&calc_position_in_graph_for_parrent, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);

				it.gcmd->start_process(it.count_signatures, is_render_tree);

				it.gcmd->process_function.function_list.clear();*/

				it.gcmd->process_function["base"] = detail::bind_function(&print_graph_gcmd, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
				it.gcmd->start_process(it.count_signatures, is_render_tree);

				it.gcmd->process_function.function_list.clear();

				gcmd_t* tree = nullptr;

				if (it.gcmd->tree.size() > 0)
					tree = it.gcmd->tree.back();

				if (tree)
				{
					real_recursion::find_last(tree, is_render_tree);
				}
				else
				{
					// root last element
					it.gcmd->get_value().is_last = true;
				}

				it.gcmd->process_function["base"] = detail::bind_function(&real_recursion::calc_recursion, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
				it.gcmd->start_process(recursion_gcmd, it.count_signatures, is_render_tree);

				it.gcmd->process_function.function_list.clear();
			}

			for (auto& it : *recursion_gcmd)
			{
				it.gcmd->process_function["base"] = detail::bind_function(&real_recursion::calc_recursion_for_recursion_gcmd, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
				it.gcmd->start_process(recursion_gcmd, it.count_signatures, is_render_tree);

				it.gcmd->process_function.function_list.clear();
			}
		}

	}
}