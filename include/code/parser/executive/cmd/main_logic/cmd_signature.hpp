#pragma once

#include "real_recursion.hpp"
#include "emulate_recursion.hpp"

#include "../groups_logic/cmd_groups_make.hpp"

#define print_timers false
#define end_return  { show_tree fmt::print("\n");   return; }

// TODO: Хвост зацикливает, пофиксить
//#define FIRST_CHIELD_OPTIMISITION


namespace parser
{
	namespace executive
	{
        struct data_block_t
        {
            global_gcmd_t* gcmd = nullptr;
            int  current_position = 0;
            int  count_not_signature = 0;
            bool is_status_find = false;
        };

        enum class format_word_t {
            one_word,
            multi_word
        };

        struct base_arg_t
        {
            format_word_t format_word = format_word_t::one_word;

            pel::groups::position_element_t* multi_words;

            words_base_t* element;
            data_block_t* region;
            tree_words_t* word;
        };

        int total_operation = 0;

        void final_signature(gcmd_t* command_graph, base_arg_t* arg, int count_signatures, bool& is_use)
        {
            cmd_t* cmd = &command_graph->get_value();
            cmd_t* parent_cmd = &command_graph->parent->get_value();
            cmd_t* root_cmd = &command_graph->root->get_value();

            bool is_result_final_signature = true;

            //	   if (command_graph->get_value().is_last)
            {
                if (cmd->status_process.status_find == status_find_t::success)
                {
                    if (arg->format_word == format_word_t::one_word)
                    {
                        if (is_result_final_signature)   fmt::print(fg(fmt::color::lawn_green), "\nLine: {3} - its signature: {0} [count op: {1}, total op: {2}]", root_cmd->value, root_cmd->count_operation, total_operation, arg->element->number_line);


                    }
                    else if (arg->format_word == format_word_t::multi_word)
                    {
                        if (is_result_final_signature)  fmt::print(fg(fmt::color::lawn_green), "\nIts signature: {0} [count op: {1}, total op: {2}]", root_cmd->value, root_cmd->count_operation, total_operation);
                    }

                    arg->region->is_status_find = true;
                }
                else if (cmd->status_process.status_find == status_find_t::failed)
                {
                    if (arg->format_word == format_word_t::one_word)
                    {
                        if (is_result_final_signature)   fmt::print(fg(fmt::color::indian_red), "\nLine: {4} - its not signature {0}: {1}[{5}:{6}] [count op: {2}, total op: {3}",
                            root_cmd->value.c_str(),
                            arg->element->data,
                            root_cmd->count_operation,
                            total_operation,
                            arg->element->number_line,
                            arg->element->start_position,
                            arg->element->end_position
                        );

                    }
                    else if (arg->format_word == format_word_t::multi_word)
                    {
                        if (is_result_final_signature)  fmt::print(fg(fmt::color::indian_red), "\nIts not signature {0}: {1} total operaion: {2}",
                            root_cmd->value.c_str(),
                            root_cmd->count_operation,
                            total_operation
                        );
                    }

                    arg->region->is_status_find = false;
                }
                else
                    if (cmd->status_process.status_find == status_find_t::unknow)
                    {
                        if (is_result_final_signature)   fmt::print(fg(fmt::color::burly_wood), "\nIts maybe signature {0}: {1} total operaion: {2}",
                            root_cmd->value.c_str(),
                            root_cmd->count_operation,
                            total_operation
                        );
                    }

                if (command_graph->get_value().status_process.is_status_exit)
                    if (is_result_final_signature)  fmt::print(fg(fmt::color::gold), " [chain reached exit]");

                if (is_result_final_signature)  fmt::print("\n");

                is_use = false;
                command_graph->stop_process();
            }
        }

        void last_signature(gcmd_t* command_graph, base_arg_t* arg, int count_signatures, bool& is_use)
        {
            cmd_t* cmd = &command_graph->get_value();
            cmd_t* parent_cmd = &command_graph->parent->get_value();
            cmd_t* root_cmd = &command_graph->root->get_value();
        }

        void last_parent(gcmd_t* command_graph, gcmd_t* first_child_graph, gcmd_t* last_child_graph, base_arg_t* arg, int count_signatures, bool& is_use)
        {
            cmd_t* cmd = &command_graph->get_value();
            cmd_t* parent_cmd = &command_graph->parent->get_value();
            cmd_t* root_cmd = &command_graph->root->get_value();

            //  if (cmd->max_position < arg->region->current_position || cmd->min_position > arg->region->current_position)
            //      return;

            std::size_t max_position = cmd->max_position;
            std::size_t min_position = cmd->min_position;

            if (max_position < arg->region->current_position || min_position > arg->region->current_position) {
                return;
            }

            if (cmd->is_type() && command_graph->is_last() && !parent_cmd->is_or()) {
                parent_cmd->is_end_find = cmd->is_end_find;
            }

            /*
               Решается ли это без перебора? Это линейно, но меня жутко раздражает, что надо каждую суб-ор вершину опрашивать.
               Другого алгоритма я не нашел.
            */
            if (cmd->is_type() && cmd->is_or() && !cmd->is_finaly_or)
            {
                status_find_t tmp_status_find = status_find_t::failed;

                bool is_have_not_checked = false;
                bool is_all_status_end_checked = false;
                bool is_have_success = false;

                int64_t count_success = 0, count_failed = 0;
                std::size_t size_or = command_graph->tree.size();
                bool state_move = false;

                for (std::size_t i = 0; i < command_graph->tree.size(); i++)
                {
                    if (command_graph->tree[i]->get_value().is_end_find && command_graph->tree[i]->get_value().status_process.status_find == status_find_t::success)
                    {
                        count_success++;

                        std::size_t len = (arg->region->current_position + 1) < command_graph->tree[i]->tree.size() ? command_graph->tree[i]->tree.size() - (arg->region->current_position + 1) : (arg->region->current_position + 1) - command_graph->tree[i]->tree.size();

                        if (len == 0)
                        {
                            state_move = true;
                            tmp_status_find = command_graph->tree[i]->get_value().status_process.status_find;
                        }

                        if (len >= 1)
                        {
                            tmp_status_find = command_graph->tree[i]->get_value().status_process.status_find;
                        }
                    }

                    if (command_graph->tree[i]->get_value().status_process.status_find == status_find_t::failed)
                    {
                        count_failed++;
                    }

                    if (!command_graph->tree[i]->get_value().is_end_find)
                    {
                        is_have_not_checked = true;
                    }
                }

                if (
                    size_or == (count_failed + count_success)
                    )
                {
                    if (state_move)
                    {
                        cmd->is_inc_current_index = true;
                    }
                    else
                    {
                        cmd->current_index++;
                    }

                    cmd->is_end_find = true;
                    cmd->is_finaly_or = true;

                    cmd->status_process.status_find = tmp_status_find;

                    if (!parent_cmd->is_or())
                    {
                        if (command_graph->is_last())
                            parent_cmd->is_end_find = true;
                    }
                }
            }

            // inversion
            if (cmd->is_not() && cmd->status_process.status_find != status_find_t::unknow)
            {
                if (cmd->status_process.status_find == status_find_t::success) {
                    cmd->status_process.status_find = status_find_t::failed;
                }
                else
                {
                    if (cmd->status_process.status_find == status_find_t::failed)
                        cmd->status_process.status_find = status_find_t::success;
                }
            }

            if (cmd->is_return() && cmd->status_process.status_find == status_find_t::success)
            {
                cmd->is_status_return = true;
                parent_cmd->is_status_return = true;
            }

            if (cmd->is_exit() && cmd->status_process.status_find == status_find_t::success)
            {
                cmd->status_process.is_status_exit = true;
            }

            if (cmd->is_maybe() && cmd->status_process.status_find != status_find_t::unknow)
            {
                cmd->status_process.status_find = status_find_t::success;
            }

            if (cmd->is_type() && cmd->is_or() && parent_cmd->is_or())
            {
                // ?
            }

            if (cmd->is_type() && cmd->is_or() && !parent_cmd->is_or())
            {
                if (parent_cmd->status_process.status_find != status_find_t::failed)
                    parent_cmd->status_process = cmd->status_process;

                if (cmd->is_end_find)
                {
                    //parent_cmd->current_index++;
                    parent_cmd->current_index = cmd->current_index;
                }
            }

            if (cmd->is_type() && !cmd->is_or() && !parent_cmd->is_or())
            {
                if (parent_cmd->status_process.status_find != status_find_t::failed)
                    parent_cmd->status_process = cmd->status_process;

                if (cmd->is_end_find)
                {
                    parent_cmd->current_index++;
                }
            }

#ifdef FIRST_CHIELD_OPTIMISITION
            if (cmd->is_end_find)
            {
                if (command_graph->next)
                    command_graph->parent->first_chield = command_graph->next;
            }
#endif // FIRST_CHIELD_OPTIMISITION

            if ((command_graph->is_root && cmd->is_end_find) || (command_graph->is_root && cmd->status_process.is_status_exit))
            {
                final_signature(command_graph, arg, count_signatures, is_use);
            }
        }


        void process_signature_base(gcmd_t* command_graph, base_arg_t* arg, int count_signatures, bool& is_use, bool is_render_tree)
        {
            cmd_t* cmd = &command_graph->get_value();
            cmd_t* parent_cmd = &command_graph->parent->get_value();
            cmd_t* root_cmd = &command_graph->root->get_value();

            // can null need use with check  "if (arg->format_word == format_word_t::one_word)"
            words_base_t* element = arg->element;

            // это просто счетчик операций для статистики
            root_cmd->count_operation++;
            total_operation++;

            if (is_render_tree) {

                if (command_graph->is_root)
                {
                    fmt::print("\n");

                    if (arg->format_word == format_word_t::one_word) {
                        fmt::print("Value: {}", arg->element->data);

                        fmt::print(" [");
                        fmt::print(fg(fmt::color::coral), "{0}", arg->region->current_position);
                        fmt::print("]");

                        fmt::print("\n\n");
                    }
                    else if (arg->format_word == format_word_t::multi_word)
                    {
                        for (auto& sub : arg->multi_words->words)
                        {
                            int count_space = 1;

                            if (arg->multi_words->position < 1000)
                                count_space = 1;

                            if (arg->multi_words->position < 100)
                                count_space = 2;

                            if (arg->multi_words->position < 10)
                                count_space = 3;

                            fmt::print(fmt::fg(fmt::color::brown), "{}", (char)221);

                            if (count_space == 3)
                                fmt::print(fmt::fg(fmt::color::lime_green), " {} ", arg->multi_words->position);

                            if (count_space == 2)
                                fmt::print(fmt::fg(fmt::color::lime_green), "{} ", arg->multi_words->position);

                            if (count_space == 1)
                                fmt::print(fmt::fg(fmt::color::lime_green), "{} ", arg->multi_words->position);

                            fmt::print(fmt::fg(fmt::color::thistle), "\"{0}\"", sub.data);
                            fmt::print(fmt::fg(fmt::color::red), ": ", sub.data);
                            fmt::print(fmt::fg(fmt::color::blanched_almond), "{0}", sub.group->name);
                            fmt::print(fmt::fg(fmt::color::white), ";");

                            fmt::print(" [");
                            fmt::print(fg(fmt::color::coral), "{0}", arg->region->current_position);
                            fmt::print("]");

                            fmt::print("\n\n");
                        }
                    }
                }

                if (!cmd->value.empty()) {

                    print_space_cmd(command_graph->level, command_graph->is_have_sub_elemets(), cmd);

                    if (command_graph->is_root)
                    {
                        fmt::print(fg(fmt::color::coral), " {}", cmd->value);
                    }
                    else
                    {
                        if (cmd->is_type())
                        {
                            fmt::print(fg(fmt::color::blanched_almond), " {}", cmd->value);
                        }

                        if (cmd->is_value())
                        {
                            fmt::print(fg(fmt::color::thistle), " \"{}\"", cmd->value);
                        }

                        if (cmd->is_group())
                        {
                            fmt::print(fg(fmt::color::khaki), " {}", cmd->value);
                        }
                    }

                    fmt::print(" [");
                    fmt::print(fg(fmt::color::coral), "{0}", cmd->min_position);
                    fmt::print("-");
                    fmt::print(fg(fmt::color::coral), "{0}", cmd->max_position);
                    fmt::print("]");

                    fmt::print(" pos: {0}", command_graph->position);

                    if (cmd->is_last)
                    {
                        fmt::print(fg(fmt::color::orange_red), " <last element>");
                    }
                }
            }

            if (cmd->status_process.is_status_exit || parent_cmd->status_process.is_status_exit)
            {
                show_tree fmt::print(" [");
                show_tree fmt::print(fg(fmt::color::alice_blue), "status exit");
                show_tree fmt::print("]");

                end_return;
            }

            if (cmd->is_status_return || parent_cmd->is_status_return)
            {
                if (command_graph->is_last())
                {
                    cmd->is_end_find = true;

                    if (!parent_cmd->is_or()) {

                        parent_cmd->is_end_find = true;
                    }
                }

                show_tree fmt::print(" [");
                show_tree fmt::print(fg(fmt::color::blue_violet), "status return");
                show_tree fmt::print("]");

                end_return;
            }

            if (cmd->is_or() && cmd->is_end_find && cmd->is_inc_current_index)
            {
                parent_cmd->current_index++;
                cmd->is_inc_current_index = false;
            }

            std::size_t max_position = cmd->max_position;
            std::size_t min_position = cmd->min_position;

            if (max_position < arg->region->current_position || min_position > arg->region->current_position) {

                if (max_position < arg->region->current_position)
                {
                    show_tree fmt::print(" [");
                    show_tree fmt::print(fg(fmt::color::alice_blue), "skip position max {0} < {1}", max_position, arg->region->current_position);
                    show_tree fmt::print("]");
                }

                if (min_position > arg->region->current_position)
                {
                    show_tree fmt::print(" [");
                    show_tree fmt::print(fg(fmt::color::blanched_almond), "skip position min {0} > {1}", min_position, arg->region->current_position);
                    show_tree fmt::print("]");
                }

                end_return;
            }

            if (parent_cmd->current_index != command_graph->position && !parent_cmd->is_or()) {
                show_tree fmt::print(" [");
                show_tree fmt::print(fg(fmt::color::gold), "skip current index {0} graph pos {1}", cmd->current_index, command_graph->position);
                show_tree fmt::print("]");
                end_return;
            }

            if (cmd->is_check && cmd->is_type()) {
             
                end_return;
            }

            if (cmd->is_check) {
              
                end_return;
            }

            if (cmd->is_type())
            {
                if (cmd->is_or())
                {
                    cmd->status_process.status_find = status_find_t::unknow;

                }
                else
                {
                    if (!parent_cmd->is_or())
                        cmd->status_process = parent_cmd->status_process;
                }

                if (parent_cmd->is_or() && parent_cmd->status_process.status_find != status_find_t::success)
                {
                    parent_cmd->status_process.status_find = status_find_t::unknow;
                }
            }

            // time for epic
            if (cmd->is_recursion())
            {
                if (cmd->recursion_element && !cmd->is_status_allocate_recursion_graph)
                {
                    // allocate graph
                    emulate_recursion::copy_process_gcmd(cmd->recursion_element, command_graph);

                    cmd->is_status_allocate_recursion_graph = true;

                    show_tree fmt::print(fg(fmt::color::green_yellow), " [allocate recursion graph]");

                    std::size_t position = 0;
                    emulate_recursion::get_position_from_parent_to_root(command_graph, position);

                    gcmd_t* cmd_left = nullptr;

                    // recalc position
                    if (position > 0 && command_graph->root->size() > 0)
                        cmd_left = command_graph->root->tree[position - 1];

                    if (cmd_left)
                    {
                        command_graph->root->get_value().min_counter = cmd_left->get_value().min_counter + 1;
                        command_graph->root->get_value().max_counter = cmd_left->get_value().max_counter + 1;
                    }
                    else
                    {
                        command_graph->root->get_value().min_counter = 0;
                        command_graph->root->get_value().max_counter = 0;
                    }

                    // TODO: check it in test
                    if (command_graph->root->size() > 0) {
                       emulate_recursion::recalc_position_in_graph_from_position(command_graph->root, position);
                    }

                    show_tree  fmt::print("\nRecalc position:\n");
                    show_tree  emulate_recursion::print_graph(command_graph->root);

                }
            }

            int status = 0;

            if (cmd->is_group() || cmd->is_value()) {

                cmd->status_process = parent_cmd->status_process;

                cmd->is_check = true;

                if (command_graph->is_last())
                {
                    parent_cmd->is_check = true;
                }
            }

            if (cmd->is_group())
            {
                // not support one word, because multi word had group
                if (arg->format_word == format_word_t::multi_word && cmd->group)
                {
                    status = 0;

                    for (const auto& sub : arg->multi_words->words)
                    {
                        if (sub.group == cmd->group)
                        {
                            status = 1;
                            break;
                        }
                    }
                }
            }

            if (cmd->is_value())
            {
                if (arg->format_word == format_word_t::one_word)
                {
                    status = cmd->value == element->data;
                }
                else if (arg->format_word == format_word_t::multi_word)
                {
                    status = 0;

                    for (const auto& sub : arg->multi_words->words)
                    {
                        if (sub.data == cmd->value)
                        {
                            status = 1;
                            break;
                        }
                    }
                }
            }

            // inversion
            if (cmd->is_not())
                status = !status;

            if (cmd->is_maybe())
                status = 1;

            if (cmd->is_exit() && status == 1)
                cmd->status_process.is_status_exit = true;

            if (cmd->is_return() && status == 1)
                cmd->is_status_return = true;

            if (cmd->is_group() || cmd->is_value())
            {
                if (cmd->status_process.is_status_exit)
                {
                    parent_cmd->status_process.is_status_exit = true;
                }

                if (cmd->is_status_return)
                {
                    parent_cmd->is_status_return = true;
                }

                if (status == 1)
                {
                    show_tree fmt::print(fg(fmt::color::green_yellow), " [true]");

                    if (!parent_cmd->is_or())
                    {
                        cmd->status_process.status_find = status_find_t::success;

                        if (parent_cmd->status_process.status_find != status_find_t::failed)
                            parent_cmd->status_process = cmd->status_process;

                        parent_cmd->current_index++;
                    }

                    if (parent_cmd->is_or())
                    {
                        cmd->status_process.status_find = status_find_t::success;
                        cmd->is_end_find = true;
                    }
                }
                else
                {
                    show_tree fmt::print(fg(fmt::color::pale_violet_red), " [false]");

                    if (!parent_cmd->is_or())
                    {
                        cmd->status_process.status_find = status_find_t::failed;

                        parent_cmd->status_process = cmd->status_process;

                        parent_cmd->current_index++;
                    }

                    if (parent_cmd->is_or())
                    {
                        cmd->status_process.status_find = status_find_t::failed;
                    }
                }

                if (command_graph->is_last())
                {
                    cmd->is_end_find = true;

                    if (!parent_cmd->is_or()) {

                        parent_cmd->is_end_find = true;
                    }
                }

#ifdef FIRST_CHIELD_OPTIMISITION
                if (cmd->is_end_find)
                {
                    if (command_graph->next)
                        command_graph->parent->first_chield = command_graph->next;
                }

                // solo element, we can skip it
                if (command_graph->size() == 0)
                {
                    if (command_graph->next)
                        command_graph->parent->first_chield = command_graph->next;
                }
#endif
            }

            {
                end_return;
            }
        }
		
	}
}