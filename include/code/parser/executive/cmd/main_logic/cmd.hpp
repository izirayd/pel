#include "..\..\..\..\..\code\detail\fmt\fmt\printf.h"
#include "..\..\..\..\..\code\detail\fmt\fmt\color.h"

#include "..\..\..\..\..\code\block_parser.hpp"
#include "..\..\..\..\..\code\parser\executive\cmd\base\cmd_base.hpp"

namespace parser
{
	namespace executive
	{
		enum parser_flag_t : std::flag32_t
		{
			parser_value	  = 1 << 0,
			parser_any_word   = 1 << 1, // �� ���������
			parser_any		  = 1 << 2, // �� ���������
			parser_print	  = 1 << 3, // �� ��������� ?
			parser_open		  = 1 << 4,
			parser_close	  = 1 << 5, 
			parser_type		  = 1 << 6,
			parser_execute    = 1 << 7,
			parser_block_type = 1 << 8,
			// ��� ���������� ��������, ��� ������ �� �����, ���-�� ������������� or, 
			// ��������� ��� ������������, ���������� � ������ ��������
			// TODO: �� ���������
			parser_tree_or    = 1 << 9,  // �� ���������
			parser_and		  = 1 << 10,
			parser_or		  = 1 << 11,
			parser_xor	      = 1 << 12, // �� ���������
			parser_not		  = 1 << 13,
			parser_tree_xor   = 1 << 14, // �� ���������
			empty_operation   = 1 << 15,
			parser_group	  = 1 << 16,
			parser_repeat	  = 1 << 17,
			parser_maybe	  = 1 << 18,
			parser_return	  = 1 << 19,
			parser_exit		  = 1 << 20,
			parser_recursion  = 1 << 21,
			parser_autogen    = 1 << 22,
			parser_breakpoint = 1 << 23, // for debug from c++
			parser_repeat_end = 1 << 24
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

				// ������ ������
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

				if (!std::check_flag(flag, parser_value))
					std::add_flag(flag, parser_value);
			}

			cmd_t(const std::string& v, std::flag32_t f) {

				value = v;
				flag  = f;

				if (!std::check_flag(flag, parser_value) && !std::check_flag(flag, parser_type) && !value.empty())
					std::add_flag(flag, parser_value);
			}

			cmd_t(std::flag32_t f, const std::string& p) {

				print = p;
				flag  = f;

				if (!std::check_flag(flag, parser_print) && !print.empty())
					std::add_flag(flag, parser_print);
			}

			cmd_t(const std::string& v, std::flag32_t f, const std::string& p)
			{
				print = p;
				value = v;
				flag  = f;

				if (!std::check_flag(flag, parser_value) && !std::check_flag(flag, parser_type) && !value.empty())
					std::add_flag(flag, parser_value);

				if (!std::check_flag(flag, parser_print) && !print.empty())
					std::add_flag(flag, parser_print);
			}

			cmd_t(const std::string& v, const std::string& p)
			{
				print = p;
				value = v;

				if (!std::check_flag(flag, parser_value) && !value.empty() && !std::check_flag(flag, parser_type))
					std::add_flag(flag, parser_value);

				if (!std::check_flag(flag, parser_print) && !print.empty())
					std::add_flag(flag, parser_print);
			}

			inline bool is_or()        { return std::check_flag(flag, parser_or);        }
			inline bool is_and()       { return std::check_flag(flag, parser_and);       }
			inline bool is_xor()       { return std::check_flag(flag, parser_xor);       }
			inline bool is_not()       { return std::check_flag(flag, parser_not);       }
			inline bool is_value()     { return std::check_flag(flag, parser_value);     }
			inline bool is_type()      { return std::check_flag(flag, parser_type);      }
			inline bool is_tree_or()   { return std::check_flag(flag, parser_tree_or);   }
			inline bool is_tree_xor()  { return std::check_flag(flag, parser_tree_xor);  }
			inline bool is_execute()   { return std::check_flag(flag, parser_execute);   }
			inline bool is_group()	   { return std::check_flag(flag, parser_group);     }
			inline bool is_repeat()	   { return std::check_flag(flag, parser_repeat);    }
			inline bool is_maybe()	   { return std::check_flag(flag, parser_maybe);     }
			inline bool is_return()	   { return std::check_flag(flag, parser_return);    }
			inline bool is_exit()	   { return std::check_flag(flag, parser_exit);      }
			inline bool is_recursion() { return std::check_flag(flag, parser_recursion); }
			inline bool is_autogen()   { return std::check_flag(flag, parser_autogen);   }
			inline bool is_breakpoint(){ return std::check_flag(flag, parser_breakpoint);}
			inline bool is_repeat_end(){ return std::check_flag(flag, parser_repeat_end);}

			inline bool is_empty_operation() { return std::check_flag(flag, empty_operation); }

			std::string   value;  // +44 bytes in debug
			
			std::flag32_t flag = 0;		
			std::string   print;  // +44 bytes in debug

			pel::groups::group_t* group  = nullptr;

			std::size_t min_position = SIZE_MAX;
			std::size_t max_position = 0;

			// counter logic 16 byte, sad
			// �� ����� ������� ��� cmd_base_t � � ��� ����� �������, ����� ������� ��� cmd_t
			// � ��� ��� ������� ��������, ����� -16 ����
			std::size_t min_counter = 0;
			std::size_t max_counter = 0;

			// ��� ���� ����������� �������
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