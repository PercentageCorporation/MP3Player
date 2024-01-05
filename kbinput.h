#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <libevdev-1.0/libevdev/libevdev.h>

// compile using: gcc kbtest.cpp -o kbtest  -l:libevdev.so -lstdc++


#define DEVROOT     "/dev/input/"
#define DEVROOT_LEN 12
#define PATH_LEN    (DEVROOT_LEN + NAME_MAX)

using namespace std;

bool isKB(struct libevdev* dev) {
    return libevdev_has_event_type(dev, EV_KEY)
        && libevdev_has_event_type(dev, EV_REP);
}

struct libevdev* open_device(int fd) {
    struct libevdev* dev = libevdev_new();
    int err;

    if (dev == NULL) {
        errno = ENOMEM;
    } else if (0 > (err = libevdev_set_fd(dev, fd))) {
        libevdev_free(dev);
        dev = NULL;
        errno = -err;
    }

    return dev;
}

DIR* dir;
struct libevdev* dev = NULL;
struct input_event ev;

void init_keyboard()
{
    struct dirent* entry;
    char path[PATH_LEN];
    int fd;
    bool key, rep, alt;
	
    if (!(dir = opendir("/dev/input"))) {
        printf("No devices found\r\n");
		return;
    }
	
    // look for keyboard device
    while (entry = readdir(dir)) {
        if (DT_CHR == entry->d_type) {
            sprintf(path, "/dev/input/%s", entry->d_name);

            if (-1 == (fd = open(path, O_RDONLY|O_NONBLOCK))) {
                printf("Unable to open device");
				return;
            }

            if (dev = open_device(fd)) {
                if (isKB(dev)) 
					break;

				printf("Skipping Input: %s\r\n", path);
                libevdev_free(dev);
                dev = NULL;
            }
        }
    }
	
	if (dev != NULL)
		printf("Keyboard Input found: %s:%d\r\n", path, entry->d_type);

	
    closedir(dir);
}

int getkey()
{
	int err = libevdev_next_event(dev, LIBEVDEV_READ_FLAG_NORMAL, &ev);
	if (err == 0 && ev.value == 1 && ev.type == EV_KEY)
	{
		//printf("ev type: %d, code: %d, value: %d\r\n", ev.type, ev.code, ev.value);
		return ev.code;
	}
	return 0;
}	
