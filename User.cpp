#include "User.h"
#include <iostream>

User::User(const std::string& _name)
	: user_name{ _name }
	, intr{ nullptr }
{}

void User::create_fs(const std::string& path) {
	intr = std::make_shared<Interface>(user_name, path);
	intr->create_fs();
}

void User::open_fs(const std::string& path){
	intr = std::make_shared<Interface>(user_name, path);
	intr->open_fs();
}

void User::cd(const std::string dir_name) {
	intr->change_dir(dir_name);
}

void User::cat(const std::string& file_name) {
	std::cout << intr->read_file(file_name) << std::endl;
}
std::string User::ls() {
	return intr->get_dir_content();
}

std::string User::pwd() {
	return intr->get_pwd();
}


void User::rm(const std::string& name) {

	if (intr->get_enty_offset_by_name(name) == -1) {
		std::cout << "Directory not found" << std::endl;
		return;
	}
	intr->delete_enty(name);
}

void User::mkdir(const std::string& dir_name) {
	intr->create_dir(dir_name);
}

void User::touch(const std::string& file_name) {
	intr->create_file(file_name);
}

void User::write_to_file(const std::string file_name, const std::string& data) {
	intr->write_to_file(file_name, data);
}

std::shared_ptr<Interface> User::get_intr() {
	return intr;
}

void User::close_fs() {
	intr->close_file();
}