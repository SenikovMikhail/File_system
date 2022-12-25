#include "Interface.h"
#include "File_system.h"
#include "I_node.h"
#include <iostream>


Interface::Interface(const std::string& usr_name, const std::string& _path)
	: user_name{ usr_name }
	, file { std::make_shared<std::fstream>(_path, std::ios::in | std::ios::out | std::ios::ate)}
	, file_sys{ nullptr }
{}


void Interface::create_dir(const std::string& name) {

	int64_t offset = file_sys->create_enty(name.substr(0, Block_data::get_data_size(name)), current_node, current_offset, user_name, 'd');
	
	I_node new_dir_i_node = file_sys->read_i_node(offset);
	std::string dir_content;
	int block_num = 0;
	while (new_dir_i_node.get_offset_data_block(block_num) != -1 && block_num < I_node::offset_blok_data_count) {
		dir_content += file_sys->read_block(new_dir_i_node.get_offset_data_block(block_num)).read();
		++block_num;
	}
	
	std::string new_enty_str = std::to_string(current_offset);
	new_enty_str.append(file_sys->get_offset_len() - new_enty_str.size(), ' ');

	new_enty_str.append(".."); // size = 2
	new_enty_str.append(I_node::max_length_name - 2, ' ');

	if ((block_num > 0) && (Block_data::copasity -
		(Block_data::get_data_size(file_sys->read_block(new_dir_i_node.get_offset_data_block(block_num - 1)).read())) > new_enty_str.size())) {
		Block_data update_block( (file_sys->read_block(new_dir_i_node.get_offset_data_block(block_num - 1)).add(new_enty_str))->read() );
		file_sys->write_fs(update_block, new_dir_i_node.get_offset_data_block(block_num - 1));
	}
	else if (block_num < I_node::offset_blok_data_count) {
		Block_data bl_data(new_enty_str);
		new_dir_i_node.set_offset_data_block(block_num, file_sys->write_fs(bl_data));
		file_sys->update_fs_enty(offset, new_dir_i_node);
	}	

 	add_dir_content(offset, name.substr(0, Block_data::get_data_size(name)));
}
	

void Interface::read_dir(const std::string& name) {

	std::string data = file_sys->read_enty(get_enty_offset_by_name(name));
	dir_content.clear();

	int block_num = 0;
	while (current_node.get_offset_data_block(block_num) != -1 && block_num < I_node::offset_blok_data_count) 
		++block_num;
	
	for (int j = 0; j < block_num; ++j) {
		for (int i = 0; i < (Block_data::copasity / (I_node::max_length_name + file_sys->get_offset_len())); ++i) {

			add_dir_content(std::stoi(data.substr((i*file_sys->get_offset_len() + i * I_node::max_length_name + j*Block_data::copasity), file_sys->get_offset_len())),
				data.substr(( (i+1)*file_sys->get_offset_len() + i * I_node::max_length_name + j*Block_data::copasity), I_node::max_length_name) );
		}
	}
}


void Interface::delete_dir(const std::string& name) {
	file_sys->delete_enty(get_enty_offset_by_name(name), 'd');
	delete_dir_content(name);
}


void Interface::create_file(const std::string& name) {
	add_dir_content(file_sys->create_enty(name, current_node, current_offset, user_name, 'f'), name);
}


std::string Interface::read_file(const std::string& name) {

	if (get_enty_offset_by_name(name) == -1) {
		std::cout << "File not found" << std::endl;
		return "";
	}

	return file_sys->read_enty(get_enty_offset_by_name(name));// , 'f');
}


void Interface::write_to_file(const std::string& name, const std::string& data) {

	int64_t offset_file = get_enty_offset_by_name(name);
	int64_t data_start = 0;
	I_node file = file_sys->read_i_node(offset_file);

	int block_number = 0;
	bool find = false;
	for (; (block_number < I_node::offset_blok_data_count); ++block_number) {
		find = (file.get_offset_data_block(block_number) == -1) ? true : false;
		
		if (find)
			break;
	}

	if (find) {
		if (block_number != 0) {
			Block_data previouse_block( (file_sys->read_block(file.get_offset_data_block(block_number - 1))).read() );
			int free_space = Block_data::copasity - previouse_block.get_size();
			previouse_block.add(data.substr(data_start, free_space) );
			data_start += free_space;
			file_sys->write_fs(previouse_block, file.get_offset_data_block(block_number - 1));
		}

		if (data.size() - data_start > 0) {
			int new_block_count = ((data.size() - data_start) / Block_data::copasity) + 1;
			for (int i = block_number; i < I_node::offset_blok_data_count && i < block_number + new_block_count; ++i) {
				Block_data new_block(data.substr(data_start, Block_data::copasity));
				//new_block.add(std::string(Block_data::copasity - new_block.get_size(), '\0'));
				file.set_offset_data_block(i, file_sys->write_fs(new_block));
				data_start += Block_data::copasity;
			}
			file_sys->update_fs_enty(offset_file, file);
		}

		file_sys->get_file()->flush();

	}
}


void Interface::delete_file(const std::string& name) {
	file_sys->delete_enty(get_enty_offset_by_name(name), 'f');
	delete_dir_content(name);
}


int64_t Interface::get_enty_offset_by_name(const std::string& name) {
	
	if (dir_content[name] == 0) {
		dir_content.erase(name);
		return -1;
	}

	return dir_content[name];
}


void Interface::delete_dir_content(const std::string& name) {
	dir_content.erase(name);
}


void Interface::add_dir_content(int64_t offset, const std::string& name) {
	dir_content.emplace(name.substr(0, name.find_first_of(" ")), offset);
}


void Interface::create_fs() {

	std::string str;
	for (int i = 0; i < 170; ++i) {
		str += std::to_string(-1);
		str.append(20 - std::to_string(-1).size(), '\0');
	}

	file->seekp(0, std::ios_base::beg);
	*file << str;

	file_sys = std::make_shared<File_system>(file);
	
	std::string root_dir_name = "/";
	I_node node(root_dir_name, user_name, 'd');
	file_sys->write_fs(node);
	current_offset = file_sys->get_start_offset();
	current_node.fill(file_sys->read_i_node(current_offset));
	pwd = current_node.get_name().substr( 0, Block_data::get_data_size(current_node.get_name()) );
	
	std::string data = file_sys->read_enty(current_offset);

	for (int i = 0; i < (data.size() / ( I_node::max_length_name + file_sys->get_offset_len()) ); ++i) {
		add_dir_content(std::stoi(data.substr((i*file_sys->get_offset_len() + i * I_node::max_length_name), file_sys->get_offset_len())),
			data.substr((i*file_sys->get_offset_len() + i * I_node::max_length_name + file_sys->get_offset_len()), I_node::max_length_name));
	}

}

void Interface::open_fs() {

	file_sys = std::make_shared<File_system>(file);
	current_offset = file_sys->get_start_offset();
	current_node.fill(file_sys->read_i_node(current_offset));
	pwd = current_node.get_name();
	
	std::string data = file_sys->read_enty(current_offset);

	for (int i = 0; i < (data.size() / (I_node::max_length_name + file_sys->get_offset_len())); ++i) {
		add_dir_content(std::stoi(data.substr((i*file_sys->get_offset_len() + i * I_node::max_length_name), file_sys->get_offset_len())),
			data.substr((i*file_sys->get_offset_len() + i * I_node::max_length_name + file_sys->get_offset_len()), I_node::max_length_name));
	}
}


void Interface::delete_enty(const std::string& name) {
	file_sys->delete_enty(dir_content[name], file_sys->read_i_node(dir_content[name]).get_file_type());
	dir_content.erase(name);
}

void Interface::change_dir(const std::string & dir_name) {


	if (get_enty_offset_by_name(dir_name) == -1) {
		std::cout << "Directory not found" << std::endl;
		return;
	}

	current_offset = dir_content.at(dir_name);
	current_node = file_sys->read_i_node(current_offset);
	read_dir(dir_name);

	if (dir_name == "..") {
		int ch_offset = pwd.find_last_of("/");
		if (ch_offset == 0)
			pwd = "/";
		else
			pwd.substr(0, ch_offset);
	}
	else {

		if(pwd != "/")
			pwd += "/";
		pwd += dir_name;
	}

}



std::string Interface::get_user_name() { return user_name; }

std::shared_ptr<std::fstream> Interface::get_file() { return file; }

I_node Interface::get_current_node() { return current_node; }

int64_t Interface::get_current_offset() { return current_offset; }

std::string Interface::get_dir_content() {

	std::string dir_content_str;

	for (auto el : dir_content) {
		dir_content_str += el.first;
		dir_content_str += "  ";
	}
		
	return dir_content_str;
}

std::string Interface::get_pwd() { return pwd; }

void Interface::close_file() {
	file->close();
}