#pragma once

#include "detail/flag.hpp"

namespace pel {

	// flag association table
	static const std::vector<std::pair<pel::obj_flag_t, parser::executive::quantum_flag_t>> table_association_obj_quentum_flags = {
		 { pel::obj_flag_t::obj_value, parser::executive::quantum_flag_t::quantum_value },
		 { pel::obj_flag_t::obj_type, parser::executive::quantum_flag_t::quantum_type },
		 { pel::obj_flag_t::obj_exists, parser::executive::quantum_flag_t::quantum_exists },
		 { pel::obj_flag_t::obj_and, parser::executive::quantum_flag_t::quantum_and },
		 { pel::obj_flag_t::obj_or, parser::executive::quantum_flag_t::quantum_or },
		 { pel::obj_flag_t::obj_not, parser::executive::quantum_flag_t::quantum_not },
		 { pel::obj_flag_t::obj_group, parser::executive::quantum_flag_t::quantum_group },
		 { pel::obj_flag_t::obj_repeat, parser::executive::quantum_flag_t::quantum_repeat },
		 { pel::obj_flag_t::obj_maybe, parser::executive::quantum_flag_t::quantum_maybe },
		 { pel::obj_flag_t::obj_return, parser::executive::quantum_flag_t::quantum_return },
		 { pel::obj_flag_t::obj_exit, parser::executive::quantum_flag_t::quantum_exit },
		 { pel::obj_flag_t::obj_recursion, parser::executive::quantum_flag_t::quantum_recursion },
		 { pel::obj_flag_t::obj_auto, parser::executive::quantum_flag_t::quantum_autogen },
		 { pel::obj_flag_t::obj_breakpoint, parser::executive::quantum_flag_t::quantum_breakpoint },
		 { pel::obj_flag_t::obj_true, parser::executive::quantum_flag_t::quantum_true },
		 { pel::obj_flag_t::obj_false, parser::executive::quantum_flag_t::quantum_false },
		 { pel::obj_flag_t::obj_break, parser::executive::quantum_flag_t::quantum_break },
		 { pel::obj_flag_t::obj_break_all, parser::executive::quantum_flag_t::quantum_break_all },
	};

	// O(n)
	// can`t be O(1) sad
	void association_flags_for_cmd(const obj_t &obj, parser::executive::cmd_t *cmd) {

		for (const auto &it : table_association_obj_quentum_flags)
		{
			if (std::check_flag(obj.flag, it.first))
				std::add_flag(cmd->flag, it.second);			
		}

		if (std::check_flag(obj.flag, pel::obj_flag_t::obj_group) && std::check_flag(obj.flag, pel::obj_flag_t::obj_type))
			std::del_flag(cmd->flag, parser::executive::quantum_type);

		if (std::check_flag(obj.flag, pel::obj_flag_t::obj_group) && std::check_flag(obj.flag, pel::obj_flag_t::obj_exists))
			std::del_flag(cmd->flag, parser::executive::quantum_exists);

		if (std::check_flag(obj.flag, pel::obj_flag_t::obj_or))
			std::del_flag(cmd->flag, parser::executive::quantum_and);

		if (std::check_flag(obj.flag, pel::obj_flag_t::obj_and))
			std::del_flag(cmd->flag, parser::executive::quantum_or);

		if ((!cmd->is_or() && !cmd->is_and()))
			std::add_flag(cmd->flag, parser::executive::empty_operation);		
	}

}