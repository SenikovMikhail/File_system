#include "Shell.h"
#include "User.h"
#include <string>
#include <iostream>
#include <limits>


Shell::Shell() {

	std::string file_name;
	std::string dir_name;
	
	std::cout << "Login: ";
	std::string user_name;
	std::cin >> user_name;

	User usr(user_name);

	std::string path;
	std::cout << "\nEnter path: ";
	std::cin >> path;
	usr.create_fs(path);

	std::string cmd;
	do {
		std::cout << usr.pwd().substr(0, usr.pwd().find_first_of('\0')) << ">";
		std::cin >> cmd;


		if (cmd == "ls" ) {

			std::cout << usr.ls() << std::endl;

		} else if (cmd == "pwd" ) {

			std::cout << usr.pwd() << std::endl;
		}
		else if (cmd == "cd") {

			dir_name.clear();
			std::cin >> dir_name;
			usr.cd(dir_name);

		} else if (cmd == "cat") {

			file_name.clear();
			std::cin >> file_name;
			std::cout << std::endl;
			usr.cat(file_name);

		} else if (cmd ==  "rm" ) {

			file_name.clear();
			std::cin >> file_name;
			std::cout << std::endl;
			usr.rm(file_name);

		} else if (cmd ==  "mkdir" ) {

			dir_name.clear();
			std::cin >> dir_name;
			std::cout << std::endl;
			usr.mkdir(dir_name);

		} else if (cmd == "touch" ) {

			file_name.clear();
			std::cin >> file_name;
			std::cout << std::endl;
			usr.touch(file_name);

		} else if (cmd == "write") {

			file_name.clear();
			std::cin >> file_name;
			std::cout << std::endl;

			if ( usr.get_intr()->get_enty_offset_by_name(file_name) == -1 ) {
				std::cout << "File not found" << std::endl;
				continue;
			}
			std::cout << "Enter data:" << std::endl;
			std::string data;
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::getline(std::cin, data);
			usr.write_to_file(file_name, data);

		} else {
			std::cout << "ERROR::command not found" << std::endl;
		}

	} while (cmd != "exit");

	usr.close_fs();

}