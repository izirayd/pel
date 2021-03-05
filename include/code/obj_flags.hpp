#pragma once

#include "detail/flag.hpp"

#include <vector>
#include <string>

namespace pel {

	using obj_flag_base_t = std::flag32_t;

	enum obj_flag_t : obj_flag_base_t {

		obj_value = 1 << 0,
		obj_type = 1 << 1,
		obj_execute = 1 << 2,
		obj_group = 1 << 3,
		obj_and = 1 << 4,
		obj_or = 1 << 5,
		obj_not = 1 << 6,
		obj_true = 1 << 7,
		obj_false = 1 << 8,
		obj_glue = 1 << 9,
		obj_split = 1 << 10,
		obj_ignore = 1 << 11,
		obj_auto = 1 << 12,
		obj_maybe = 1 << 13,
		obj_recursion = 1 << 14,
		obj_repeat = 1 << 15,
		obj_breakpoint = 1 << 16,
		obj_return = 1 << 17,
		obj_exit = 1 << 18,
		obj_break = 1 << 19,
		obj_break_all = 1 << 20,
		obj_max = 1 << 21,
	};

	static const std::vector<std::pair<obj_flag_t, std::vector<std::string>>> property_flags = {
		{ obj_flag_t::obj_execute,   { "execute"    } },
		{ obj_flag_t::obj_glue,      { "glue"       } },
		{ obj_flag_t::obj_split,     { "split"      } },
		{ obj_flag_t::obj_ignore,    { "ignore"     } },
		{ obj_flag_t::obj_maybe,     { "maybe"      } },
		{ obj_flag_t::obj_recursion, { "recursion"  } },
		{ obj_flag_t::obj_repeat,    { "repeat"     } },
		{ obj_flag_t::obj_breakpoint,{ "breakpoint" } },
		{ obj_flag_t::obj_return,    { "return"     } },
		{ obj_flag_t::obj_exit,      { "exit"       } },
		{ obj_flag_t::obj_break,     { "break"      } },
		{ obj_flag_t::obj_break_all, { "break_all"  } },
	};

	static const std::vector<std::pair<obj_flag_t, std::vector<std::string>>> chain_flags = {
		{ obj_flag_t::obj_and,  { "and", "," } },
		{ obj_flag_t::obj_or,   { "or"    } },
	};

}