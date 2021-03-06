/*
 * SO2 Lab - Linux device drivers (#4)
 * User-space test file
 */

#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "../include/so2_cdev.h"

#define DEVICE_PATH	"/dev/so2_cdev"

/*
 * prints error message and exits
 */

static void error(const char *message)
{
	perror(message);
	exit(EXIT_FAILURE);
}

/*
 * print use case
 */

static void usage(const char *argv0)
{
	printf("Usage: %s <options>\n options:\n"
			"\tp - print\n"
			"\ts string - set buffer\n"
			"\tg - get buffer\n"
			"\td - down\n"
			"\tu - up\n", argv0);
	exit(EXIT_FAILURE);
}

/*
 * Sample run:
 *  ./so2_cdev_test p		; print ioctl message
 *  ./so2_cdev_test d		; wait on wait_queue
 *  ./so2_cdev_test u		; wait on wait_queue
 */

int main(int argc, char **argv)
{
	int fd;
	int ret;

	if (argc < 2)
		usage(argv[0]);

	if (strlen(argv[1]) != 1)
		usage(argv[0]);

	fd = open (DEVICE_PATH, O_RDONLY);

	char buf[BUFFER_SIZE] = "ANA ARE MERE ROSII\n\0";

	switch (argv[1][0]) {
	case 'p':				/* print */
		ret = ioctl(fd, MY_IOCTL_PRINT);
		if (ret < 0) {
			printf("Error!\n");
		}

		break;
	case 's':				/* set buffer */
		if (argc < 3)
			usage(argv[0]);
		ioctl(fd, MY_IOCTL_SET_BUFFER, argv[2]);

		break;
	case 'g':				/* get buffer */
		ioctl(fd, MY_IOCTL_GET_BUFFER, buf);
		printf("User: got buffer: %s\n", buf);
		break;
	case 'd':				/* down */

		break;
	case 'u':				/* up */

		break;
	default:
		error("Wrong parameter");
	}

	close(fd);

	return 0;
}
