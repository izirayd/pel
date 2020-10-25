#pragma once

#include <cstddef>
#include <iostream>
#include <string.h>
#include <typeinfo>
#include <vector>
#include <functional>
#include <cstring>
#include <memory>
#include <mutex>
#include <atomic>
#include <thread>
#include <future>

#include "function_wrapper.hpp"

#include "fmt/fmt/printf.h"
#include "fmt/fmt/color.h"

namespace std { template<class T> inline void clear(std::vector<T>& vec) { std::vector<T>().swap(vec); } }
namespace std { template<class T> inline void clear(std::string& vec) { std::string().swap(vec); } }

void print_space(std::size_t count, bool is_elements)
{
	if (is_elements && count == 0)
	{
		printf("%c", (char)254);
		return;
	}

	std::size_t tmp_count = count;

	for (size_t i = 0; i < tmp_count; i++)
	{
		printf("%c", (char)221);
	}

	if (is_elements)
	{
		printf("%c", (char)254);
	}
	else
	{
		printf("%c", (char)254);
	}
}

template <typename data_value_t>
class tree_t
{
public:
	std::vector<tree_t<data_value_t>*> tree;
	bool is_value = false;

private:
	data_value_t          value;

	tree_t<data_value_t>* push_tree(tree_t<data_value_t>* tree) {

		tree->root  = root;

		if (tree->index == 0) {
			tree->index = root->last_index;
			root->last_index++;	
		}

		tree->is_root = false;
		tree->parent  = this;

		if (tree->index_level == 0) {
			tree->index_level = tree->parent->last_index_level;
			tree->parent->last_index_level++;
		}

		this->tree.push_back(tree);
		tree->position = this->tree.size() - 1;

		tree->calculate_previous_next();

		return tree;
	}

public:

	inline bool is_last() const { if (parent->size() == 0) return false; return parent->size() - 1 == position; }

	void sync(tree_t<data_value_t>* tree)
	{
		tree->is_value = false;
		tree->root     = root;
		tree->parent   = this;

		if (index == 0)
		{
			tree->index = root->last_index;
			root->last_index++;
		}

		if (index_level == 0) {
			tree->index_level = tree->parent->last_index_level;
			tree->last_index_level++;
		}
	}

	data_value_t *last_element() {
	
		if (tree.empty())
			return nullptr;

		return &tree.back()->get_value();
	}


	tree_t<data_value_t>* parent   = nullptr; // родитель
	tree_t<data_value_t>* root     = nullptr;

	tree_t<data_value_t>* previous = nullptr;
	tree_t<data_value_t>* next	   = nullptr;
	tree_t<data_value_t>* first_chield = nullptr;
	tree_t<data_value_t>* real_first_chield = nullptr; // for reset state

	bool    is_root    = true;

	// TODO: size_t?
	int     last_index = 0;
	int     index      = 0;

	int     last_index_level = 0;
	int     index_level = 0;

	// index position in list in parent for check last element in graph
	std::size_t position = 0;

	void calculate_previous_next() {

		if (position == 0) 
		{
			previous = nullptr;

			if (parent && !parent->first_chield)
			{
				parent->first_chield = this;
				parent->real_first_chield = this;
			}

			if ((position + 1) < parent->size())
			{
				next = parent->tree[position + 1];
			}
				else
			{
				next = nullptr;
			}
		}
		else if (position > 0)
		{
			previous = parent->tree[position - 1];

			previous->next = this;

			if ((position + 1) < parent->size())
			{
				next = parent->tree[position + 1];
			}
				else
			{
				next = nullptr;
			}
		}
	}

	tree_t() {
		root   = this; 
		parent = root;
	}

	tree_t(const data_value_t& data) {
		value    = data;
		is_value = true;
		root     = this;
		parent   = root;
	}

	tree_t(const std::initializer_list<data_value_t>& v)
	{
		root   = this;
		parent = root;

		for (const auto& itm : v)
			push(itm);
	}

	inline tree_t<data_value_t>* push(const std::initializer_list<data_value_t>& v) {

		tree_t<data_value_t>* last_element = nullptr;

		if (v.size() == 0)
		{
			last_element = push_tree(new tree_t<data_value_t>);
			last_element->is_value = true;

		} else
		for (const auto& itm : v)
			last_element = push(itm);

		return last_element;
	}

	inline tree_t<data_value_t>* push() { return nullptr; }

	template <class... next_t> inline tree_t<data_value_t>* push(next_t... args) {
		return push(args...);
	}

	inline tree_t<data_value_t>* push(const data_value_t& data) {
		return push_tree(new tree_t<data_value_t>(data));
	}

	inline tree_t<data_value_t>* push(tree_t<data_value_t>* child) {
		return push_tree(child);
	}

	template <class first_t, class... next_t>
	inline tree_t<data_value_t>* push(const first_t& first, const next_t& ... next) {
		push_tree(new tree_t<data_value_t>(first));
		return push(next...);
	}

	std::function<void(tree_t<data_value_t>*)> func;

	detail::function_wrapper_t process_function;

	inline std::size_t size() { return tree.size(); }

	void stop_process()
	{
		root->is_process = false;
	}

	template <typename... arguments>
	void start_process(arguments&& ... args)
	{
		root->is_process = true;

		int lvl = 0;
		process(this, lvl, (args)...);

		root->is_process = false;
	}

	template <typename... arguments>
	void start_process_for(const std::string &base, const std::string& last_parent, arguments&& ... args)
	{
		root->is_process = true;

		int lvl = 0;
		process(base, last_parent, this, lvl, (args)...);

		root->is_process = false;
	}

	template <typename... arguments>
	void process(const std::string& base, const std::string& last_parent, tree_t* tree, int lvl, arguments&& ... args)
	{
		if (!root->is_process)
			return;

		if (!tree)
			return;

		tree->level = lvl;

		if (tree->is_value)
			process_function.call(base, *tree, (args)...);

		for (size_t i = 0; i < tree->tree.size(); i++)
		{
			if (tree->tree[i])
			{
				lvl++;

				process(base, last_parent, tree->tree[i], lvl, (args)...);

				if (tree->is_value)
				{
					if (tree->tree[i]->is_last())
					{
						process_function.call(last_parent, *tree, *tree->tree[0], *tree->tree[i], (args)...);
					}
				}

				lvl--;
			}
		}
	}


	void process_root_enable() {
		root->is_process = true;
	}

	template <typename... arguments>
	void process(tree_t* tree, int lvl, arguments&& ... args)
	{
		if (!root->is_process)
			return;

		if (!tree)
			return;

		tree->level = lvl;

		if (tree->is_value)
			process_function.call("base", *tree, (args)...);
		
		for (size_t i = 0; i < tree->tree.size(); i++)
		{
			if (tree->tree[i])
			{
			    lvl++;

				  process(tree->tree[i], lvl, (args)...);

				  if (i == (tree->tree.size() - 1))
				  {
					  if (tree->is_value)
						  process_function.call("last", *tree, (args)...);
				  }

				  if (tree->is_value)
				  {
					  if (tree->tree[i]->is_last())
					  {
						  process_function.call("last_parent", *tree, *tree->tree[0], *tree->tree[i], (args)...);
					  }
				  }


				lvl--;
			}
		}
	}
	
	bool is_have_sub_elemets()
	{
		return !tree.empty();
	}

	std::size_t level = 0;

	void delete_tree()
	{
		for (size_t i = 0; i < tree.size(); i++)
		{
			if (tree[i]) {

				tree[i]->delete_tree();

				delete tree[i];
				tree[i] = nullptr;
			}
		}

		std::clear(tree);
	}

	inline data_value_t  get_value() const   { return value; }
	inline data_value_t& get_value()		 { return value; }

	void set_value(const data_value_t& new_value) {
		value    = new_value;
		is_value = true;
	}

	void flush_value()
	{
		is_value = true;
	}

	bool is_process = false;
};
