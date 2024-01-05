# C++ MP3 player for Raspberry Pi / Linux

## Usage: playmp3 \<search path\> [-d \<search term\>] [-s \<search term\>]  [\<starting index\>]  
- if \<search path\> is a directory, play all mp3 files in directory tree  
- if \<search path\> is an mp3 file, play it  
-   Options:  
-   -d : search all direcories with <search term> in name  
-   -s : search all direcories for mp3 files with <search term> in name  
-   startingIndex : start playing at startingIndex of files found  

This project is currently running on a "headless" Raspberry Pi 3B+ runing Rasbian Lite.  
There is an HDMI screen and a USB keyboard attached for configuration during first boot.

The following image was flashed using the balenaEtcher:  
  2023-12-11-raspios-bookworm-arm64-lite.img  

After flashing, SSH access was enabled using "sudo raspi-config".  
"/boot/cmdline.txt" was edited to add the following and remove the "console" references.  
```
quiet plymouth.ignore-serial-consoles logo.nologo vt.global_cursor_default=0
```
After reboot, the "tty" service was disabled.
```
sudo systemctl stop getty@tty1.service
sudo systemctl disable getty@tty1.service
```

I use a small Samsung USB keyboard for the player control input.  
For the keyboard input libraries I installed:  
```
sudo apt-get install libevdev-dev  
sudo apt-get install libudev-dev  
```

For the MP3 player libraries I installed:  
```
sudo apt-get install libmpg123-dev  
sudo apt-get install libao-dev  
```

The player output is currently the headphone jack on the Pi.  

Compile using:  
```
gcc playmp3.cpp -o playmp3 -lao -lmpg123 -lstdc++ -l:libevdev.so  
```

Assuming that you have a number of MP3 files in a directory named /home/pi/Music you can play them with the following command:
```
./playmp3 /home/pi/Music
```
The following keyboard commands are available:
- x - Exit the program
- LeftArrow - play previous file
- RightArrow - play next file
- UpArrow - increase volume
- DownArrow - decrease volume
