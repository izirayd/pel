#pragma once

#include "detail/flag.hpp"

enum class parse_flag_t : std::flag16_t
{
	unknow		 = 1 << 0,
	word		 = 1 << 1,
	space_tab    = 1 << 2,
	symbol		 = 1 << 3,
	group_symbol = 1 << 4,
	new_line	 = 1 << 5
};