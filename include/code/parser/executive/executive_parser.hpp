#pragma once

#include "cmd/main_logic/real_recursion.hpp"
#include "cmd/main_logic/emulate_recursion.hpp"

#include "cmd/groups_logic/cmd_groups_make.hpp"

#include "cmd/main_logic/cmd_signature.hpp"

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

               void last_group_parent(groups::gcmd_group_t* command_graph, groups::gcmd_group_t* first_child_graph, groups::gcmd_group_t* last_child_graph, int& status, const pel::groups::group_element_t& element)
               {
                   auto cmd         = &command_graph->get_value();
                   auto parent_cmd  = &command_graph->parent->get_value();
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

                   if (parent_cmd->is_and() || parent_cmd->is_empty_operation())
                   {
                       if (
                           parent_cmd->status_process.status_find == status_find_t::unknow ||
                           parent_cmd->status_process.status_find == status_find_t::success
                          )
                       {
                           parent_cmd->status_process = cmd->status_process;
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
                   auto parent_cmd = &command_graph->parent->get_value();
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
                       cmd->status_process = parent_cmd->status_process;
                   }

                   if (cmd->is_value()) {

                       cmd->status_process = parent_cmd->status_process;

                       bool tmp_status = cmd->group.is_belongs(element);
              
                       // inversion
                       if (cmd->is_not())
                           tmp_status = !tmp_status;

                       if (tmp_status)
                            cmd->status_process.status_find = status_find_t::success;
                        else
                            cmd->status_process.status_find = status_find_t::failed;

                       if (parent_cmd->is_and() || parent_cmd->is_empty_operation())
                       {                        
                          if (parent_cmd->status_process.status_find == status_find_t::unknow || parent_cmd->status_process.status_find == status_find_t::success)
                              parent_cmd->status_process = cmd->status_process; // если это faile, то разрушит состояние success
                       }
                       
                   }

                   if (is_render_group)
                     end_return;
               }

               void process_signature_for_graph(gcmd_t* command_graph, base_arg_t* arg, int count_signatures, bool& is_use)
               {
                   if (!command_graph->root->is_process)
                       return;

                   if (!command_graph)
                       return;

                   std::size_t need_remove = 0;
                
                   if (command_graph->is_value)
                       process_signature_base(command_graph, arg, count_signatures, is_use, is_render_tree);

                   for (size_t i = 0; i < command_graph->tree.size(); i++)
                   {
                       if (command_graph->tree[i])
                       {
                           process_signature_for_graph(command_graph->tree[i], arg, count_signatures, is_use);

                           if (command_graph->is_value)
                           {
                               if (command_graph->tree[i]->is_last())
                               {
                                   last_parent(command_graph, command_graph->tree[0], command_graph->tree[i], arg, count_signatures, is_use);
                               }
                           }     
                       }
                   }
               }
    
               void emulate_recursion_for_process_signature(gcmd_t* command_graph, base_arg_t* arg, int count_signatures, bool& is_use)
               {                     
                   gcmd_t* current_graph   = command_graph->root;
                   gcmd_t* parent_iterator = nullptr;

                   std::size_t counter_level = 0; // 0 - root

                   for (size_t i = 0; i <= command_graph->root->last_index; i++)
                   {
                       /*
                          Function actions
                       */
                       if (current_graph->is_value) {

                           current_graph->level = counter_level;

                           process_signature_base(current_graph, arg, count_signatures, is_use, is_render_tree);

                           if (current_graph->is_last())
                           {
                              last_parent(current_graph->parent, current_graph->parent->size() > 0 ? current_graph->parent->tree[0] : nullptr, current_graph->parent->size() > 0 ? current_graph->parent->tree[current_graph->parent->size() - 1] : nullptr, arg, count_signatures, is_use);
                           }
                       }

                       if (current_graph->first_chield)
                       {
                           counter_level++;
                           current_graph = current_graph->first_chield;
                       } 
                       else
                       if (current_graph->next)
                       {
                           current_graph = current_graph->next;
                       }
                       else
                       {
                           parent_iterator = current_graph->parent;

                           for (;;) // infinity is heart of recursion and loop
                           {
                               if (parent_iterator->next)
                               {
                                   current_graph = parent_iterator->next;
                                   counter_level--;
                                   break;
                               }
                               else
                               {
                                   parent_iterator = parent_iterator->parent;
                               }

                               if (parent_iterator->is_root)
                                   break;
                           }
                       }
                   }
               }

               void reset_graph(gcmd_t* command_graph)
               {
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
                           real_recursion::copy_global_cmd(&global_gcmd, region->gcmd);
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
                               // here used "real" recursion
                               /* 
                               it.gcmd->process_function["base"]         = detail::bind_function(&base_parser_t::process_signature_base, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
                               it.gcmd->process_function["last"]         = detail::bind_function(&base_parser_t::last_signature,    this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
                               it.gcmd->process_function["last_parent"]  = detail::bind_function(&base_parser_t::last_parent,      this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6);

                               it.gcmd->start_process(base_arg, it.count_signatures, d->is_use);
                               */

                               it.gcmd->is_process = true;

                               // here used emulate recursion
                               emulate_recursion_for_process_signature(it.gcmd, &base_arg, it.count_signatures, d->is_use);
                             
                               // here used "real" recursion
                               // process_signature_for_graph(it.gcmd, &base_arg, it.count_signatures, d->is_use);

                               it.gcmd->is_process = false;

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
                       real_recursion::copy_global_cmd(&global_gcmd, region->gcmd);
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

                             //   it.gcmd->process_function["base"]         = detail::bind_function(&base_parser_t::process_signature_base, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
                            //    it.gcmd->process_function["last"]         = detail::bind_function(&base_parser_t::last_signature,    this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
                            //    it.gcmd->process_function["last_parent"] = detail::bind_function(&base_parser_t::last_parent,      this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6);

                            //    it.gcmd->start_process(base_arg, it.count_signatures, d->is_use);

                                it.gcmd->is_process = true;

                                process_signature_for_graph(it.gcmd, &base_arg, it.count_signatures, d->is_use);

                                it.gcmd->is_process = false;

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
                    it.gcmd->process_function["last_parent"] = detail::bind_function(&base_parser_t::last_group_parent, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);
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