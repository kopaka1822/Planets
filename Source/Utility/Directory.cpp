#include "Directory.h"
#include <dirent.h>

std::vector< std::string > GetDirectoryFilenames(const std::string& folder)
{
	std::vector< std::string > names;

	DIR* dir;
	struct dirent* ent;
	if ((dir = opendir(folder.c_str())) != nullptr)
	{
		while ((ent = readdir(dir)) != nullptr)
			names.push_back(ent->d_name);

		closedir(dir);
	}
	return names;
}