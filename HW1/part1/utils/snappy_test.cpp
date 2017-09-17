#include<iostream>
#include <time.h>
#include <limits.h>
#include <sys/time.h>
#include <string>

#include "../../../snappy/snappy.h"
using namespace std;

#if 1
void compress_data(){
	string input = "a";
	string output;
	
	// Generate 1k Data
	for (int i = 0; i < 1023; ++i) {
		string temp = " ";
		temp[0] = ' ' + (i % 95);
		input += temp;
	}

	// Compress
	snappy::Compress(input.data(), input.size(), &output);
	cout << "input size:" << input.size() << " output size:"
		<< output.size() << endl;
	
	// Uncompress
	string output_uncom;
	snappy::Uncompress(output.data(), output.size(), &output_uncom);
	if (input == output_uncom) {
		cout << "Equal" << endl;
	} else {
		cout << "ERROR: not equal" << endl;
	}
}
#endif

int main() {
	compress_data();
	return 0;
}

