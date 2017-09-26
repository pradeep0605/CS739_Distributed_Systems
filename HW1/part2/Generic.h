#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <stdio.h>
#include <sched.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <time.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <errno.h>
#include <sys/resource.h>
#include <sys/mman.h>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>

using namespace std;

#define RD 0
#define WR 1

char output[1000] = {0};

#define socketperror(format, ...) \
	sprintf(output, format, ##__VA_ARGS__); \
	if (write(STDERR_FILENO, output, strlen(output)) == -1 || \
		fflush(stderr) < 0) \
		perror("Error in writing to STDERR\n");

#define shrdmem_perror(format, ...) \
	sprintf(output, format, ##__VA_ARGS__); \
	if (write(STDERR_FILENO, output, strlen(output)) == -1 || \
		fflush(stderr) < 0) \
		perror("Error in writing to STDERR\n");

#define SERVER_PORT 3141

typedef unsigned char uchar;
typedef unsigned int uint;
typedef struct timespec TimeSpec;
typedef long long int longtime;
typedef long long int longint;
typedef unsigned long long int uint64;

#define BUFF_SIZE (1024 * 1024)
#define DATA_SIZE (1024 * 1024 * 64)
#define NUM_TRIALS (2000)
#define BILLION (1000000000)

TimeSpec zero;
uchar _buffer[BUFF_SIZE]; /* 512k */
// uchar _data[DATA_SIZE]; /* 8 MB */

TimeSpec global_res;
longtime global_time;



/* Optimized macro */
#define get_current_time() \
	({ longtime global_time = 0;\
	 clock_gettime(CLOCK_REALTIME, &global_res);\
	 global_time = (global_res.tv_sec * BILLION) + global_res.tv_nsec;\
	 global_time;\
	})

inline uint64 convert_to_time(const TimeSpec& time) {                            
	return (time.tv_sec * BILLION) + time.tv_nsec;                               
}
                                                                                 
int get_packet_size(char *s) {
	uint len = strlen(s);
	uint res = 0;
	if (s[len - 1] == 'm' || s[len - 1] == 'M') {
		s[len - 1] = '\0';
		res = ((atoi(s)) * 1024 * 1024);
		s[len - 1] = 'M';
		return res;
	} else if (s[len - 1] == 'k' || s[len - 1] == 'K') {
		s[len - 1] = '\0';
		res = ((atoi(s)) * 1024);
		s[len - 1] = 'K';
		return res;
	} else {
		return atoi(s);
	}
}

//Function to affine a process to a CPU core
int
set_cpu_core(pid_t pid, int core_num) {
	/* Define cpu_set bit mask. */
	cpu_set_t my_set;
 	/* Initialize to 0,no CPUs selected. */
	CPU_ZERO(&my_set);
 	/* Set the bit that corresponding to core_num */
	CPU_SET(core_num, &my_set);
	/* Set affinity of current process to core_num */
	return sched_setaffinity(pid, sizeof(cpu_set_t), &my_set);
}

//Function to verify the affinity of process set
int
get_cpu_core(pid_t pid) {
//  pid_t pid = getpid();
  cpu_set_t my_set;
  int ret;

  CPU_ZERO(&my_set);
  ret = sched_getaffinity(pid, sizeof(my_set), &my_set);
  if (ret == 0) {
    char str[80];
    strcpy(str," ");
    int count = 0;
    int j;
    for (j = 0; j < CPU_SETSIZE; ++j) {
        if (CPU_ISSET(j, &my_set)) {
            ++count;
            char cpunum[3];
            sprintf(cpunum, "%d ", j);
            strcat(str, cpunum);
        }
    }
    printf("pid %d affinity has %d CPUs ... %s\n", pid, count, str);
  }
  return ret;
}

int write_full(int fd, void *buff, uint size) {
	int write_size = size;
	int rem = 0;
	while ((rem = write(fd, buff, write_size)) < write_size) {
		if (rem < 0) { return rem; }
		write_size -=rem;
		buff = (void*) (((char *)buff) + rem);
#if 1		
		printf("In loop. rem = %u, write_size = %u, size = %u\n", rem,
		write_size, size);
#endif
	}
	return size;
}

int read_full(int fd, void *buff, uint size) {
	int read_size = size;
	int rem = 0;
	while ((rem = read(fd, buff, read_size)) < read_size) {
		if (rem < 0) { return rem; }
		read_size -=rem;
		buff = (void*) (((char *)buff) + rem);
#if 1
		printf("In loop. rem = %u, read_size = %u, size = %u\n", rem,
		read_size, size);
#endif
	}
	return size;
}

/* As memory is shared, just read through the message */
#define GO_THROUGH 1

#if GO_THROUGH
#define faster_memcpy(d, s, length) \
	{ uint k = 0; uint* src = (uint *) s, *dest = (uint*) d;\
		for (; k < (length) / sizeof(uint); k+=1);\
	}

#else
#define faster_memcpy(d, s, length)\
	{ uint k = 0; uint* src = (uint *) s, *dest = (uint*) d;\
		for (; k < (length / sizeof(uint)); k+=1) \
			dest[k] = src[k];\
	}
#endif
