#include<iostream>
#include<stdlib.h>
#include<random>
#include <time.h>
#include "Socket.h"


using namespace std;
typedef unsigned int uint;

uint RandomNumber() {
	// srand(time(NULL));
	return rand() * get_current_time();
}

int main(int argc, char **argv) {
	if (argc < 2) {
		cout << "Usage: a.out percentage" << endl;
		return 0;
	}
	int percentage;
	int count = 0;
	int N = 100000;
	percentage = atoi(argv[1]);

	for (int i = 0; i < N; ++i) {
		uint ranum = RandomNumber() % 100;
		if (ranum < percentage) {
			count++;
		}
		// cout << ranum << endl;
	}
	cout << "Count = " << count << endl;
	return 0;
}
