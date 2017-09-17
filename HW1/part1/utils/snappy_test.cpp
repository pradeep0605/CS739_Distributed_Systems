#include<iostream>
#include <time.h>
#include <limits.h>
#include <sys/time.h>
#include <string>

#include "../../../snappy/snappy.h"
using namespace std;

#if 1
void compress_data(){
	string input = "Hello World";
	string output;
	for (int i = 0; i < 5; ++i) {
		input += input;
	}
	snappy::Compress(input.data(), input.size(), &output);
	cout << "input size:" << input.size() << " output size:"
		<< output.size() << endl;
}
#endif

int main() {
	compress_data();
	return 0;
}

