#pragma once

#include "real_recursion.hpp"
#include "emulate_recursion.hpp"

#include "../groups_logic/cmd_groups_make.hpp"

#define print_timers false
#define end_return  { show_tree fmt::print("\n");   return; }

// TODO: Хвост зацикливает, пофиксить
// Shifts all nodes in such a way that no entry into already processed nodes occurs
#define FIRST_CHILD_OPTIMISITION
// Сделать сортировку элементов по длине подмножеств, для инструкций or

// If the next node in the root node is not available, then the search for vertices stops
#define NEXT_ELEMENT_SKIP_OPTIMISITION

#define SKIP_IN_SUBSETS_NODES_OPTIMISITION
#define SKIP_ERROR_NODES_OPTIMISITION

// TODO: The selected node will rebuild its positions during recursion only when the entry is reached at these positions
//#define RECURSION_REBUILD_OPTIMISITION

//#define FAKE_REPEAT_OPTIMISITION
//#define REAL_REPEAT_NODES

namespace parser
{
	namespace executive
	{
        struct data_block_t
        {
            global_gcmd_t* gcmd      = nullptr;

            int  current_position    = 0;
            int  count_not_signature = 0;
            bool is_status_find      = false;
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
            cmd_t* cmd        = &command_graph->get_value();
            cmd_t* parent_cmd = &command_graph->parent->get_value();
            cmd_t* root_cmd   = &command_graph->root->get_value();

            bool is_result_final_signature = true;

            //	   if (command_graph->get_value().is_last)
            {
                if (cmd->status_process.status_find == status_find_t::success)
                {
                    if (arg->format_word == format_word_t::one_word)
                    {
                        if (is_result_final_signature)   fmt::print(fg(fmt::color::lawn_green), "Line: {3} - its signature: {0} [count op: {1}, total op: {2}]", root_cmd->value, root_cmd->count_operation, total_operation, arg->element->number_line);
                    }
                    else if (arg->format_word == format_word_t::multi_word)
                    {
                        if (is_result_final_signature)  fmt::print(fg(fmt::color::lawn_green), "Its signature: {0} [count op: {1}, total op: {2}]", root_cmd->value, root_cmd->count_operation, total_operation);
                    }

                    arg->region->is_status_find = true;
                }
                else if (cmd->status_process.status_find == status_find_t::failed)
                {
                    if (arg->format_word == format_word_t::one_word)
                    {
                        if (is_result_final_signature)   
                            fmt::print(fg(fmt::color::indian_red), "Line: {4} - its not signature {0}: {1}[{5}:{6}] [count op: {2}, total op: {3}",
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
                        if (is_result_final_signature)  fmt::print(fg(fmt::color::indian_red), "Its not signature {0}: {1} total operaion: {2}",
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
                        if (is_result_final_signature)   fmt::print(fg(fmt::color::burly_wood), "Its maybe signature {0}: {1} total operaion: {2}",
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
            cmd_t* cmd        = &command_graph->get_value();
            cmd_t* parent_cmd = &command_graph->parent->get_value();
            cmd_t* root_cmd   = &command_graph->root->get_value();
        }

        void last_parent(gcmd_t* command_graph, gcmd_t* first_child_graph, gcmd_t* last_child_graph, base_arg_t* arg, int count_signatures, bool& is_use, bool is_render_tree, bool &is_skip_from_parent)
        {
            cmd_t* cmd        = &command_graph->get_value();
            cmd_t* parent_cmd = &command_graph->parent->get_value();
            cmd_t* root_cmd   = &command_graph->root->get_value();

            root_cmd->count_operation++;
            total_operation++;

            //  if (cmd->max_position < arg->region->current_position || cmd->min_position > arg->region->current_position)
            //      return;

            std::size_t max_position = cmd->max_position;
            std::size_t min_position = cmd->min_position;

            if (max_position < arg->region->current_position || min_position > arg->region->current_position) {
                return;
            }

            if (is_render_tree) {

                if (command_graph->is_root)
                {
                    fmt::print("\n");
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

                    fmt::print(" [");
                    fmt::print(fg(fmt::color::coral), "parent");
                    fmt::print("]");

                    fmt::print(" pos: {0} pci: {1} ci: {2}", command_graph->position, parent_cmd->current_index, cmd->current_index);

                    if (cmd->is_last)
                    {
                        fmt::print(fg(fmt::color::orange_red), " <last element>");
                    }
                }
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

                        std::size_t len_graph = (arg->region->current_position + 1) < command_graph->tree[i]->tree.size() ? command_graph->tree[i]->tree.size() - (arg->region->current_position + 1) : (arg->region->current_position + 1) - command_graph->tree[i]->tree.size();

                        if (len_graph == 0)
                        {
                            state_move      = true;
                            tmp_status_find = command_graph->tree[i]->get_value().status_process.status_find;
                        }

                        if (len_graph >= 1)
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

                    if (command_graph->tree[i]->get_value().status_process.status_find == status_find_t::unknow && command_graph->tree[i]->get_value().is_end_find)
                    {
                       // show_tree fmt::print(fg(fmt::color::aqua), " [wtf?!]");
                        count_failed++;
                    }

                }

                if (
                    size_or == (count_failed + count_success)
                    )
                {
                    if (state_move)
                    {
                        cmd->is_inc_current_index_parent = true;

                        show_tree fmt::print(fg(fmt::color::light_pink), " [start inc for {}]", parent_cmd->value);
                    }
                    else
                    {
                        show_tree fmt::print(" [");
                        show_tree fmt::print(fg(fmt::color::sea_green), "ci {} -> {} for {}", cmd->current_index, cmd->current_index + 1, cmd->value);
                        show_tree fmt::print("]");

                        cmd->current_index++;
                    }

                    cmd->is_end_find  = true;
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

                if (parent_cmd->is_autogen())
                    command_graph->parent->parent->get_value().is_status_return = true;
            }

            if (cmd->is_exit() && cmd->status_process.status_find == status_find_t::success)
            {
                cmd->status_process.is_status_exit = true;
            }

            if (cmd->status_process.is_status_exit)
            {
                parent_cmd->status_process.is_status_exit = true;
            }

            if (cmd->is_maybe() && cmd->status_process.status_find != status_find_t::unknow)
            {
                cmd->status_process.status_find = status_find_t::success;
            }

            if (cmd->is_type() && cmd->is_or() && parent_cmd->is_or())
            {
                if (cmd->is_end_find)
                {
                    //show_tree fmt::print(" [");
                    //show_tree fmt::print(fg(fmt::color::dark_sea_green), "pci {} -> {} for {}", parent_cmd->current_index, parent_cmd->current_index + 1, parent_cmd->value);
                    //show_tree fmt::print("]");

                    ////  parent_cmd->current_index++;
                    //parent_cmd->is_inc_current_index = true;
                }

            }

            if (cmd->is_type() && cmd->is_or() && !parent_cmd->is_or())
            {
                if (parent_cmd->status_process.status_find != status_find_t::failed)
                    parent_cmd->status_process = cmd->status_process;

                if (cmd->is_end_find)
                {
                    show_tree fmt::print(" [");
                    show_tree fmt::print(fg(fmt::color::dark_sea_green), "pci {} -> {} for {}", parent_cmd->current_index, parent_cmd->current_index + 1, parent_cmd->value);
                    show_tree fmt::print("]");

                   // parent_cmd->is_move_current_index_in_next_it = true;
                   //  parent_cmd->current_index++;
                    parent_cmd->is_inc_current_index = true;
                   // parent_cmd->current_index = cmd->current_index;
       
                }
            }

            if (cmd->is_type() && !cmd->is_or() && !parent_cmd->is_or())
            {
                if (parent_cmd->status_process.status_find != status_find_t::failed)
                    parent_cmd->status_process = cmd->status_process;

                if (cmd->is_end_find)
                {
                    show_tree fmt::print(" [");
                    show_tree fmt::print(fg(fmt::color::dark_sea_green), "pci {} -> {} for {}", parent_cmd->current_index, parent_cmd->current_index + 1, parent_cmd->value);
                    show_tree fmt::print("]");

                  //  parent_cmd->current_index++;
                    parent_cmd->is_inc_current_index = true;
                }
            }

#ifdef FIRST_CHILD_OPTIMISITION
            if (cmd->is_end_find)
            {
                if (command_graph->next) {

                   if (cmd->is_inc_current_index_parent)
                       parent_cmd->is_inc_current_index = false;
                    
                   command_graph->next->parent->get_value().is_move_current_index_in_next_it = cmd->is_inc_current_index_parent;
                   command_graph->parent->first_child = command_graph->next;
                    
                }
            }
#endif // FIRST_CHILD_OPTIMISITION

            {
                if (is_render_tree) fmt::print("\n");
            }

            if ((command_graph->is_root && cmd->is_end_find) || (command_graph->is_root && cmd->status_process.is_status_exit))
            {
                final_signature(command_graph, arg, count_signatures, is_use);
            }
        }

        void process_signature_base(gcmd_t* command_graph, base_arg_t* arg, int count_signatures, bool& is_use, bool is_render_tree, bool &is_skip_all, bool &is_skip_subsets)
        {
            cmd_t* cmd        = &command_graph->get_value();
            cmd_t* parent_cmd = &command_graph->parent->get_value();
            cmd_t* root_cmd   = &command_graph->root->get_value();

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

                        if (cmd->is_true() || cmd->is_false())
                        {
                            show_tree fmt::print(fg(fmt::color::gold), " {}", cmd->value);
                        }
                        else {

                            if (cmd->is_value())
                            {
                                fmt::print(fg(fmt::color::thistle), " \"{}\"", cmd->value);
                            }
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

                    if (cmd->is_last)
                    {
                        fmt::print(fg(fmt::color::orange_red), " <last element>");
                    }

                    if (cmd->is_breakpoint())
                    {
                        show_tree fmt::print(fg(fmt::color::red), " [breakpoint]");
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
    
            std::size_t max_position = cmd->max_position;
            std::size_t min_position = cmd->min_position;

            if (max_position < arg->region->current_position || min_position > arg->region->current_position) {

                show_tree  fmt::print(" pos: {} pci: {} ci: {}", command_graph->position, parent_cmd->current_index, cmd->current_index);

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

#ifdef NEXT_ELEMENT_SKIP_OPTIMISITION

                    if (command_graph->parent->is_root)
                    {
                        is_skip_all = true;
                    }

    #ifdef SKIP_IN_SUBSETS_NODES_OPTIMISITION
                    is_skip_subsets = true;
    #endif

#endif
                }

                end_return;
            }

            // TODO: Имеет ли проблемы ситуация с комментированием cmd->is_or() && cmd->is_end_find && ?
            if (/*cmd->is_or() && cmd->is_end_find &&*/ cmd->is_inc_current_index_parent)
            {

                show_tree fmt::print(" [");
                show_tree fmt::print(fg(fmt::color::blue_violet), "inc index");
                show_tree fmt::print("]");

                show_tree fmt::print(" [");
                show_tree fmt::print(fg(fmt::color::dark_sea_green), "pci {} -> {} for {}", parent_cmd->current_index, parent_cmd->current_index + 1, parent_cmd->value);
                show_tree fmt::print("]");

                parent_cmd->current_index++;
                cmd->is_inc_current_index_parent = false;

            }

            // TODO: Имеет ли проблемы ситуация с комментированием cmd->is_or() && cmd->is_end_find && ?
            if (/*cmd->is_or() && cmd->is_end_find &&*/ cmd->is_inc_current_index)
            {

                show_tree fmt::print(" [");
                show_tree fmt::print(fg(fmt::color::blue_violet), "inc index");
                show_tree fmt::print("]");

                show_tree fmt::print(" [");
                show_tree fmt::print(fg(fmt::color::dark_sea_green), "ci {} -> {} for {}", cmd->current_index, cmd->current_index + 1, cmd->value);
                show_tree fmt::print("]");

                cmd->current_index++;
                cmd->is_inc_current_index = false;
            }

            if (parent_cmd->is_move_current_index_in_next_it)
            {
                if (parent_cmd->is_move_current_index_in_next_it && parent_cmd->is_move_current_index_in_next_it_tmp)
                {
                    if (is_render_tree)
                    {
                        show_tree fmt::print(" [");
                        show_tree fmt::print(fg(fmt::color::blue_violet), "inc parent next");
                        show_tree fmt::print("]");

                        show_tree fmt::print(" [");
                        show_tree fmt::print(fg(fmt::color::dark_sea_green), "pci {} -> {} for {}", parent_cmd->current_index, parent_cmd->current_index + 1, parent_cmd->value);
                        show_tree fmt::print("]");
                    }

                    parent_cmd->current_index++;
                    parent_cmd->is_move_current_index_in_next_it_tmp = false;
                    parent_cmd->is_move_current_index_in_next_it = false;
                }
                else
                {
                    parent_cmd->is_move_current_index_in_next_it_tmp = true;
                }
            }


            show_tree fmt::print(" pos: {} pci: {} ci: {}", command_graph->position, parent_cmd->current_index, cmd->current_index);

            if (parent_cmd->current_index != command_graph->position && !parent_cmd->is_or() && !command_graph->is_root) {
                show_tree fmt::print(" [");
                show_tree fmt::print(fg(fmt::color::gold), "skip pos: {} != pci: {}",  command_graph->position, parent_cmd->current_index );
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

            if (cmd->is_repeat())
            {
                /*
                  Должна происходить в ноде родителе при выходе
                */
                if (cmd->repeat_element && !cmd->is_status_allocate_recursion_graph)
                {
                    std::size_t position = command_graph->position;

                    gcmd_t* node = new gcmd_t;
                       
                    emulate_recursion::copy_process_gcmd(cmd->repeat_element, node);
       
                    cmd->is_status_allocate_repeat_graph = true;

                    node->get_value().value = node->get_value().value;

                    // insert after
                    position++;
                 
                    command_graph->parent->insert(node, position);

                    show_tree fmt::print(fg(fmt::color::green_yellow), " [allocate repeat graph]");

                  //  emulate_recursion::get_position_from_parent_to_root(command_graph, position);

                    gcmd_t* command_graph_left = nullptr;

                    // recalc position
                    if (position > 0 && command_graph->root->size() > 0)
                        command_graph_left = command_graph->root->tree[position - 1];

                    if (command_graph_left)
                    {
                        auto cmd_left = &command_graph_left->get_value();
                                
                        // TODO: need fix bug this +1 and +0
                        root_cmd->min_counter = cmd_left->min_counter; //+1;
                        root_cmd->max_counter = cmd_left->max_counter; //+1;
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

                    gcmd_t* command_graph_left = nullptr;

                    // recalc position
                    if (position > 0 && command_graph->root->size() > 0)
                        command_graph_left = command_graph->root->tree[position - 1];

                    if (command_graph_left)
                    {
                        auto cmd_left = &command_graph_left->get_value();

                        // TODO: need fix bug this +1 and +0
                        root_cmd->min_counter = cmd_left->min_counter; //+1;
                        root_cmd->max_counter = cmd_left->max_counter; //+1;
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

                    //show_tree  fmt::print("\nRecalc position:\n");
                    //show_tree  emulate_recursion::print_graph(command_graph->root);

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

            if (cmd->is_true())
                status = 1;

            if (cmd->is_false())
                status = 0;

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

                    if (parent_cmd->is_autogen())
                        command_graph->parent->parent->get_value().is_status_return = true;
                }

                if (status == 1)
                {
                    show_tree fmt::print(fg(fmt::color::green_yellow), " [true]");

                    if (!parent_cmd->is_or())
                    {
                        cmd->status_process.status_find = status_find_t::success;

                        if (parent_cmd->status_process.status_find != status_find_t::failed)
                            parent_cmd->status_process = cmd->status_process;

                        //parent_cmd->current_index++;
                      //  parent_cmd->is_move_current_index_in_next_it = true;
                        parent_cmd->is_inc_current_index = true;
                        show_tree fmt::print(fg(fmt::color::dark_orchid), " [start inc next for {}]", parent_cmd->value);
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

                        parent_cmd->is_inc_current_index = true;
                        show_tree fmt::print(fg(fmt::color::dark_orchid), " [start inc next for {}]", parent_cmd->value);
                    }

                    if (parent_cmd->is_or())
                    {
                        cmd->status_process.status_find = status_find_t::failed;
                    }

                    {
                        // todo: дописал для тестов, как остановку цепочки, если она уже не верная
                        cmd->is_end_find = true;

                        if (!parent_cmd->is_or()) {

                            parent_cmd->is_end_find = true;
                        }
                    }
                }

                if (command_graph->is_last())
                {
                    cmd->is_end_find = true;

                    if (!parent_cmd->is_or()) {

                        parent_cmd->is_end_find = true;
                    }
                }

#ifdef FIRST_CHILD_OPTIMISITION
                if (cmd->is_end_find)
                {
                    if (command_graph->next)
                        command_graph->parent->first_child = command_graph->next;
                }

                // solo element, we can skip it
                if (command_graph->size() == 0)
                {
                    if (command_graph->next)
                        command_graph->parent->first_child = command_graph->next;
                }
#endif
            }

            {
                end_return;
            }
        }
		
	}
}