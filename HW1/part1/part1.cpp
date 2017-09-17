#include <iostream>
#include <time.h>
#include <limits.h>
#include <sys/time.h>
#include <mutex>
#include "../../snappy/snappy.h"
#include <memory>
#include <fstream>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define BILLION (1000000000)
#define MILLION (1000000)

#define KILO (1000)

using namespace std;

typedef struct timespec TimeSpec;
typedef struct timeval TimeVal;
typedef unsigned long long int uint64;
typedef unsigned char uint8;
typedef char int8;
typedef long long int int64;
typedef unsigned int uint32;
typedef pair<uint32, int> Resolution;


inline uint64 convert_to_time(const TimeSpec& time) {
	return (time.tv_sec * BILLION) + time.tv_nsec;
}

//-----------------------------------------------------------------------------

inline uint64 get_current_time() {
	TimeSpec time;
	clock_gettime(CLOCK_REALTIME, &time);
	return (time.tv_sec * BILLION) + time.tv_nsec;
}

//-----------------------------------------------------------------------------

string clock_name(int clock_id) {
	switch (clock_id) {
		case CLOCK_MONOTONIC: 
			return string("CLOCK_MONOTONIC");
		case CLOCK_REALTIME:
			return string("CLOCK_REALTIME");
		case CLOCK_MONOTONIC_RAW:
			return string("CLOCK_MONOTONIC_RAW");
	}
}

//-----------------------------------------------------------------------------

void print_cache_time() {
	TimeSpec start, end;
	uint64 min, max, sum;
	int iterations = 0;
	max = sum = 0;
	min = ULLONG_MAX;
	// Number fetched from lscpu
	int l1_size = 32 * 1024, l2_size = 256 * 1024, l3_size = 6144 * 1024;
	int total_size = l1_size + l2_size + l3_size;
	// leave some space in cache for other things.
	total_size -= (100 * 1024);

	unique_ptr<char[]> buffer = make_unique<char[]>(total_size);
	// Bring everything into cache with 0th index bytes present in l1 cache, 0th +
	// 32k index bytes present in L3 and 0th + 32k + 256k bytes present in l3.

	// hopefully all the cache hot by the end of this loop
	for(int i = total_size -1; i >= 0; --i) {
		buffer[i] = (i % 256);
	}

	iterations= 0;
	max = sum = 0;
	min = ULLONG_MAX;
	register int reg = 0;
	int helper = 0;
	char *buf = buffer.get();
	int i = 0;

	for (i = 0; i < l1_size; ++i) {
		iterations++;
		clock_gettime(CLOCK_REALTIME, &start);
		reg = buf[i];
		clock_gettime(CLOCK_REALTIME, &end);
		
		// making sure compiler doesn't optimize things. So, using the read value
		// from l1 cache.
		helper += reg;
		uint64 diff = convert_to_time(end) - convert_to_time(start);
		sum += diff;
		if (diff < min) { min = diff;}
		if (diff > max) { max = diff;}
	}
	
	cout << "L1 cache times :\t\t avg = "
			<< (static_cast<double>(sum) / static_cast<double>(iterations))
		<< " ns,\tmin = " << min << ",\tmax = " << max << endl;

	iterations= 0;
	max = sum = 0;
	min = ULLONG_MAX;
	int start_idx = i;
	for (; i < start_idx + l2_size; i += l1_size) {
		iterations++;
		clock_gettime(CLOCK_REALTIME, &start);
		reg = buf[i];
		clock_gettime(CLOCK_REALTIME, &end);
		
		// making sure compiler doesn't optimize things. So, using the read value
		// from l1 cache.
		helper += reg;
		uint64 diff = convert_to_time(end) - convert_to_time(start);
		sum += diff;
		if (diff < min) { min = diff;}
		if (diff > max) { max = diff;}
	}
	cout << "L2 cache times :\t\t avg = "
			<< (static_cast<double>(sum) / static_cast<double>(iterations))
		<< " ns,\tmin = " << min << ",\tmax = " << max << endl;

	iterations= 0;
	max = sum = 0;
	min = ULLONG_MAX;
	start_idx = i;
	for (; i < l3_size; i += l2_size) {
		iterations++;
		clock_gettime(CLOCK_REALTIME, &start);
		reg = buf[i];
		clock_gettime(CLOCK_REALTIME, &end);
		
		// making sure compiler doesn't optimize things. So, using the read value
		// from l1 cache.
		helper += reg;
		uint64 diff = convert_to_time(end) - convert_to_time(start);
		sum += diff;
		if (diff < min) { min = diff;}
		if (diff > max) { max = diff;}
	}
	cout << "L3 cache times :\t\t avg = "
			<< (static_cast<double>(sum) / static_cast<double>(iterations))
			<< " ns,\tmin = " << min << ",\tmax = " << max << ", helper = " << helper 
			<< endl;
}

//-----------------------------------------------------------------------------

void print_mutex_lock_time() {
	mutex mtx;
	uint64 min, max, sum;
	max = sum = 0;
	min = ULLONG_MAX;
	for (int repeat = 0; repeat < MILLION; repeat++) {
		TimeSpec start, end;
		clock_gettime(CLOCK_REALTIME, &start);
		mtx.lock();
		clock_gettime(CLOCK_REALTIME, &end);
		mtx.unlock();
		uint64 diff = convert_to_time(end) - convert_to_time(start);
		sum += diff;
		if (diff < min) { min = diff;}
		if (diff > max) { max = diff;}
	}

	cout << "Mutex lock times :\t\t avg = "
			<< (static_cast<double>(sum) / static_cast<double>(MILLION))
			<< " ns,\tmin = " << min << ",\tmax = " << max << endl;
}

//-----------------------------------------------------------------------------

void print_mutex_unlock_time() {
	mutex mtx;
	uint64 min, max, sum;
	max = sum = 0;
	min = ULLONG_MAX;
	for (int repeat = 0; repeat < MILLION; repeat++) {
		TimeSpec start, end;
		mtx.lock();
		clock_gettime(CLOCK_REALTIME, &start);
		mtx.unlock();
		clock_gettime(CLOCK_REALTIME, &end);
		uint64 diff = convert_to_time(end) - convert_to_time(start);
		sum += diff;
		if (diff < min) { min = diff;}
		if (diff > max) { max = diff;}
	}

	cout << "Mutex unlock times :\t\t avg = "
			<< (static_cast<double>(sum) / static_cast<double>(MILLION))
			<< " ns,\tmin = " << min << ",\tmax = " << max << endl;
}

//-----------------------------------------------------------------------------

void print_compress_uncompress_data(){
	uint64 min, max, sum;
	max = sum = 0;
	min = ULLONG_MAX;

	string input = "a";
	string output;

	// Generate 1k Data
	for (int i = 0; i < 1023; ++i) {
		string temp = " ";
		temp[0] = ' ' + (i % 95);
		input += temp;
	}

	for (int repeat = 0; repeat < KILO; repeat++) {
		TimeSpec start, end;
		// Compress
		clock_gettime(CLOCK_REALTIME, &start);
		snappy::Compress(input.data(), input.size(), &output);
		clock_gettime(CLOCK_REALTIME, &end);
		// cout << "input size:" << input.size() << " output size:"
		//			<< output.size() << endl;
		uint64 diff = convert_to_time(end) - convert_to_time(start);
		sum += diff;
		if (diff < min) { min = diff;}
		if (diff > max) { max = diff;}
	}
	cout << "Snappy 1k Compression Times : \t avg = "
		<< (static_cast<double>(sum) / static_cast<double>(KILO))
		<< " ns,\tmin = " << min << ",\tmax = " << max << endl;


	max = sum = 0;
	min = ULLONG_MAX;
	uint64 iter = 0;

	for (int repeat = 0; repeat < KILO; repeat++) {
		TimeSpec start, end;
		// Uncompress
		string output_uncom;

		clock_gettime(CLOCK_REALTIME, &start);
		snappy::Uncompress(output.data(), output.size(), &output_uncom);
		clock_gettime(CLOCK_REALTIME, &end);
		if (input == output_uncom) {
			// cout << "Equal" << endl;

			uint64 diff = convert_to_time(end) - convert_to_time(start);
			sum += diff;
			iter += 1;
			if (diff < min) { min = diff;}
			if (diff > max) { max = diff;}

		} else {
			cout << "ERROR:not equal" << endl;
		}
	}
	cout << "Snappy Decompress Times :\t avg = "
		<< (static_cast<double>(sum) / static_cast<double>(iter))
		<< " ns,\tmin = " << min << ",\tmax = " << max << endl;
}

//-----------------------------------------------------------------------------

void print_main_mem_reference_time() {
	// Assuming cache lines are 64 bytes. Plan is to access memory at a stride
	// length of 64 bytes so that the request always goes to memory. There may be
	// other factors which may in action that we're unware of.
	
	TimeSpec start, end;
	int _1_mb = 1024 * 1024;
	uint64 min, max, sum;
	max = sum = 0;
	min = ULLONG_MAX;
	
	// 1 GB
	int n = _1_mb * 1024 / sizeof(uint64);
	// skip 7 mb at once
	int skip = (_1_mb * 7) / sizeof(uint64);
	int iterations = 0;
	unique_ptr<uint64[]> ptr = make_unique<uint64[]>(n);
	for (register int i = 0; i < n; i += skip) {
		iterations++;
		clock_gettime(CLOCK_REALTIME, &start);
		// Access the memory
		ptr[i] = i;
		clock_gettime(CLOCK_REALTIME, &end);
		
		uint64 diff = convert_to_time(end) - convert_to_time(start);
		sum += diff;
		if (diff < min) { min = diff;}
		if (diff > max) { max = diff;}
	}
	cout << "Memory Access times :\t\t avg = "
			<< (static_cast<double>(sum) / static_cast<double>(iterations))
			<< " ns,\tmin = " << min << ",\tmax = " << max << endl;
}

//-----------------------------------------------------------------------------

void read_1mb_from_disk() {
	TimeSpec start, end;
	int _1_mb = 1024 * 1024 * 1;
	uint64 min, max, sum;
	max = sum = 0;
	min = ULLONG_MAX;
	int n = 1;
	for(int i = 0; i < n; ++i) {
		const char *test_file =
			static_cast<const char*>(string("test_file").c_str());
		// system("dd if=/dev/zero of=test_file bs=1G count=1 2> /dev/null");
			
		int read_fd = open(test_file, 0, "r");
		char *buffer = new char[_1_mb];
		clock_gettime(CLOCK_REALTIME, &start);
		int ret = read(read_fd, buffer, _1_mb);
		clock_gettime(CLOCK_REALTIME, &end);

		if (ret < 0 && ret != _1_mb) {
			cerr << "read returned with error: " << ret << endl;
		}
		close(read_fd);
		
		uint64 diff = convert_to_time(end) - convert_to_time(start);
		sum += diff;
		if (diff < min) { min = diff;}
		if (diff > max) { max = diff;}
	}
	cout << "#Read " << (_1_mb / (1024 * 1024)) 
			<< " MB from disk:\t\t avg = "
			<< (sum / n)
			<< " ns,\tmin = " << min << ",\tmax = " << max << endl;
}


int main() {
	print_cache_time();
	print_mutex_lock_time();
	print_mutex_unlock_time();
	print_main_mem_reference_time();
	print_compress_uncompress_data();
	read_1mb_from_disk();
	return 0;
}
