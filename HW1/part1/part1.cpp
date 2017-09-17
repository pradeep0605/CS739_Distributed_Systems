#include<iostream>
#include <time.h>
#include <limits.h>
#include <sys/time.h>
#include <mutex>
#include "../../snappy/snappy.h"

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
			<< "ns, min = " << min << ", max = " << max << endl;
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
			<< "ns, min = " << min << ", max = " << max << endl;
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
	cout << "Snappy Compression Times : \t\t avg = "
		<< (static_cast<double>(sum) / static_cast<double>(KILO))
		<< "ns, min = " << min << ", max = " << max << endl;


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
	cout << "Snappy Decompress Times :\t\t avg = "
		<< (static_cast<double>(sum) / static_cast<double>(iter))
		<< "ns, min = " << min << ", max = " << max << endl;
}

//-----------------------------------------------------------------------------

void print_main_mem_reference_time() {
	
}

//-----------------------------------------------------------------------------

int main() {
	print_mutex_lock_time();
	print_mutex_unlock_time();
	print_main_mem_reference_time();
	print_compress_uncompress_data();
	return 0;

}
