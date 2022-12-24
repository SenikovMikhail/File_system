#pragma once

#include <string>
#include <cstdint>

class Block_data {

public:

	Block_data();
	Block_data(Block_data&& other);
	Block_data(const std::string& data);
	std::string& read();
	Block_data* add(const std::string& data);

	int64_t get_size();
	int64_t get_copasity();
	static int64_t get_data_size(const std::string& data);
	static const int copasity = 85; // 524288 byte = 512Kb

private:

	int size = 0;
	std::string block;

};
