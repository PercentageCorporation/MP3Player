#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <iostream>
#include <filesystem>
#include <thread>

#include "listdir.h"
#include "kbinput.h"
#include "playmp3.h"

// ****************************************************************************

void printUsage()
{
		printf("Usage: playmp3 <search path> [-d <search term>] [-s <search term>]  [<starting index>]\r\n");
		printf("  playmp3 <search path>\r\n");
		printf("    if <search path> is a directory, play all mp3 files in directory tree\r\n");
		printf("    if <search path> is an mp3 file, play it\r\n");
		printf("  Options:");
		printf("    -d : search all direcories with <search term> in name\r\n");
		printf("    -s : search all direcories for mp3 files with <search term> in name\r\n");
		printf("    startingIndex : start playing at startingIndex of files found\r\n");
}

bool isNumber(char* str)
{
	int len = strlen(str);
	if (len == 0) return false;
	for (int i = 0; i < len; ++i)
	{
		if (!isdigit(str[i])) return false;
	}
	return true;
}

int main(int argc, char** argv) 
{
	const char*	searchDirectory = NULL;
	const char*	dirSearchTerm = NULL;
	const char*	fileSearchTerm = NULL;
	int		startingIndex = 0;
	
	int ii = 0;
	if (argc < 2)
	{
		printUsage();
		return -1;
	}

	printf("argc: %d\r\n", argc);
	searchDirectory = argv[1];
	printf("searchDirectory: %s\r\n", searchDirectory);
	struct stat sb;
	if (stat(searchDirectory, &sb) != 0)
	{
		printf("Directory not found!\r\n");
		return -1;
	}
	
	if (argc == 3)
	{
		// assume parameter is startingIndex
		if (!isNumber(argv[2]))
		{
			printUsage();
			return -1;
		}
		startingIndex = atoi(argv[2]);
	}
	else if (argc >= 4)
	{
		int argIx = 2;
		do
		{
			printf("argv[%d]: %s\r\n", argIx, argv[argIx]);
			if (strcmp(argv[argIx], "-d") == 0)
			{
				dirSearchTerm = argv[++argIx];
			}
			else if (strcmp(argv[argIx], "-s") == 0)
			{
				fileSearchTerm = argv[++argIx];
			}
			else
			{
				if (!isNumber(argv[argIx]))
				{
					printUsage();
					return -1;
				}
				startingIndex = atoi(argv[argIx]);
			}
			++argIx;
		}
		while (argc > argIx);
	}
	
	if (dirSearchTerm != NULL)
	{
		printf("Searching for <%s> in directory names\r\n", dirSearchTerm);
	}
	if (fileSearchTerm != NULL)
	{
		printf("Searching for <%s> in file names\r\n", fileSearchTerm);
	}
		
    printf("Searching for keyboard...\r\n");
	init_keyboard();
    if (dev == NULL) 
	{
        printf("Keyboard not found");
		return -1;
    }
	
	printf("\r\nPlay directory: %s\r\n", searchDirectory);
	listdir(searchDirectory, dirSearchTerm, fileSearchTerm);
	sort(filelist.begin(), filelist.end());
	
	int index = 1;
	for (auto &fl : filelist)
	{
		printf("%2d - %s\n", index++, fl.c_str());
	}
	
	int numsongs = filelist.size();
	printf("%d files to play\r\n", numsongs);
	
	ii = startingIndex;
	if (startingIndex > 0)
	{
		printf("Starting with file %d\r\n", ii);
		--ii;
	}

	int* pause = new int(0);
	bool paused = false;
	int retval = 0;
	
	while(ii < numsongs)
	{
		const char* song = filelist[ii].c_str();
		printf("Playing: %s (%d)\r\n", song, ii);
	
		std::thread t2(play, (char*)song, std::ref(*pause));
		while(t2.joinable())
		{
			int k = getkey();
			//if (k != 0) printf("Key: %d\r\n", k);
			
			if (k == KEY_LEFT)		// previous song 105
			{
				retval = -1;
				*pause = 2;
				t2.join();
			}
			else if (k == KEY_RIGHT)	// next song 106
			{
				retval = 1;
				*pause = 2;
				t2.join();
			}
			else if (k == KEY_UP)	// vol up 103
			{
				*pause = 3;
			}
			else if (k == KEY_DOWN)	// vol down 108
			{
				*pause = 4;
			}
			else if (k == KEY_X)	// quit playing
			{
				retval = 86;
				*pause = 2;
				t2.join();
			}
			else if (k == KEY_SPACE)	// spacebar - pause 57
			{
				paused = !paused;
				*pause = paused ? 1 : 0;
			}
			else if (*pause == 85)
			{
				retval = 1;
				*pause = 0;
				t2.join();
			}
			else if (*pause == 86)
			{
				retval = 86;
				*pause = 2;
				t2.join();
			}
			else
			{
				//printf("Sleeping (%d)...\r\n", *pause);
				//sleep(1);
				usleep( 100000 );	// 100 ms
			}
		}
		printf("Finished playing (%d)\r\n", retval);

		if (retval == 86 ) break;
		ii += retval;
	}
	
	printf("Done!\r\n");
	
    return 0;
}
