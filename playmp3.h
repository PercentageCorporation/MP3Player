#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <thread>

#include <ao/ao.h>
#include <mpg123.h>

// compile with:  gcc mp3test.cpp -o mp3test -lmpg123 -lao -lstdc++

#define BITS 8

void play(char* audiofile, int& mp3_pause)
{
	// 1 - pause, 2 - exit
	mp3_pause = 0;
	
    mpg123_handle *mh;
    unsigned char *buffer;
    size_t buffer_size;
    size_t done;
    int err;

    int driver;
    ao_device *dev;

    ao_sample_format format;
    int channels, encoding;
    long rate;

    /* initializations */

    ao_initialize();
    driver = ao_default_driver_id();
    mpg123_init();
    mh = mpg123_new(NULL, &err);
	
    buffer_size = mpg123_outblock(mh);
    buffer = (unsigned char*) malloc(buffer_size * sizeof(unsigned char));

    /* open the file and get the decoding format */
    mpg123_open(mh, audiofile);
    mpg123_getformat(mh, &rate, &channels, &encoding);

    /* set the output format and open the output device */
    format.bits = mpg123_encsize(encoding) * BITS;
    format.rate = rate;
    format.channels = channels;
    format.byte_format = AO_FMT_NATIVE;
    format.matrix = 0;
	
    int saved_stderr = dup(STDERR_FILENO);
    int devnull = open("/dev/null", O_RDWR);
    dup2(devnull, STDERR_FILENO);  // Replace standard out
    dev = ao_open_live(driver, &format, NULL);
    dup2(saved_stderr, STDERR_FILENO);	

	printf("Starting play\r\n");
	double base;
	double really;
	double rva_db;
	mpg123_getvolume(mh, &base, &really, &rva_db);
	printf("Volume: %f / %f / %f\r\n", base, really, rva_db);
	
    /* decode and play */
	bool playing = true;
    while (playing)
	{
		if (mp3_pause == 1)
		{
			usleep( 200000 );	// 200 ms
		}
		else if (mp3_pause == 2)
		{
			playing = false;
			break;
		}
		else if (mp3_pause == 3)	// vol up
		{
			mp3_pause = 0;
			if (really < 10.0)
				mpg123_volume_change(mh, 0.1);
			mpg123_getvolume(mh, &base, &really, &rva_db);
			printf("Volume: %f / %f / %f\r\n", base, really, rva_db);
			continue;
		}
		else if (mp3_pause == 4)
		{
			mp3_pause = 0;
			if (really >= 0.0)
				mpg123_volume_change(mh, -0.1);
			mpg123_getvolume(mh, &base, &really, &rva_db);
			printf("Volume: %f / %f / %f\r\n", base, really, rva_db);
			continue;
		}
		else if (playing)
		{
			
			if (mpg123_read(mh, (char*)buffer, buffer_size, &done) != MPG123_OK) 
			{
				playing = false;
				mp3_pause = 85;
				break;
			}
			ao_play(dev, (char*)buffer, done);
		}
	}
	
	printf("Ending play\r\n");

    /* clean up */
    free(buffer);
    ao_close(dev);
    mpg123_close(mh);
    mpg123_delete(mh);
    mpg123_exit();
    ao_shutdown();

	printf("Play ended (%d)\r\n", mp3_pause);


}
