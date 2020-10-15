#pragma once

#include "cmd/main_logic/cmd_make.hpp"
#include "cmd/groups_logic/cmd_groups_make.hpp"

//#define timers
#define print_timers false
#define end_return  { show_tree fmt::print("\n");   return; }


#ifdef timers
#define start_timer(name)   std::chrono::high_resolution_clock::time_point timer_##name = std::chrono::high_resolution_clock::now();
#define end_timer(name) 	std::chrono::high_resolution_clock::time_point end_##name = std::chrono::high_resolution_clock::now(); std::chrono::duration<double, std::micro> result_##name =  end_##name - timer_##name; if (print_timers) {	print(fg(fmt::color::azure), "\nProcess parse {} end: ", #name); print(fg(fmt::color::coral), "{}", result_##name.count()); 		print(fg(fmt::color::azure), "us\n"); }
#else
#define start_timer(name)
#define end_timer(name)
#endif

namespace parser
{
    namespace executive
    {
        struct data_block_t
        {
            global_gcmd_t *gcmd      = nullptr;
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

            pel::groups::position_element_t *multi_words;

            words_base_t* element;
            data_block_t* region;
            tree_words_t* word;
        };

        class base_parser_t
        {
           public:
               block_depth_t<data_block_t> block_depth;

               base_arg_t                  base_arg;
               global_gcmd_t               global_gcmd;
               recursion_gcmd_t            recursion_gcmd;

               groups::global_gcmd_group_t global_gcmd_group;

               bool is_render_tree = false;
               bool is_render_group = false;

               base_parser_t() {  }

               void last_group_parrent(groups::gcmd_group_t* command_graph, groups::gcmd_group_t* first_child_graph, groups::gcmd_group_t* last_child_graph, int& status, const pel::groups::group_element_t& element)
               {
                   auto cmd         = &command_graph->get_value();
                   auto parrent_cmd = &command_graph->parent->get_value();
                   auto root_cmd    = &command_graph->root->get_value();

                   if (cmd->is_or())
                   {
                       status_find_t tmp = status_find_t::unknow;

                       for (std::size_t i = 0; i < command_graph->tree.size(); i++)
                       {
                           auto current_cmd = &command_graph->tree[i]->get_value();

                           if (current_cmd->status_process.status_find == status_find_t::success)
                           {
                               tmp = status_find_t::success;
                           }
                       }

                       if (tmp == status_find_t::unknow)
                       {
                           tmp = status_find_t::failed;
                       }

                       cmd->status_process.status_find = tmp;
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

                   if (parrent_cmd->is_and() || parrent_cmd->is_empty_operation())
                   {
                       if (
                           parrent_cmd->status_process.status_find == status_find_t::unknow || 
                           parrent_cmd->status_process.status_find == status_find_t::success
                          )
                       {
                           parrent_cmd->status_process = cmd->status_process;
                       }                     
                   }

                   if (command_graph->is_root)
                   {
                       if (cmd->status_process.status_find == status_find_t::success)
                           status = true;

                       if (cmd->status_process.status_find == status_find_t::failed)
                           status = false;

                       if (cmd->status_process.status_find == status_find_t::unknow)
                           status = false;

                       // reset
                       cmd->status_process.status_find = status_find_t::unknow;
                   }
               }

               void process_group_signature(groups::gcmd_group_t* command_graph, int& status, const pel::groups::group_element_t& element)
               {
                   auto cmd         = &command_graph->get_value();
                   auto parrent_cmd = &command_graph->parent->get_value();
                   auto root_cmd    = &command_graph->root->get_value();

                   if (is_render_tree && is_render_group) {

                       if (command_graph->is_root)
                       {
                           fmt::print("\n");
                           fmt::print("Value: {}", (char)element.element);
                           fmt::print("\n\n");
                       }

                       if (!cmd->group.name.empty() || cmd->group.size() > 0) {

                           groups::print_space_cmd_group(command_graph->level, command_graph->is_have_sub_elemets(), cmd);

                           if (command_graph->is_root)
                           {
                               fmt::print(fg(fmt::color::coral), " {}", cmd->group.name);
                           }
                           else
                           {
                               if (cmd->is_type())
                               {
                                   fmt::print(fg(fmt::color::blanched_almond), " {}", cmd->group.name);
                               }
                               else
                               {
                                   fmt::print("{}", " {");

                                   for (size_t i = 0; i < cmd->group.get().size() - 1; i++)
                                   {
                                       fmt::print(fg(fmt::color::thistle), "'{}'", (char)cmd->group.get()[i].element);
                                       fmt::print(", ");
                                   }

                                   fmt::print(fg(fmt::color::thistle), "'{}'", (char)cmd->group.get()[cmd->group.get().size() - 1].element);

                                   fmt::print("{}", "};");
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

                   if (cmd->is_type()) {
                       cmd->status_process = parrent_cmd->status_process;
                   }

                   if (cmd->is_value()) {

                       cmd->status_process = parrent_cmd->status_process;

                       bool tmp_status = cmd->group.is_belongs(element);
              
                       // inversion
                       if (cmd->is_not())
                           tmp_status = !tmp_status;

                       if (tmp_status)
                            cmd->status_process.status_find = status_find_t::success;
                        else
                            cmd->status_process.status_find = status_find_t::failed;

                       if (parrent_cmd->is_and() || parrent_cmd->is_empty_operation())
                       {                        
                          if (parrent_cmd->status_process.status_find == status_find_t::unknow || parrent_cmd->status_process.status_find == status_find_t::success)
                             parrent_cmd->status_process = cmd->status_process; // если это faile, то разрушит состояние success
                       }
                       
                   }

                   if (is_render_group)
                     end_return;
               }

               void init_recursive_function()
               {
                  // global_graph.process_function["base"] = detail::bind_function(&base_parser_t::process_signature, this, std::placeholders::_1, std::placeholders::_2);
               }

               void final_signature(gcmd_t* command_graph, base_arg_t* arg, int count_signaturs, bool& is_use)
               {
                   cmd_t* cmd         = &command_graph->get_value();
                   cmd_t* parrent_cmd = &command_graph->parent->get_value();
                   cmd_t* root_cmd    = &command_graph->root->get_value();

            //	   if (command_graph->get_value().is_last)
                   {
                       if (cmd->status_process.status_find == status_find_t::success)
                       {              
                           if (arg->format_word == format_word_t::one_word)
                           {
                               fmt::print(fg(fmt::color::lawn_green), "\nLine: {3} - its signature: {0} [count op: {1}, total op: {2}]", root_cmd->value, root_cmd->count_operation, total_operation, arg->element->number_line);
                              

                           }
                           else if (arg->format_word == format_word_t::multi_word)
                           {
                               fmt::print(fg(fmt::color::lawn_green), "\nIts signature: {0} [count op: {1}, total op: {2}]", root_cmd->value, root_cmd->count_operation, total_operation);                        
                           }

                           arg->region->is_status_find = true;
                       }
                       else if (cmd->status_process.status_find == status_find_t::failed)
                       {
                           if (arg->format_word == format_word_t::one_word)
                           {
                               fmt::print(fg(fmt::color::indian_red), "\nLine: {4} - its not signature {0}: {1}[{5}:{6}] [count op: {2}, total op: {3}",
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
                               fmt::print(fg(fmt::color::indian_red), "\nIts not signature {0}: {1} total operaion: {2}",
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
                               fmt::print(fg(fmt::color::burly_wood), "\nIts maybe signature {0}: {1} total operaion: {2}",
                                   root_cmd->value.c_str(),
                                   root_cmd->count_operation,
                                   total_operation
                               );
                           }

                       if (command_graph->get_value().status_process.is_status_exit)
                           fmt::print(fg(fmt::color::gold), " [chain reached exit]");

                       fmt::print("\n");

                       is_use = false;
                       command_graph->stop_process();
                   }
               }

               void last_signature(gcmd_t* command_graph, base_arg_t* arg, int count_signaturs, bool& is_use)
               {
                   cmd_t* cmd         = &command_graph->get_value();
                   cmd_t* parrent_cmd = &command_graph->parent->get_value();
                   cmd_t* root_cmd    = &command_graph->root->get_value();
               }

               void last_parrent(gcmd_t* command_graph, gcmd_t* first_child_graph, gcmd_t* last_child_graph, base_arg_t* arg, int count_signaturs, bool& is_use)
               {
                   cmd_t* cmd         = &command_graph->get_value();
                   cmd_t* parrent_cmd = &command_graph->parent->get_value();
                   cmd_t* root_cmd    = &command_graph->root->get_value();

                   if (cmd->max_position < arg->region->current_position || cmd->min_position > arg->region->current_position)
                       return;

                   if (cmd->is_type() && command_graph->is_last() && !parrent_cmd->is_or()) {
                        parrent_cmd->is_end_find = cmd->is_end_find;
                   }

                   /*
                      Решается ли это без перебора? Это линейно, но меня жутко раздражает, что надо каждую суб-ор вершину опрашивать.
                      Другого алгоритма я не нашел.
                   */
                   if (cmd->is_type() && cmd->is_or() && !cmd->is_finaly_or)
                   {			  
                       status_find_t tmp_status_find = status_find_t::failed;
          
                       bool is_have_not_checked       = false;
                       bool is_all_status_end_checked = false;
                       bool is_have_success           = false;

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
              
                           cmd->is_end_find  = true;
                           cmd->is_finaly_or = true;

                           cmd->status_process.status_find = tmp_status_find;

                           if (!parrent_cmd->is_or())
                           {
                               if (command_graph->is_last())
                                   parrent_cmd->is_end_find = true;
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
                       parrent_cmd->is_status_return = true;
                   }

                   if (cmd->is_exit() && cmd->status_process.status_find == status_find_t::success)
                   {
                       cmd->status_process.is_status_exit = true;
                   }

                   if (cmd->is_maybe() && cmd->status_process.status_find != status_find_t::unknow)
                   {
                       cmd->status_process.status_find = status_find_t::success;
                   }

                   if (cmd->is_type() && cmd->is_or() && parrent_cmd->is_or())
                   {
                       // ?
                   }

                   if (cmd->is_type() && cmd->is_or() && !parrent_cmd->is_or())
                   {
                       if (parrent_cmd->status_process.status_find != status_find_t::failed)
                           parrent_cmd->status_process = cmd->status_process;

                       if (cmd->is_end_find)
                       {
                           //parrent_cmd->current_index++;
                           parrent_cmd->current_index = cmd->current_index;
                       }
                   }

                   if (cmd->is_type() && !cmd->is_or() && !parrent_cmd->is_or())
                   {
                       if (parrent_cmd->status_process.status_find != status_find_t::failed)
                           parrent_cmd->status_process = cmd->status_process;

                       if (cmd->is_end_find)
                       {
                           parrent_cmd->current_index++;
                       }               
                   }

                   if ((command_graph->is_root && cmd->is_end_find) || (command_graph->is_root && cmd->status_process.is_status_exit))
                   {
                       final_signature(command_graph, arg, count_signaturs, is_use);
                   }
               }

               int total_operation = 0;


               std::size_t recursion_offset_min = 0;
               std::size_t recursion_offset_max = 0;

               void process_signature(gcmd_t* command_graph, base_arg_t* arg, int count_signaturs, bool& is_use)
               {
                   start_timer(process_signature);

                   cmd_t* cmd         = &command_graph->get_value();
                   cmd_t* parrent_cmd = &command_graph->parent->get_value();
                   cmd_t* root_cmd    = &command_graph->root->get_value();

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

                   if (cmd->status_process.is_status_exit || parrent_cmd->status_process.is_status_exit)
                   {
                       show_tree fmt::print(" [");
                       show_tree fmt::print(fg(fmt::color::alice_blue), "status exit");
                       show_tree fmt::print("]");

                       end_return;
                   }

                   if (cmd->is_status_return || parrent_cmd->is_status_return)
                   {
                       if (command_graph->is_last())
                       {
                           cmd->is_end_find = true;

                           if (!parrent_cmd->is_or()) {

                               parrent_cmd->is_end_find = true;
                           }
                       }

                       show_tree fmt::print(" [");
                       show_tree fmt::print(fg(fmt::color::blue_violet), "status return");
                       show_tree fmt::print("]");

                       end_return;
                   }
         
                   if (cmd->is_or() && cmd->is_end_find && cmd->is_inc_current_index)
                   {
                       parrent_cmd->current_index++;
                       cmd->is_inc_current_index = false;
                   }

                   std::size_t max_position = cmd->max_position + recursion_offset_max;
                   std::size_t min_position = cmd->min_position + recursion_offset_min;

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

                    
                       end_timer(process_signature);
                       end_return;
                   }

                   if (parrent_cmd->current_index != command_graph->position && !parrent_cmd->is_or()) {
                       show_tree fmt::print(" [");
                       show_tree fmt::print(fg(fmt::color::gold), "skip current index {0} graph pos {1}", cmd->current_index, command_graph->position);
                       show_tree fmt::print("]");
                       end_return;
                   }

                   if (cmd->is_check && cmd->is_type()) {
                       end_timer(process_signature);
                       end_return;
                   }

                   if (cmd->is_check) {
                       end_timer(process_signature);
                       end_return;
                   }

                   if (cmd->is_type())
                   {
                       if (cmd->is_or())
                       {
                           cmd->status_process.status_find = status_find_t::unknow;

                       } else 
                       {
                           if (!parrent_cmd->is_or())
                            cmd->status_process = parrent_cmd->status_process;
                       }

                       if (parrent_cmd->is_or() && parrent_cmd->status_process.status_find != status_find_t::success)
                       {
                           parrent_cmd->status_process.status_find = status_find_t::unknow;
                       }
                   }

                   // time for epic
                   if (cmd->is_recursion())
                   {
                       if (cmd->recursion_element && !cmd->is_status_allocate_recursion_graph)
                       {
                           // allocate graph
                           copy_process_gcmd(cmd->recursion_element, command_graph);

                           cmd->is_status_allocate_recursion_graph = true;

                           show_tree fmt::print(fg(fmt::color::green_yellow), " [allocate recursion graph]");

                           recursion_offset_max += cmd->max_position + 1;
                           recursion_offset_min += cmd->min_position + 1;                   
                       } 
                   }

                   int status = 0;

                   if (cmd->is_group() || cmd->is_value()) {

                       cmd->status_process = parrent_cmd->status_process;

                       cmd->is_check = true;

                       if (command_graph->is_last())
                       {
                           parrent_cmd->is_check = true;
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
                           parrent_cmd->status_process.is_status_exit = true;
                       }

                       if (cmd->is_status_return)
                       {
                           parrent_cmd->is_status_return = true;
                       }

                       if (status == 1)
                       {
                           show_tree fmt::print(fg(fmt::color::green_yellow), " [true]");

                           if (!parrent_cmd->is_or())
                           {
                               cmd->status_process.status_find = status_find_t::success;

                               if (parrent_cmd->status_process.status_find != status_find_t::failed)
                                   parrent_cmd->status_process = cmd->status_process;

                               parrent_cmd->current_index++;
                           }

                           if (parrent_cmd->is_or())
                           {
                               cmd->status_process.status_find = status_find_t::success;
                               cmd->is_end_find = true;
                           }							   
                       }
                       else
                       {
                           show_tree fmt::print(fg(fmt::color::pale_violet_red), " [false]");

                           if (!parrent_cmd->is_or())
                           {
                               cmd->status_process.status_find = status_find_t::failed;

                               parrent_cmd->status_process = cmd->status_process;

                               parrent_cmd->current_index++;
                           }

                           if (parrent_cmd->is_or())
                           {		   
                               cmd->status_process.status_find = status_find_t::failed;
                           }
                       }

                       if (command_graph->is_last())
                       {
                           cmd->is_end_find = true;	  
                        
                           if (!parrent_cmd->is_or()) {
                         
                               parrent_cmd->is_end_find = true;
                           }
                       }
                   }	

                   {
                       end_timer(process_signature);
                       end_return;
                   }
               }

               void reset_graph(gcmd_t* command_graph)
               {
                   // TODO: Remove
                  recursion_offset_min = 0;
                  recursion_offset_max = 0;

                   command_graph->get_value().reset();
               }

               void reset(int level, base_arg_t* arg)
               {
                   global_gcmd_t* gcmd = arg->region->gcmd;

                   for (auto& it : *gcmd)
                   {
                       it.gcmd->process_function["reset"] = detail::bind_function(&base_parser_t::reset_graph, this, std::placeholders::_1);
                       it.gcmd->start_process_for("reset", "empty");

                       data_block_global_gcmd_t* d = it.block_depth.get_block(level);
                       d->is_use = true;
                   }

                   arg->region->current_position = 0;
               }

               void delete_gcmd()
               {
                   for (size_t i = 0; i < block_depth.block.size(); i++)
                   {
                       global_gcmd_t* gcmd = block_depth.block[i].gcmd;

                       for (auto &it: *gcmd)
                       {
                           it.block_depth.delete_alloc();
                           it.gcmd->delete_tree();
                           delete it.gcmd;
                       }

                       delete block_depth.block[i].gcmd;
                   }
               }

               void delete_global_gcmd()
               {
                   for (auto &it : global_gcmd)
                   {
                       it.block_depth.delete_alloc();
                       it.gcmd->delete_tree();
                       delete it.gcmd;
                   }

                   std::clear(global_gcmd);

                   for (auto& it : global_gcmd_group)
                   {                 
                       it.gcmd->delete_tree();
                       delete it.gcmd;
                   }

                   std::clear(global_gcmd_group);


                   for (auto& it : recursion_gcmd)
                   {
                       it.block_depth.delete_alloc();
                       it.gcmd->delete_tree();
                       delete it.gcmd;
                   }

                   std::clear(recursion_gcmd);
               }

               void process_executive_array_words(pel::groups::array_words_t& array_words, const std::size_t &level)
               {
                   if (is_render_group) {

                       fmt::print("\n");

                       for (auto& it : array_words.data)
                       {
                           for (auto& sub : it.words)
                           {
                               int count_space = 1;

                               if (it.position < 1000)
                                   count_space = 1;

                               if (it.position < 100)
                                   count_space = 2;

                               if (it.position < 10)
                                   count_space = 3;

                               fmt::print(fmt::fg(fmt::color::dark_cyan), "{}", (char)221);

                               if (count_space == 3)
                                   fmt::print(fmt::fg(fmt::color::lime_green), " {} ", it.position);

                               if (count_space == 2)
                                   fmt::print(fmt::fg(fmt::color::lime_green), "{} ", it.position);

                               if (count_space == 1)
                                   fmt::print(fmt::fg(fmt::color::lime_green), "{} ", it.position);

                               fmt::print(fmt::fg(fmt::color::thistle), "\"{0}\"", sub.data);
                               fmt::print(fmt::fg(fmt::color::red), ": ", sub.data);
                               fmt::print(fmt::fg(fmt::color::blanched_almond), "{0}", sub.group->name);
                               fmt::print(fmt::fg(fmt::color::white), ";\n");
                           }
                       }
                   }

                   for (size_t position = 0; position < array_words.data.size(); position++)
                   {
                       data_block_t* region = block_depth.get_block(level);

                       if (!region->gcmd)
                       {
                           region->gcmd = new global_gcmd_t;
                           copy_global_cmd(&global_gcmd, region->gcmd);
                       }

                       base_arg.region = region;
                       base_arg.multi_words = &array_words.data[position];
                       base_arg.format_word = format_word_t::multi_word;

                       global_gcmd_t* gcmd = base_arg.region->gcmd;

                       for (auto& it : *gcmd)
                       {
                           data_block_global_gcmd_t* d = it.block_depth.get_block(level);
                           base_arg.region->is_status_find = false;

                           //if (d->is_use)
                           {
                               it.gcmd->process_function["base"]         = detail::bind_function(&base_parser_t::process_signature, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
                               it.gcmd->process_function["last"]         = detail::bind_function(&base_parser_t::last_signature,    this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
                               it.gcmd->process_function["last_parrent"] = detail::bind_function(&base_parser_t::last_parrent,      this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6);

                               it.gcmd->start_process(base_arg, it.count_signaturs, d->is_use);

                               if (!d->is_use)
                               {
                                   region->count_not_signature++;
                               }

                               if (base_arg.region->is_status_find)
                               {
                             //      break;
                               }
                           }
                       }

                       if (region->count_not_signature >= gcmd->size() || base_arg.region->is_status_find)
                       {
                           region->count_not_signature = 0;

                           reset(level, &base_arg);
                       }
                       else
                       {
                           if (!base_arg.region->is_status_find)
                               base_arg.region->current_position++;
                       }
                   }
               }

               void process_executive_tree(tree_words_t *word)
               {
                   if (!word)
                       return;

                   start_timer(region);

                   data_block_t* region = block_depth.get_block(word->level);

                   if (!region->gcmd)
                   {
                       region->gcmd = new global_gcmd_t;
                       copy_global_cmd(&global_gcmd, region->gcmd);
                   }

                   base_arg.region  = region;
                   base_arg.element = word->get_value();
                   base_arg.word    = word;

                   end_timer(region);
               
                   if (word->get_value()->is_symbol() || word->get_value()->is_word()) {

                       global_gcmd_t* gcmd = base_arg.region->gcmd;

                       int w = 0;
            
                       start_timer(iteration_gcmd);

                       for (auto& it : *gcmd)
                       {
                           data_block_global_gcmd_t *d = it.block_depth.get_block(word->level);
                           base_arg.region->is_status_find = false;

                           if (d->is_use)
                           {
                               //fmt::print("Graph: %s\n", it.gcmd->root->get_value().value.c_str());

                                it.gcmd->process_function["base"]         = detail::bind_function(&base_parser_t::process_signature, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
                                it.gcmd->process_function["last"]         = detail::bind_function(&base_parser_t::last_signature,    this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
                                it.gcmd->process_function["last_parrent"] = detail::bind_function(&base_parser_t::last_parrent,      this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6);

                                it.gcmd->start_process(base_arg, it.count_signaturs, d->is_use);

                                if (!d->is_use)
                                {
                                    region->count_not_signature++;
                                }

                                if (base_arg.region->is_status_find)
                                {
                                    break;
                                }
                           }
                       }

                       end_timer(iteration_gcmd);

                       if (region->count_not_signature >= gcmd->size() || base_arg.region->is_status_find)
                       {		  
                           //fmt::print("[error 1] No signature for: %s\n", base_arg.element->data.c_str());

                           region->count_not_signature = 0;

                           reset(word->level, &base_arg);                   
                       }
                       else
                       {
                           if (!base_arg.region->is_status_find)
                               base_arg.region->current_position++;
                       }
                   } 
               }
        };
    }
}

namespace parser
{
    namespace executive
    {
        class parser_engine_t : public parser::executive::base_parser_t, public parser::block_parser_t, public parser::words_parser_t
        {
        public:
            std::string               code;
            words_t                   words;
            tree_words_t              tree;
            int32_t                   level = -1;

            // no launch delete_global_gcmd()
            void delete_alloc()
            {
                delete_gcmd();
                block_depth.delete_alloc();
                
                tree.delete_tree();
                words.delete_alloc();
            }

            void process_parse_pel_to_words()
            {
                parser::words_parser_t::process_parse_pel_to_words(code, words);
            }

            void group_init() {

                for (auto& it : global_gcmd_group)
                {
                    it.gcmd->process_function["base"] = detail::bind_function(&base_parser_t::process_group_signature, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
                    it.gcmd->process_function["last_parrent"] = detail::bind_function(&base_parser_t::last_group_parrent, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);
                }
            }

            void group_parse(const pel::groups::group_element_t &element, pel::groups::group_result_t &result) {

                for (auto &it: global_gcmd_group)
                {
                    int status = 0;

                    it.gcmd->start_process(status, element);

                    // То граф принадлежит
                    if (status == 1)
                    {
                        result.push_group(&it.gcmd->get_value().group);
                    }
                }
                
                if (is_render_tree && is_render_group)
                    fmt::print("\n");        
            }

            void parse_tree()
            {
                tree.process_function["base"] = detail::bind_function(&base_parser_t::process_executive_tree, this, std::placeholders::_1);
                tree.start_process();
            }	
        };
    }
}