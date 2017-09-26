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
#include <string.h>

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

void print_l1_cache_time() {
	TimeSpec start, end;
	uint64 min, max, sum;
	int iterations = 0;
	max = sum = 0;
	min = ULLONG_MAX;
	// Number fetched from lscpu
	int l1_size = 32 * 1024, l2_size = 256 * 1024, l3_size = 6144 * 1024;
	// int total_size = l1_size + l2_size + l3_size;
	int total_size = l3_size;
	// leave some space in cache for other things.
	total_size -= (100 * 1024);

	// unique_ptr<char[]> buffer = make_unique<char[]>(total_size);
	// char *buf = buffer.get();
	char *buf = (char *) malloc(total_size);
	
	iterations= 0;
	max = sum = 0;
	min = ULLONG_MAX;
	int helper = 0;
	uint64 loop_time = 0;
	register int i = 0;
	register int reg = 0;

	for (int iter = 0; iter < KILO; ++iter) {
		TimeSpec loop_start, loop_end;

		clock_gettime(CLOCK_REALTIME, &loop_start);
		for (i = 0; i < l1_size; ++i) {}
		clock_gettime(CLOCK_REALTIME, &loop_end);
		
		loop_time += (convert_to_time(loop_end) - convert_to_time(loop_start));
	}

	loop_time = loop_time / KILO;

	// Bring everything into cache with 0th index bytes present in l1 cache, 0th +
	// 32k index bytes present in L3 and 0th + 32k + 256k bytes present in l3.

	// hopefully all the caches are hot by the end of this loop
	for(int p = 0; p < 100; p++) {
		for(int k = l1_size - 1; k >= 0; --k) {
			buf[k] = ((k + p) % 256);
		}
	}

	const int REPEAT = 1;
	for (int repeat = 0; repeat < REPEAT; repeat++) {
		clock_gettime(CLOCK_REALTIME, &start);
		for (i = 0; i < l1_size; ++i) {
			reg += buf[i];
		}
		clock_gettime(CLOCK_REALTIME, &end);

		iterations += i;
		// making sure compiler doesn't optimize things. So, using the read value
		// from l1 cache.
		helper += reg;
		uint64 diff = convert_to_time(end) - convert_to_time(start);
		sum += diff;
		if (diff < min) { min = diff;}
		if (diff > max) { max = diff;}

	}

	cout << "L1 cache times :\t\t avg = "
			<< (static_cast<double>(sum - (loop_time)) /
				static_cast<double>(iterations))
		<< " ns,\tmin = " << static_cast<double>(min - (loop_time / REPEAT))
			/ static_cast<double>(l1_size)
		<< ",\tmax = " << static_cast<double>((max - (loop_time / REPEAT)) /
			static_cast<double>(l1_size))
		<< " helper  = " << helper
		<< endl;
}

//-----------------------------------------------------------------------------

void print_l2_cache_time() {
	uint64 min, max, sum;
	int iterations = 0;
	max = sum = 0;
	min = ULLONG_MAX;
	// Number fetched from lscpu
	int l1_size = 32 * 1024, l2_size = 256 * 1024, l3_size = 6144 * 1024;
	// int total_size = l1_size + l2_size + l3_size;
	int total_size = l3_size;
	// leave some space in cache for other things.
	total_size -= (100 * 1024);

	// unique_ptr<char[]> buffer = make_unique<char[]>(total_size);
	// char *buf = buffer.get();
	char *buf = (char *) malloc(total_size);
	
	iterations= 0;
	max = sum = 0;
	min = ULLONG_MAX;
	int helper = 0;
	uint64 loop_time = 0;
	register int i = 0;
	register int reg = 0;

	int skip_size = 4096; 
	for (int iter = 0; iter < KILO; ++iter) {
		TimeSpec loop_start, loop_end;

		clock_gettime(CLOCK_REALTIME, &loop_start);
		for (i = l1_size; i < l1_size + l2_size; i += skip_size) {}
		clock_gettime(CLOCK_REALTIME, &loop_end);
		
		loop_time += (convert_to_time(loop_end) - convert_to_time(loop_start));
	}

	loop_time = loop_time / KILO;
	
	// Bring everything into cache with 0th index bytes present in l1 cache, 0th +
	// 32k index bytes present in L3 and 0th + 32k + 256k bytes present in l3.

	// hopefully all the caches are hot by the end of this loop
	for(int p = 0; p < 1; p++) {
		for(int k = l2_size + l1_size - 1; k >= 0; --k) {
			// buf[l2_size - k + 1] = ((k + p) % 256);
			buf[k] = ((k + p) % 256);
		}
	}

	const int REPEAT = 1;
	for (int repeat = 0; repeat < REPEAT; repeat++) {
		TimeSpec start, end;
		
		clock_gettime(CLOCK_REALTIME, &start);
		for (i = l1_size; i < l1_size + l2_size; i += skip_size) {
			reg += buf[i];
		}
		clock_gettime(CLOCK_REALTIME, &end);

		iterations += (l2_size) / skip_size;
		// making sure compiler doesn't optimize things. So, using the read value
		// from l1 cache.
		helper += reg;
		uint64 diff = convert_to_time(end) - convert_to_time(start);
		sum += diff;
		if (diff < min) { min = diff;}
		if (diff > max) { max = diff;}
	}

	cout << "L2 cache times :\t\t avg = "
			<< (static_cast<double>(sum - (loop_time)) /
				static_cast<double>(iterations))
		<< " ns,\tmin = " << static_cast<double>(min - (loop_time / REPEAT))
			/ static_cast<double>(l1_size)
		<< ",\tmax = " << static_cast<double>((max - (loop_time / REPEAT)) /
			static_cast<double>(l1_size))
		<< " helper  = " << helper
		<< endl;
}

//-----------------------------------------------------------------------------

void print_l3_cache_time() {
	uint64 min, max, sum;
	int iterations = 0;
	max = sum = 0;
	min = ULLONG_MAX;
	// Number fetched from lscpu
	int l1_size = 32 * 1024, l2_size = 256 * 1024, l3_size = 6144 * 1024;
	// int total_size = l1_size + l2_size + l3_size;
	int total_size = l3_size;
	// leave some space in cache for other things.
	total_size -= (100 * 1024);

	// unique_ptr<char[]> buffer = make_unique<char[]>(total_size);
	// char *buf = buffer.get();
	char *buf = (char *) malloc(total_size);
	
	iterations= 0;
	max = sum = 0;
	min = ULLONG_MAX;
	int helper = 0;
	uint64 loop_time = 0;
	register int i = 0;
	register int reg = 0;

	int skip_size = 1024; 
	for (int iter = 0; iter < KILO; ++iter) {
		TimeSpec loop_start, loop_end;

		clock_gettime(CLOCK_REALTIME, &loop_start);
		for (i = l1_size + l2_size; i < total_size; i += skip_size) {}
		clock_gettime(CLOCK_REALTIME, &loop_end);
		
		loop_time += (convert_to_time(loop_end) - convert_to_time(loop_start));
	}

	loop_time = loop_time / KILO;
	
	// Bring everything into cache with 0th index bytes present in l1 cache, 0th +
	// 32k index bytes present in L3 and 0th + 32k + 256k bytes present in l3.

	// hopefully all the caches are hot by the end of this loop
	for(int p = 0; p < 100; p++) {
		for(int k = total_size - 1; k >= 0; --k) {
			// buf[total_size - k + 1] = ((k + p) % 256);
			buf[k] = ((k + p) % 256);
		}
	}

	const int REPEAT = 1;
	for (int repeat = 0; repeat < REPEAT; repeat++) {
		TimeSpec start, end;
		
		clock_gettime(CLOCK_REALTIME, &start);
		for (i = l1_size + l2_size; i < total_size; i += skip_size) {
			reg += buf[i];
		}
		clock_gettime(CLOCK_REALTIME, &end);

		iterations += (l2_size) / skip_size;
		// making sure compiler doesn't optimize things. So, using the read value
		// from l1 cache.
		helper += reg;
		uint64 diff = convert_to_time(end) - convert_to_time(start);
		sum += diff;
		if (diff < min) { min = diff;}
		if (diff > max) { max = diff;}
	}

	cout << "L3 cache times :\t\t avg = "
			<< (static_cast<double>(sum - (loop_time)) /
				static_cast<double>(iterations))
		<< " ns,\tmin = " << static_cast<double>(min - (loop_time / REPEAT))
			/ static_cast<double>(l1_size)
		<< ",\tmax = " << static_cast<double>((max - (loop_time / REPEAT)) /
			static_cast<double>(l1_size))
		<< " helper  = " << helper
		<< endl;
}

//-----------------------------------------------------------------------------

void print_cache_time() {
	print_l1_cache_time();
	print_l2_cache_time();
	print_l3_cache_time();
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
	// skip 1 mb at once
	int skip = (_1_mb * 1) / sizeof(uint64);
	int iterations = 0;
	unique_ptr<uint64[]> ptr = make_unique<uint64[]>(n);
	uint64 *buf = ptr.get();

	// Make sure there will be no page faults.
	for (register int i = 0; i < n; i++) {
		buf[i] = i;
	}
	
	register int helper = 0;
	register int val = 0;
	for (register int i = 0; i < n; i += skip) {
		iterations++;
		clock_gettime(CLOCK_REALTIME, &start);
		// Access the memory
		val = buf[i];
		clock_gettime(CLOCK_REALTIME, &end);
		
		helper += val;
		uint64 diff = convert_to_time(end) - convert_to_time(start);
		sum += diff;
		if (diff < min) { min = diff;}
		if (diff > max) { max = diff;}
	}
	cout << "Memory Access times :\t\t avg = "
			<< (static_cast<double>(sum) / static_cast<double>(iterations))
			<< " ns,\tmin = " << min << ",\tmax = " << max << "helper = " << helper
			<< endl;
}

//-----------------------------------------------------------------------------

void read_1mb_from_disk() {
	TimeSpec start, end;
	int _1_mb = 1024 * 1024 * 1;
	uint64 min, max, sum;
	max = sum = 0;
	min = ULLONG_MAX;
	int n = 10;
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

void read_1mb_from_memory() {
	TimeSpec start, end;
	int _1_mb = 1024 * 1024;
	uint64 min, max, sum;
	max = sum = 0;
	min = ULLONG_MAX;
	int iterations = 0;
	for (int repeat = 0; repeat < KILO; repeat++) {
		unique_ptr<char[]> memory = make_unique<char[]>(_1_mb);
		unique_ptr<char[]> copy = make_unique<char[]>(_1_mb);
		char *buf = memory.get();
		char *copy_buf = copy.get();
		for (int i = 0; i < _1_mb; ++i) {
			buf[i] = (i % 256);
			copy_buf[i] = ((i  + 1) % 256);
		}
		iterations++;
		clock_gettime(CLOCK_REALTIME, &start);
		memcpy(copy_buf, buf, _1_mb);
		clock_gettime(CLOCK_REALTIME, &end);

		uint64 diff = convert_to_time(end) - convert_to_time(start);
		sum += diff;
		if (diff < min) { min = diff; }
		if (diff > max) { max = diff; }
	}
	cout << "Reading 1MB from Memory :\t avg = "
			<< (static_cast<double>(sum) / static_cast<double>(iterations))
			<< " ns,\tmin = " << min << ",\tmax = " << max << endl;
	

}

int main() {
	print_cache_time();
	print_mutex_lock_time();
	print_mutex_unlock_time();
	print_main_mem_reference_time();
	print_compress_uncompress_data();
	read_1mb_from_memory();
	read_1mb_from_disk();
	
	return 0;
}
