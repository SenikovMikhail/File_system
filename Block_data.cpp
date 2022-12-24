#include "Block_data.h"

Block_data::Block_data() {
	block.reserve(copasity);
}

Block_data::Block_data(Block_data&& other)
	: size(other.size)
	, block(other.block)
{}


Block_data::Block_data(const std::string& data)
	: block(data, 0, (Block_data::get_data_size(data) > copasity ? copasity : Block_data::get_data_size(data)))
	, size(Block_data::get_data_size(data) > copasity ? copasity : Block_data::get_data_size(data))
{
	if (copasity - size > 0)
		block.insert(size, copasity - size, '\0');
}

std::string& Block_data::read() {
	return block;
}


Block_data* Block_data::add(const std::string& data) {

	int free_space = copasity - size;

	block.insert(size, data, 0, (free_space < data.size() ? free_space : data.size()));
	size += data.size();

	if (copasity - size > 0)
		block.insert(size, copasity - size, '\0');

	return this;
}


int64_t Block_data::get_size() { return size; }

int64_t Block_data::get_copasity() { return copasity; }

int64_t Block_data::get_data_size(const std::string& data) {
	return (data.find_first_of('\0') > copasity ? data.size() : data.find_first_of('\0') );
}