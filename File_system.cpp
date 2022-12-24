#include <iterator>

#include "./File_system.h"
#include "./I_node.h"

File_system::File_system(std::shared_ptr<std::fstream> _file)
	: file{ _file }
{
	if (file->tellg() > 0) {
		
		std::string read_data;
		read_data.reserve(offset_len);

		file->seekp(0, std::ios_base::beg);
		for (int i = 0; i < free_node_count; ++i) {
			file->read(&read_data[0], offset_len);
			free_node[i] = stoi(read_data);
		}

		for (int i = 0; i < free_block_count; ++i) {
			file->read(&read_data[0], offset_len);
			free_block[i] = stoi(read_data);
		}
	}
}


int64_t File_system::create_enty(const std::string& name, I_node& current_node, int64_t offset, const std::string& owner, char type) {

	std::string dir_content;
	
	int block_num = 0;
	while (current_node.get_offset_data_block(block_num) != -1 && block_num < I_node::offset_blok_data_count) {
		dir_content += read_block(current_node.get_offset_data_block(block_num)).read();
		++block_num;
	}

	// if i 10, look up add. block

	I_node new_enty(name, owner, type);

	int64_t offset_new_enty = write_fs(new_enty);

	std::string new_enty_str = std::to_string(offset_new_enty);
	new_enty_str.append(offset_len - new_enty_str.size(), ' ');

	new_enty_str += new_enty.get_name().substr(0, Block_data::get_data_size(new_enty.get_name()));
	new_enty_str.append(I_node::max_length_name - Block_data::get_data_size(new_enty.get_name()), ' ');

	if ((block_num > 0)&& (Block_data::copasity -
		(Block_data::get_data_size(read_block(current_node.get_offset_data_block(block_num - 1)).read())) > new_enty_str.size() )) {
		
		Block_data update_block((read_block(current_node.get_offset_data_block(block_num - 1)).add(new_enty_str))->read());
		write_fs(update_block, current_node.get_offset_data_block(block_num - 1));

	} else if(block_num < I_node::offset_blok_data_count){

		Block_data bl_data(new_enty_str);
		current_node.set_offset_data_block(block_num, write_fs(bl_data));
		update_fs_enty(offset, current_node);
	}

	return offset_new_enty;
}


std::string File_system::read_enty(int64_t offset) {

	I_node enty = read_i_node(offset);

	std::string data;
	int i = 0;
	while (enty.get_offset_data_block(i) != -1) {
		data += read_block(enty.get_offset_data_block(i)).read();
		++i;
	}

	return data;
}


void File_system::delete_enty(int64_t offset, char type) {

	if (type == 'f') {

		I_node deleted_file = read_i_node(offset);

		int block_num = 0;
		while (deleted_file.get_offset_data_block(block_num) != -1) {
			delete_block_data(deleted_file.get_offset_data_block(block_num));
			++block_num;
		}

		delete_i_node(offset);

	}
	else if (type == 'd') {


		// надо реализрвать рекурсивное удаление
	}
}


int64_t File_system::get_node_offset() {

	for (int node_number = 0; (node_number < free_node_count); ++node_number) {
		if (free_node[node_number] != -1) {

			int64_t offset = free_node[node_number];
			free_block[node_number] = -1;

			file->seekp(node_number * offset_len, std::ios_base::beg);
			std::string str = std::to_string(free_block[node_number]);
			str.append(offset_len - str.size(), '\0');
			*file << str;

			return offset;
		}
	}
	return -1;
}


int64_t File_system::get_block_offset() {
	for (int block_number = 0; block_number < free_block_count; ++block_number) {

		if (free_block[block_number] != -1) {

			int64_t offset = free_block[block_number];
			free_block[block_number] = -1;

			file->seekp(free_node_count*offset_len + block_number * offset_len, std::ios_base::beg);
			std::string str = std::to_string(free_block[block_number]);
			str.append(offset_len - str.size(), '\0');
			*file << str;

			return offset;
		}
	}

	return -1;
}


I_node File_system::read_i_node(int64_t offset) {

	file->flush();
	file->seekp(offset, std::ios_base::beg);

	
	std::string name(I_node::max_length_name, '\0');
	file->read(&name[0], I_node::max_length_name);
	//std::string name = tmp;
	std::string owner(I_node::max_length_name, '\0');
	file->read(&owner[0], I_node::max_length_name);
	//std::string owner = tmp;
	char file_type = '\0';
	file->read(&file_type, I_node::file_type_size);
	//char file_type = tmp[0];
	std::string mask(I_node::mask_size, '\0');
	file->read(&mask[0], I_node::mask_size);
	//std::string mask = tmp;

	int64_t offset_blok_data[10] = { -1 };// = new int64_t[I_node::offset_blok_data_count];
	//tmp.clear();
		std::string tmp(offset_len, '\0');
	for (int i = 0; i < I_node::offset_blok_data_count; ++i) {
		file->read(&tmp[0], offset_len);
			offset_blok_data[i] = std::stoi(tmp);
	}

	file->read(&tmp[0], offset_len);
	int64_t add_bl = std::stoi(tmp);

	I_node node(name, owner, file_type, mask, offset_blok_data, add_bl);

	return node;
}

Block_data File_system::read_block(int64_t offset) {

	std::string read_data(Block_data::copasity, '\0');

	file->seekp(offset);

	file->read(&read_data[0], Block_data::copasity);

	//std::string _data(&read_data[0], &read_data[Block_data::copasity]);
	Block_data block(read_data.substr(0, Block_data::copasity));

	return block;
}


int64_t File_system::write_fs(I_node& node, int64_t offset) {


	if (offset == -2)
		offset = get_block_offset();
	else
		file->seekp(offset, std::ios_base::beg);

	if (offset == -1) {
		file->seekp(0, std::ios_base::end);
		offset = file->tellp();
	}


	std::string i_node_str;
	i_node_str += node.get_name();
	i_node_str += node.get_owner();
	i_node_str += node.get_file_type();
	i_node_str += node.get_mask();
	for (int i = 0; i < I_node::offset_blok_data_count; ++i) {
		i_node_str += std::to_string(node.get_offset_data_block(i));
		i_node_str.append(offset_len - std::to_string(node.get_offset_data_block(i)).size(), '\0');
	}

	i_node_str += std::to_string(node.get_add_bl());
	i_node_str.append(offset_len - std::to_string(node.get_add_bl()).size(), '\0');

	file->write(&i_node_str[0], i_node_str.size());
	file->flush();
	return offset;
}

int64_t File_system::write_fs(Block_data& block, int64_t offset) {



	if (offset == -2)
		offset = get_block_offset();
	else
		file->seekp(offset, std::ios_base::beg);

	if (offset == -1) {
		file->seekp(0, std::ios_base::end);
		offset = file->tellp();
	}
	

	file->write(&block.read()[0], Block_data::copasity);

	return offset;
}


void File_system::update_fs_enty(int64_t offset, I_node& node) {
	write_fs(node, offset);
}


void File_system::update_fs_enty(int64_t offset, Block_data& block) {
	write_fs(block, offset);
}


void File_system::delete_i_node(int64_t offset) {

	for (int i = 0; i < free_node_count; ++i) {
		if (free_node[i] == -1) {

			free_node[i] = offset;

			file->seekp(i*offset_len, std::ios_base::beg);

			std::string str = std::to_string(offset);
			str.append(offset_len - str.size(), '\0');

			*file << str;

			return;
		}
	}
	// если список свободных блоков полон, надо сделать
}


void File_system::delete_block_data(int64_t offset) {

	for (int i = 0; i < free_block_count; ++i) {
		if (free_node[i] == -1) {
			free_block[i] = offset;

			file->seekp(free_node_count*offset_len + i * offset_len, std::ios_base::beg);

			std::string str = std::to_string(offset);
			str.append(offset_len - str.size(), '\0');

			*file << str;

			return;
		}
	}
}


std::shared_ptr<std::fstream> File_system::get_file() {
	return file;
}

int64_t File_system::get_start_offset() {
	return start_offset;
}


int64_t File_system::get_offset_len() {
	return offset_len;
}


int64_t File_system::get_free_node_count() {
	return free_node_count;
}


int64_t File_system::get_free_block_count() {
	return free_block_count;
}
