#pragma once

#include "Interface.h"
#include <string>

class User {

public:

	User(const std::string& _name);
	void create_fs(const std::string& path);
	void open_fs(const std::string& path);
	void cd(const std::string dir_name);
	void cat(const std::string& file_name);
	std::string ls();
	std::string pwd();
	void rm(const std::string& name);
	void mkdir(const std::string& dir_name);
	void touch(const std::string& file_name);
	void write_to_file(const std::string file_name, const std::string& data);

	std::shared_ptr<Interface> get_intr();

	void close_fs();

private:

	std::string user_name;
	std::shared_ptr<Interface> intr;


};