#include "syscall.h"
#include "stdio.h"
#include "stdlib.h"

#define BUFSIZE 1024
#define BUFSIZE2 300

char buf[BUFSIZE];
char buf2[BUFSIZE2];

int main(int argc, char** argv)
{
	testCreatWriteClose();
	testOpenClose();
	testRead();
	exit(0);
}

<<<<<<< .mine
int testCreatWriteClose() {
	char * fileName = "/Users/willywu/Desktop/newFile.c";
	int fileDesc = creat(fileName);
	if (fileDesc != -1) {
		printf("You successfully opened the file: %s", fileName);
	} else {
		printf("You failed to open %s", fileName);
	}
	buf[0] = 'W';
	buf[1] = 'i';
	buf[2] = 'l';
	buf[3] = 'l';
	buf[4] = 'y';
	buf[5] = '\n';
	write(fileDesc, buf, 10);
	close(fileDesc);
	return 0;
}

int testOpenClose() {
=======
int testCreatWriteClose() {
	char * fileName = "newFile.c";
	int fileDesc = creat(fileName);
	if (fileDesc != -1) {
		printf("You successfully opened the file: %s", fileName);
	} else {
		printf("You failed to open %s", fileName);
	}
	buf[0] = 'W';
	buf[1] = 'i';
	buf[2] = 'l';
	buf[3] = 'l';
	buf[4] = 'y';
	buf[5] = '\n';
	write(fileDesc, buf, 10);
	close(fileDesc);
	return 0;
}

int testOpenClose() {
>>>>>>> .r119
	char * fileName = "cat.c";
	int fileDesc = open(fileName);
	if (fileDesc != -1) {
		printf("You successfully opened the file: %s", fileName);
	} else {
		printf("You failed to open %s", fileName);
	}
	close(fileDesc);
	return 0;
}

int testRead() {
	char * fileName = "cat.c";
	int fileDesc = open(fileName);
	int bytesRead = read(fileDesc, buf, 20);
	bytesRead = read(fileDesc, buf, 100);
	bytesRead = read(fileDesc, buf, 0x800);
	close(fileDesc);
	return 0;
}

