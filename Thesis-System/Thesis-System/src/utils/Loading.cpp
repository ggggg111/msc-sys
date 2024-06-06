#include <fstream>
#include <sstream>

#include "Loading.h"

std::vector<std::string> LoadLabelMap(const std::string& path)
{
	std::vector<std::string> categories;
	std::ifstream file(path);

	if(file.is_open())
	{
		std::string category;

		while(std::getline(file, category))
		{
			categories.push_back(category);
		}

		file.close();
	}

	return categories;
}