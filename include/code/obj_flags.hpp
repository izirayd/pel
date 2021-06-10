#pragma once

#include "detail/flag.hpp"

#include <vector>
#include <string>
#include <unordered_map>

namespace pel {

	using obj_flag_base_t = std::flag32_t;

	enum obj_flag_t : obj_flag_base_t {

		obj_value = 1 << 0,
		obj_type = 1 << 1,
		obj_exists = 1 << 2,
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

	static const std::unordered_map<std::string, obj_flag_t> property_flags = {
		{    "exists",     obj_flag_t::obj_exists,    },
		{    "glue",       obj_flag_t::obj_glue        },
		{    "split",      obj_flag_t::obj_split,      },
		{    "ignore",     obj_flag_t::obj_ignore      },
		{    "maybe",      obj_flag_t::obj_maybe       },
		{    "recursion",  obj_flag_t::obj_recursion   },
		{    "repeat",     obj_flag_t::obj_repeat      },
		{    "breakpoint", obj_flag_t::obj_breakpoint  },
		{    "return",     obj_flag_t::obj_return      },
		{    "exit",       obj_flag_t::obj_exit        },
		{    "break",      obj_flag_t::obj_break       },
		{    "break_all",  obj_flag_t::obj_break_all   },
		{    "true",       obj_flag_t::obj_true        },
		{    "false",      obj_flag_t::obj_false       },
	};

	static const std::unordered_map<std::string, obj_flag_t>  chain_flags = {
		{ "and", obj_flag_t::obj_and,  },
		{ ",",   obj_flag_t::obj_and,  },
		{ "or",  obj_flag_t::obj_or   }
	};
}