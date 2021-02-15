#include "..\..\..\..\..\code\detail\fmt\fmt\printf.h"
#include "..\..\..\..\..\code\detail\fmt\fmt\color.h"

#include "..\..\..\..\..\code\block_parser.hpp"
#include "..\..\..\..\..\code\parser\executive\cmd\base\cmd_base.hpp"

namespace parser
{
	namespace executive
	{
		enum quantum_flag_t : std::flag32_t
		{
			quantum_value	   = 1 << 0,
			quantum_type	   = 1 << 1,
			quantum_execute    = 1 << 2,
			quantum_block_type = 1 << 3,
			quantum_and		   = 1 << 4,
			quantum_or		   = 1 << 5,
			quantum_not		   = 1 << 6,
			empty_operation    = 1 << 7,
			quantum_group	   = 1 << 8,
			quantum_repeat	   = 1 << 9,
			quantum_maybe	   = 1 << 10,
			quantum_return	   = 1 << 11,
			quantum_exit	   = 1 << 12,
			quantum_recursion  = 1 << 13,
			quantum_autogen    = 1 << 14,
			quantum_breakpoint = 1 << 15, // for debug from c++
			quantum_repeat_end = 1 << 16, 
			quantum_true       = 1 << 17,
			quantum_false      = 1 << 18,
			quantum_global     = 1 << 19,
			quantum_local      = 1 << 20,
			quantum_break      = 1 << 21,
			quantum_break_now  = 1 << 22,
			quantum_break_all  = 1 << 23,
		};

		template<typename block_depth_base_t>
		class block_depth_t
		{		  
		  public:
			std::vector<block_depth_base_t> block;

			void delete_alloc()
			{
				std::clear(block);
			}

			block_depth_base_t* get_block(std::size_t level /*copy!*/) {

				if (level > 0)
					level--;

				// Нельзя убрать
				if (level < 0)
					level = 0;

				if (block.size() > level)
					return &block[level];

				std::size_t delta = level - block.size();

				block_depth_base_t null;

				if (delta == 0)
				{
					block.push_back(null);
					return &block[level];
				}

				delta += 1;

				for (size_t i = 0; i < delta; i++)
					block.push_back(null);

				return &block[level];
			}
		};

		struct region_data_t
		{
			bool is_signature = false;
			bool is_use = true;
			int  current_position = 0;
		};

		struct cmd_t;
	
		using gcmd_t = tree_t<cmd_t>;

		struct cmd_t
		{
			cmd_t() {
				
			}

			cmd_t(std::flag32_t f) { flag = f; }

			cmd_t(const std::string& v)
			{
				value = v;

				if (!std::check_flag(flag, quantum_value))
					std::add_flag(flag, quantum_value);
			}

			cmd_t(const std::string& v, std::flag32_t f) {

				value = v;
				flag  = f;

				if (!std::check_flag(flag, quantum_value) && !std::check_flag(flag, quantum_type) && !value.empty())
					std::add_flag(flag, quantum_value);
			}

			cmd_t(std::flag32_t f, const std::string& p) {

				print = p;
				flag  = f;

			
			}

			inline bool is_or()        { return std::check_flag(flag, quantum_or);        }
			inline bool is_and()       { return std::check_flag(flag, quantum_and);       }
			inline bool is_not()       { return std::check_flag(flag, quantum_not);       }
			inline bool is_value()     { return std::check_flag(flag, quantum_value);     }
			inline bool is_type()      { return std::check_flag(flag, quantum_type);      }
			inline bool is_execute()   { return std::check_flag(flag, quantum_execute);   }
			inline bool is_group()	   { return std::check_flag(flag, quantum_group);     }
			inline bool is_repeat()	   { return std::check_flag(flag, quantum_repeat);    }
			inline bool is_maybe()	   { return std::check_flag(flag, quantum_maybe);     }
			inline bool is_return()	   { return std::check_flag(flag, quantum_return);    }
			inline bool is_exit()	   { return std::check_flag(flag, quantum_exit);      }
			inline bool is_recursion() { return std::check_flag(flag, quantum_recursion); }
			inline bool is_autogen()   { return std::check_flag(flag, quantum_autogen);   }
			inline bool is_breakpoint(){ return std::check_flag(flag, quantum_breakpoint);}
			inline bool is_repeat_end(){ return std::check_flag(flag, quantum_repeat_end);}
			inline bool is_true()      { return std::check_flag(flag, quantum_true);      }
			inline bool is_false()     { return std::check_flag(flag, quantum_false);     }

			inline bool is_empty_operation() { return std::check_flag(flag, empty_operation); }

			std::string   value;  // +44 bytes in debug
			
			std::flag32_t flag = 0;		
			std::string   print;  // +44 bytes in debug

			pel::groups::group_t* group  = nullptr;

			std::size_t min_position = SIZE_MAX;
			std::size_t max_position = 0;

			// counter logic 16 byte, sad
			// Мы можем сделать тип cmd_base_t и в нём будет счетчик, после сделать тип cmd_t
			// в нём уже счетчик вырезать, будет -16 байт
			std::size_t min_counter = 0;
			std::size_t max_counter = 0;

			// Еще один позиционный счетчик
			std::size_t current_index = 0;

			// TODO: delete in release?
			std::size_t count_operation = 0;

			// 8 byte + 1 (+7) ?! its can just 1 byte std::flag8_t
			status_process_t status_process;

			bool is_status_return    = false;

			// TODO: reset state?
			bool is_status_allocate_recursion_graph = false;
			bool is_status_allocate_repeat_graph = false;

			gcmd_t* recursion_element = nullptr;
			gcmd_t* repeat_element    = nullptr;

			// used for recursion, recursion rebuilds positions
			void reinit() {
				min_position = SIZE_MAX;
			}

			void reset() {

				status_process.status_find    = status_find_t::unknow;
				status_process.is_status_exit = false;

				is_check   = false;
		
				count_operation      = 0;
		
				is_end_find			 = false;
				current_index        = 0;
				is_inc_current_index = false;

				is_finaly_or     = false;
				is_status_return = false;

				is_move_current_index_in_next_it     = false;	
				is_move_current_index_in_next_it_tmp = false;

				is_inc_current_index_parent = false;
			}

			// cast in std::flag8_t ?
			bool is_end_find          = false;
			bool is_inc_current_index = false;
			bool is_inc_current_index_parent = false;
			bool is_check             = false;
			bool is_finaly_or		  = false;
			bool is_last			  = false;

			bool is_move_current_index_in_next_it     = false;
			bool is_move_current_index_in_next_it_tmp = false;
		};


	}
}