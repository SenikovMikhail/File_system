#pragma once

#include <string>
#include <cstdint>

class I_node {

public:

	I_node() = default;
	I_node(const std::string& _name, const std::string& _owner, char _file_type, const std::string& _mask = "600", int64_t *_offset_block_data = nullptr, int64_t add_bl = -1);

	void fill(I_node&& other);

	std::string& get_name();
	std::string& get_owner();
	char get_file_type();
	void set_file_type(char _file_type);
	std::string& get_mask();
	int64_t get_offset_data_block(int num); // max 10
	void set_offset_data_block(int num, int64_t offset);
	int64_t get_add_bl();

	static const int max_length_name = 64;
	static const int file_type_size = 1;
	static const int mask_size = 3;
	static const int offset_blok_data_count = 10;

private:
	std::string name; // max 64 byte
	std::string owner;
	char file_type;
	std::string mask; // 000-777
	int64_t offset_block_data[10]; // block data offset
	int64_t add_bl; // additional block data offset
};