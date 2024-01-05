#include <string.h>
#include <string>
#include <cstring>
#include <vector>
#include <dirent.h>
#include <algorithm>

using namespace std;

vector<string> filelist;
vector<string> dirlist;

void listdir(const char* dirname, const char* dirSearch, const char* fileSearch)
{
    DIR *dir;
    struct dirent *dp;
	unsigned char type;

	printf("Searching: %s\n", dirname);
	
	dir = opendir(dirname);
	while ((dp=readdir(dir)) != NULL) {
		if ( !strcmp(dp->d_name, ".") || !strcmp(dp->d_name, "..") ) continue;

		char* name = dp->d_name;
		char path_name[1024];
		snprintf(path_name, sizeof(path_name), "%s/%s", dirname, name);

		if (dp->d_type == DT_DIR)
		{
			if (dirSearch == NULL || strcasestr(path_name, dirSearch))
			{
				dirlist.push_back(path_name);
				listdir(path_name, dirSearch, fileSearch);
			}
		}
		else
		{
			int len = strlen(name);
			if (len >3 && strcasecmp((const char*)&name[len-4],".mp3")==0)
				if (fileSearch == NULL || strcasestr(name, fileSearch))
					filelist.push_back(path_name);
		}
	}
	
	closedir(dir);
}
