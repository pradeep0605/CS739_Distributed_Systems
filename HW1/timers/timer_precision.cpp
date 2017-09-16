#include<iostream>
#include <time.h>
#include <limits.h>
#include <sys/time.h>

#define BILLION (1000000000)
#define MILLION (1000000)
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

inline uint64 convert_to_time(const TimeVal& time) {
	return (time.tv_sec * MILLION) + time.tv_usec;
}

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

void clock_gettime_resolution(int clock_id) {
	TimeSpec start, end;
	uint64 sum = 0, iteration_count = 0;
	uint64 min = ULLONG_MAX, max = 0;
	uint64 min_iter = ULLONG_MAX, max_iter = 0;

	
	for (int repeat = 0; repeat < MILLION; ++repeat) {
		int n = 0;
		clock_gettime(clock_id, &start);
		clock_gettime(clock_id, &end);
		while ((convert_to_time(end) - convert_to_time(start)) <= 0) {
			clock_gettime(clock_id, &start);
			for(int i = 0; i < n; ++i) {}
			clock_gettime(clock_id, &end);
			n++;
		}
		uint64 diff = convert_to_time(end) - convert_to_time(start);
		sum += diff;
		iteration_count += n;
		if (diff < min) { min = diff; }
		if (diff > max) { max = diff; }
		if (n < min_iter) { min_iter = n;}
		if (n > max_iter) { max_iter = n;}
	}
	cout << "\t" << clock_name(clock_id) << " Resolution:\t\t" << " avg = " <<
		(static_cast<double>(sum) / static_cast<double>(MILLION)) << "ns"
		<< ", min = " << min << ", max = " <<  max << ", avg iter = " 
		<< (iteration_count / static_cast<double>(MILLION)) << ", min iter = "
		<< min_iter << ", max iter = " << max_iter << endl;
}

void gettimeofday_resolution() {
	TimeVal start, end;
	uint64 sum = 0, iteration_count = 0;
	uint64 min = ULLONG_MAX, max = 0;
	uint64 min_iter = ULLONG_MAX, max_iter = 0;

	
	for (int repeat = 0; repeat < MILLION; ++repeat) {
		int n = 0;
		gettimeofday(&start, NULL);
		gettimeofday(&end, NULL);
		while ((convert_to_time(end) - convert_to_time(start)) <= 0) {
			gettimeofday(&start, NULL);
			for(int i = 0; i < n; ++i) {}
			gettimeofday(&end, NULL);
			n++;
		}
		uint64 diff = convert_to_time(end) - convert_to_time(start);
		sum += diff;
		iteration_count += n;
		if (diff < min) { min = diff; }
		if (diff > max) { max = diff; }
		if (n < min_iter) { min_iter = n;}
		if (n > max_iter) { max_iter = n;}
	}
	cout << "\t" << "gettimeofday() Resolution:\t\t" << " avg = " <<
		(static_cast<double>(sum) / static_cast<double>(MILLION)) << "ms"
		<< ", min = " << min << ", max = " <<  max << ", avg iter = " 
		<< (iteration_count / static_cast<double>(MILLION)) << ", min iter = "
		<< min_iter << ", max iter = " << max_iter << endl;
}

int main() {
	cout << "clock_gettime() reslution:\n";
	clock_gettime_resolution(CLOCK_MONOTONIC);
	clock_gettime_resolution(CLOCK_REALTIME);
	clock_gettime_resolution(CLOCK_MONOTONIC_RAW);
	cout << "gettimeofday() reslution:\n";
	gettimeofday_resolution();
	return 0;
}
	
