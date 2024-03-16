/*
Программа должна иметь возможность осуществлять:
- копирование файлов,
- перемещение файлов,
- получение информации о файле (права, размер, время изме-
нения),
- изменение прав на выбранный файл.
- help
- 
*/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <filesystem>


void help_command()
{
    std::cout << "Available commands:" << std::endl;
	std::cout << "--help                                Shows all commands" << std::endl;
	std::cout << "cp [filename] [dest]                  File copy" << std::endl;
	std::cout << "mv [filename] [dest]                  File move" << std::endl;
	std::cout << "info [filename]                       File information" << std::endl;
	std::cout << "chmod [права/маска] [filename]        Change file permisions" << std::endl;
}

bool move_command(std::string full_file_name, std::string destination)
{
	std::ifstream file(full_file_name);
	if (!file.is_open()) // проверка на существование
	{
        std::cout << "File with this name does not exist\n";
        file.close();
        return 0;
	}
    int file_name_start = 0; // индексация с 0
    std::string file_path;
    // выделяем часть с директорией у названия файла
    for (int i = full_file_name.length() - 1; i > 0; --i)
    {
        if (full_file_name[i] == '/')
            file_name_start = i;
    }
    // выделяем часть с директорией у destination
    bool was_dot = false;
    int dest_start = 0; // индексация с 0
    for (int i = destination.length() - 1; i > 0; --i)
    {
        if (destination[i] == '.')
            was_dot = true;
        if (was_dot && destination[i] == '/')
            dest_start = i;
    }
    if (file_name_start) // если у нас полный путь
        file_path = full_file_name.substr(0, file_name_start); // копируем часть с директорией в отдельную строку
    else // иначе
            file_path = get_current_dir_name();
    if (file_path == destination)
    {
        std::cout << "You cant move a file into itself\n";
        return 0;
    }
    if (!dest_start) //если у нас не указано новое название файла, дописываем текущее
        destination += full_file_name.substr(file_name_start, full_file_name.size());
    std::cout << "Completed\n";
    return rename(full_file_name.c_str(), destination.c_str());
}

bool copy_command(std::string file_name, std::string dest)
{
	if (file_name == dest) // проверяем на копирование в себя
	{
		std::cout << "Man what's wrong with you" << std::endl;
		return 0;
	}
	std::ifstream in;
	std::ofstream out;
	in.open(file_name, std::ios::binary);
	if (in.is_open())
	{
		const int buffer_size = 4;
		char* buffer = new char[buffer_size];
		out.open(dest, std::ios::binary);
        while (!in.eof())
		{
		    in.read(buffer, buffer_size);
		    if (in.gcount()) // если что-то считали, записываем
		    	out.write(buffer, in.gcount());
	    }
		out.close();
		delete[] buffer;
	}
	else
	{
		std::cout << "File does not exist\n";
		return 0;
	}
	in.close();
    return 1;
}

void info_command(std::string file_name)
{
    struct stat file_stat;
    if(stat(file_name.c_str(), &file_stat) == 0)
    {
        std::cout << "File size: " << file_stat.st_size << " bytes\n";
		std::cout << "File permisions: ";
		std::cout << ((file_stat.st_mode & S_IRUSR) ? "r" : "-");
		std::cout << ((file_stat.st_mode & S_IWUSR) ? "w" : "-");
		std::cout << ((file_stat.st_mode & S_IXUSR) ? "x" : "-");
		std::cout << ((file_stat.st_mode & S_IRGRP) ? "r" : "-");
		std::cout << ((file_stat.st_mode & S_IWGRP) ? "w" : "-");
		std::cout << ((file_stat.st_mode & S_IXGRP) ? "x" : "-");
		std::cout << ((file_stat.st_mode & S_IROTH) ? "r" : "-");
		std::cout << ((file_stat.st_mode & S_IWOTH) ? "w" : "-");
		std::cout << ((file_stat.st_mode & S_IXOTH) ? "x" : "-") << std::endl;
		time_t modification_time = file_stat.st_mtime;
		std::cout << "Last change: " << asctime(localtime(&modification_time));
    }
}

bool is_chmod(std::string perms)
{
    if (perms.size() != 9)
        return false;
    for (int i = 0; i < 6; i+=3)
        {
            if (perms[0+i] != '-' && perms[0+i] != 'r')
                return false;
            else if (perms[1+i] != '-' && perms[1+i] != 'w')
                return false;
            else if (perms[2+i] != '-' && perms[2+i] != 'x')
                return false;
        }
    return true;
}

void chmod_command(std::string file_name, std::string perms)
{
    std::filesystem::perms new_perms = std::filesystem::perms::owner_all | std::filesystem::perms::group_all | std::filesystem::perms::others_all;
    if (perms[0] == 'r') new_perms |= std::filesystem::perms::owner_read;
	if (perms[1] == 'w') new_perms |= std::filesystem::perms::owner_write;
	if (perms[2] == 'x') new_perms |= std::filesystem::perms::owner_exec;
	if (perms[3] == 'r') new_perms |= std::filesystem::perms::group_read;
	if (perms[4] == 'w') new_perms |= std::filesystem::perms::group_write;
	if (perms[5] == 'x') new_perms |= std::filesystem::perms::group_exec;
	if (perms[6] == 'r') new_perms |= std::filesystem::perms::others_read;
	if (perms[7] == 'w') new_perms |= std::filesystem::perms::others_write;
	if (perms[8] == 'x') new_perms |= std::filesystem::perms::others_exec;
    std::filesystem::permissions(file_name.c_str(), new_perms);
    std::cout << "File permissions have been changed\n";
}

bool command_check(std::vector<std::string> &command) // смотрим что за команда
{
    std::string command_name = command[0];
    if (command_name == "--help" && command.size() == 1)
    {
        help_command();
        return 1;
    }
    if (command_name == "mv" && command.size() == 3)
    {
        move_command(command[1], command[2]);
        return 1;
    }
    if (command_name == "cp" && command.size() == 3)
    {
        copy_command(command[1], command[2]);
        return 1;
    }
    if (command_name == "info" && command.size() == 2)
    {
        info_command(command[1]);
        return 1;
    }
    if (command_name == "chmod" && is_chmod(command[1]))
    {
        chmod_command(command[2], command[1]);
        return 1;
    }
    else
    {
        std::cout << "Wrong command, use --help to see available commands\n";
        return 0;
    }
}

void command_input(std::vector<std::string> &command) // парсим ввод
{
    std::string input;
    getline(std::cin, input);
    size_t pos = 0;
    std::string command_part;
    while ((pos = input.find(' ')) != std::string::npos) // находим первое вхождение | npos - нет вхождений
    {
        command.push_back(input.substr(0, pos));
        input.erase(0, pos + 1);
    }
    command.push_back(input); // докидываем последнюю часть
}

char continue_input()
{
    char answ;
    std::cout << "Do you want to continue? y/n: ";
    std::cin >> answ;
    std::cin.ignore(256, '\n');
    while (answ != 'y' && answ != 'n')
    {
        std::cout << "Enter y or n!\n";
        std::cin >> answ;
        std::cin.ignore(256, '\n');
    }
    return answ;
}

void console_input()
{
    std::cout << "Use --help to see all commands\n";
    do
    {
        std::vector<std::string> command;
        command_input(command);
        command_check(command);
    } while (continue_input() == 'y');
}

int main(int permsc, char* permsv[])
{
    bool input = permsc;
    if (input)
        console_input();
    else
        std::cout << "Terminal input is not supported";
    return 0;
}