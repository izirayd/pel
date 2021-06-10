#pragma once
#include <stdint.h>

#define show_logs   if (is_render_tree)
#define show_result if (is_render_tree)
#define show_tree   if (is_render_tree)

enum class status_find_t : int8_t
{
	unknow,
	success,
	failed
};

class status_process_t
{
public:
	status_find_t status_find = status_find_t::unknow;

	bool is_status_exit  = false;
	bool is_status_break = false;
};
