#pragma once

#include "File_system.h"
#include "i_node.h"


#include <string>
#include <cstdint>
#include <memory>
#include <unordered_map>

class File_system;

class Interface {

public:

	Interface(const std::string& usr_name, const std::string& _path);

	void create_dir(const std::string& name);
	void read_dir(const std::string& name);
	void delete_dir(const std::string& name);

	void create_file(const std::string& name);
	std::string read_file(const std::string& name);
	void write_to_file(const std::string& name, const std::string& data);
	void delete_file(const std::string& name);

	int64_t get_enty_offset_by_name(const std::string& name);
	void delete_dir_content(const std::string& name);
	void add_dir_content(int64_t offset, const std::string& name);

	void create_fs();
	void open_fs();

	void delete_enty(const std::string& name);

	void change_dir(const std::string& dir_name);

	std::string get_user_name();
	std::shared_ptr<std::fstream> get_file();
	I_node get_current_node();
	int64_t get_current_offset();
	std::string get_dir_content();
	std::string get_pwd();

	void close_file();

private:

	std::string user_name;
	std::shared_ptr<std::fstream> file;
	std::shared_ptr<File_system> file_sys;
	std::string pwd;
	int64_t current_offset;
	I_node current_node;
	std::unordered_map<std::string, int64_t> dir_content;
};