#pragma once

#include "./Block_data.h"
#include "I_node.h"

#include <string>
#include <cstdint>
#include <memory>
#include <fstream>

class File_system {

public:

	File_system(std::shared_ptr<std::fstream> _file);

	int64_t create_enty(const std::string& name, I_node& current_node, int64_t offset, const std::string& owner, char type);
	std::string read_enty(int64_t offset);// !? , char type);
	void delete_enty(int64_t offset, char type);

	int64_t get_node_offset();
	int64_t get_block_offset();

	I_node read_i_node(int64_t offset);
	Block_data read_block(int64_t offset);

	int64_t write_fs(I_node& node, int64_t offset = -2);
	int64_t write_fs(Block_data& block, int64_t offset = -2);

	void update_fs_enty(int64_t offset, I_node& node);
	void update_fs_enty(int64_t offset, Block_data& block);

	void delete_i_node(int64_t offset);
	void delete_block_data(int64_t offset);

	std::shared_ptr<std::fstream> get_file();
	int64_t get_start_offset();
	int64_t get_offset_len();
	int64_t get_free_node_count();
	int64_t get_free_block_count();

private:

	std::shared_ptr<std::fstream> file;
	int offset_len = 20;
	int free_node_count = 20;
	int free_block_count = 150;
	int64_t start_offset = (free_block_count + free_node_count) * offset_len;
	int64_t free_node[20];
	int64_t free_block[150];

};