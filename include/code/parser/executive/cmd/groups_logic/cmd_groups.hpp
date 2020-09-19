#pragma once

#include "..\..\..\..\..\code\block_parser.hpp"

#include "..\..\..\..\..\code\parser\executive\cmd\groups_logic\groups.hpp"
#include "..\..\..\..\..\code\obj.hpp"

#include "..\..\..\..\..\code\parser\executive\cmd\base\cmd_base.hpp"

namespace parser
{
	namespace executive
	{
		namespace groups
		{	
			enum class group_flag_t : std::flag32_t
			{
				object_value      = 1 << 0,
				property_ex       = 1 << 1,
				operation_and     = 1 << 2,
				operation_or      = 1 << 3,
				operation_not     = 1 << 4,
				object_type       = 1 << 5,
				operation_empty   = 1 << 6,
				property_glue     = 1 << 7,
				property_split    = 1 << 8,
				property_ignore   = 1 << 9,

			};

			struct cmd_group_t
			{
			   public:
				std::flag32_t		 flag = 0;
				pel::groups::group_t group;

				std::size_t min_position = SIZE_MAX;
				std::size_t max_position = 0;

				std::size_t min_counter = 0;
				std::size_t max_counter = 0;

				std::size_t current_index = 0;

				bool is_last = false;

				status_process_t status_process;

				inline bool is_or()     { return std::check_flag(flag, group_flag_t::operation_or);   }
				inline bool is_and()    { return std::check_flag(flag, group_flag_t::operation_and);  }
				inline bool is_ex()     { return std::check_flag(flag, group_flag_t::property_ex);    }
				inline bool is_not()    { return std::check_flag(flag, group_flag_t::operation_not);  }
				inline bool is_value()  { return std::check_flag(flag, group_flag_t::object_value);   }
				inline bool is_type()   { return std::check_flag(flag, group_flag_t::object_type);    }
				inline bool is_glue()   { return std::check_flag(flag, group_flag_t::property_glue);  }
				inline bool is_split()  { return std::check_flag(flag, group_flag_t::property_split); }
				inline bool is_ignore()  { return std::check_flag(flag, group_flag_t::property_ignore); }

				inline bool is_empty_operation() { return std::check_flag(flag, group_flag_t::operation_empty); }
			};

			using gcmd_group_t = tree_t<cmd_group_t>;

			struct element_gcmd_group_t
			{
				element_gcmd_group_t() {}
				element_gcmd_group_t(gcmd_group_t* g) { gcmd = g; }

				gcmd_group_t* gcmd = nullptr;
			};

			using global_gcmd_group_t = std::vector<element_gcmd_group_t>;

			void get_property(gcmd_group_t* gcmd, const pel::obj_t* obj, const pel::obj_t* original_obj = nullptr)
			{
				cmd_group_t* cmd = &gcmd->get_value();
				cmd_group_t* parent_cmd = &gcmd->parent->get_value();

				if (obj->is_type)
				{
					cmd->group.name = obj->name;
				} 

				if (obj->is_value)
				{
					cmd->group.push_element(obj->name);
					cmd->group.sort_elements();
				}
						
				bool is_or = false, is_and = false;

				for (auto& it : obj->values)
				{
					if (it.is_or)
						is_or = true;

					if (it.is_and)
						is_and = true;
				}

				if (obj->is_type)
					std::add_flag(cmd->flag, group_flag_t::object_type);

				if (obj->is_value)
					std::add_flag(cmd->flag, group_flag_t::object_value);

				if (obj->is_ex)
					std::add_flag(cmd->flag, group_flag_t::property_ex);

				if (obj->is_not)
					std::add_flag(cmd->flag, group_flag_t::operation_not);


				if (is_or)
				{
					std::add_flag(cmd->flag, group_flag_t::operation_or);
					std::del_flag(cmd->flag, group_flag_t::operation_and);
				}

				if (is_and)
				{
					std::add_flag(cmd->flag, group_flag_t::operation_and);
					std::del_flag(cmd->flag, group_flag_t::operation_or);
			
				}

				if ((!cmd->is_or() && !cmd->is_and()))
					std::add_flag(cmd->flag, group_flag_t::operation_empty);

				if (obj->is_or) {
					std::add_flag(parent_cmd->flag, group_flag_t::operation_or);
					std::del_flag(parent_cmd->flag, group_flag_t::operation_and);
				}

				if (obj->is_glue)
				{
					std::add_flag(parent_cmd->flag, group_flag_t::property_glue);
					cmd->group.is_glue = true;
				}
				else
				{
					cmd->group.is_glue = false;
				}

				if (obj->is_split)
				{
					std::add_flag(parent_cmd->flag, group_flag_t::property_split);
					cmd->group.is_split = true;
				}
				else
				{
					cmd->group.is_split = false;
				}

				if (obj->is_ignore)
				{
					std::add_flag(parent_cmd->flag, group_flag_t::property_ignore);
					cmd->group.is_ignore = true;
				}
				else
				{
					cmd->group.is_ignore = false;
				}

				gcmd->flush_value();
			}
		}
	}

}