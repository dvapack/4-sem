#include<iostream>
#include<unistd.h>
#include<string>
#include<algorithm>
#include<vector>
#include<sstream>
#include <sys/stat.h>
#include<fstream>
#include<filesystem>


int main(int argc, char* argv[])
{
	std::string s = "scott";
	std::string delimiter = ">=";

	size_t pos = 0;
	std::string token;
	while ((pos = s.find(delimiter)) != std::string::npos) {
    	token = s.substr(0, pos);
    	std::cout << token << std::endl;
    	s.erase(0, pos + delimiter.length());
	}
	std::cout << s << std::endl;
}