#include "I_node.h"


// i_node::i_node(const std::string& _name, const std::string& _owner, const std::string& _mask) ?

I_node::I_node(const std::string& _name, const std::string& _owner, char _file_type, const std::string& _mask, int64_t *_offset_block_data, int64_t _add_bl)
	: name(_name, 0, (_name.size() < max_length_name ? _name.size() : max_length_name))
	, owner(_owner, 0, (_owner.size() < max_length_name ? _owner.size() : max_length_name))
	, file_type(_file_type)
	, mask(_mask, 0, 3)
	, add_bl(_add_bl)
{


	if (_offset_block_data == nullptr)
		for (int i = 0; i < 10; ++i)
			offset_block_data[i] = -1;
	else
		for (int i = 0; i < 10; ++i)
			offset_block_data[i] = _offset_block_data[i];

	if (_name.size() < max_length_name)
		name.append(max_length_name - _name.size(), '\0');

	if (_owner.size() < max_length_name)
		owner.append(max_length_name - _owner.size(), '\0');
}


void I_node::fill(I_node&& other)
{
	name = other.name;
	owner = other.owner;
	file_type = other.file_type;
	mask = other.mask;
	for (int i = 0; i < offset_blok_data_count; ++i)
		offset_block_data[i] = other.offset_block_data[i];
	add_bl = other.add_bl;
}

std::string& I_node::get_name() { return name; }

std::string& I_node::get_owner() { return owner; }

char I_node::get_file_type() { return file_type; }

/*
void I_node::set_file_type(char _file_type) {
	file_type = _file_type;
}
*/

std::string& I_node::get_mask() { return mask; }

int64_t I_node::get_offset_data_block(int num) { return offset_block_data[num]; }

void I_node::set_offset_data_block(int num, int64_t offset) {
	offset_block_data[num] = offset;
}

int64_t I_node::get_add_bl() { return add_bl; }