#include "Socket.h"

void calculate_round_trip_times(ServerSocket& server) {
	int packet_sizes[] = {1024, 1024 *2, 1024 *4, 1024 * 8, 1024 * 16, 1024 * 32,
									1024 * 63};

	int total_buffer_size = 0;
	for (int pkt_size : packet_sizes) {
		total_buffer_size += pkt_size;
	}
	int iterations = 500;
	
	TimeSpec start, end;

	unique_ptr<char[]> buffer = make_unique<char[]>(total_buffer_size);

	for (int i = 0; i < total_buffer_size; ++i) {
		buffer[i] = i;
	}

	int skip_size = 0;
	for (int pkt_size : packet_sizes) {
		uint64 min = ULLONG_MAX;
		uint64 max = 0;
		uint64 sum = 0;
		clock_gettime(CLOCK_REALTIME, &start);	
		for(int i = 0; i < iterations; ++i) {
			server.WriteData(buffer.get() + skip_size, pkt_size);
		}
		clock_gettime(CLOCK_REALTIME, &end);	
		uint64 diff = convert_to_time(end) - convert_to_time(start);
		sum += diff;
		skip_size += pkt_size;
		cout << "RTTime for packet size " << pkt_size << " :\t\t" << "avg = "
			<< (sum / iterations) << endl;
	}
}

void calculate_bandwidth(ServerSocket& server) {
	int packet_sizes[] = {1024, 1024 *2, 1024 *4, 1024 * 8, 1024 * 16, 1024 * 32,
									1024 * 63};

	int pkt_sizes_len = (sizeof(packet_sizes) / sizeof(int));
	TimeSpec start, end;

	int pkt_size = packet_sizes[pkt_sizes_len - 1];
	unique_ptr<char[]> buffer = make_unique<char[]>(pkt_size);
	for (int i = 0; i < pkt_size; ++i) {
		buffer[i] = i;
	}

	const int iterations = 2000;
	uint64 min = ULLONG_MAX;
	uint64 max = 0;
	uint64 sum = 0;
	clock_gettime(CLOCK_REALTIME, &start);	
	for(int i = 0; i < iterations; ++i) {
		server.WriteData(buffer.get(), pkt_size);
	}
	clock_gettime(CLOCK_REALTIME, &end);
	uint64 diff = convert_to_time(end) - convert_to_time(start);
	sum += diff;
	cout.setf(std::ios::fixed,std::ios::floatfield);
	cout.precision(3);
	cout << "Bandwitdh for pkt size " << pkt_size << " :\t\t"
		<< ((static_cast<double>(pkt_size * iterations) /
			(static_cast<double>(sum) / BILLION)) /
				static_cast<double>(1024 * 1024)) << " MBps" << endl;
}


void calculate_round_trip_with_drops(ServerSocket& server) {
	// seems to be best from the graph.
	int pkt_size = 1024 * 8;
	int iterations = 200;
	
	TimeSpec start, end;
	
	uint total_buffer_size = pkt_size * iterations;
	unique_ptr<char[]> buffer = make_unique<char[]>(total_buffer_size);

	for (int i = 0; i < total_buffer_size ; ++i) {
		buffer[i] = i;
	}

	int skip_size = 0;
	for (int drop_percent = 5; drop_percent < 95; drop_percent += 5) {
		uint64 sum = 0;
		server.SetDropPercentage(drop_percent);
		clock_gettime(CLOCK_REALTIME, &start);	
		for(int i = 0; i < iterations; ++i) {
			server.WriteData(buffer.get() + skip_size, pkt_size);
		}
		clock_gettime(CLOCK_REALTIME, &end);	
		uint64 diff = convert_to_time(end) - convert_to_time(start);
		sum += diff;
		skip_size += pkt_size;
		cout << "RTTime for drop percent " << drop_percent << "% :\t\t" << "avg = "
			<< (sum / iterations) << endl;
	}
}


int main(int argc, char *argv[]) {
	if (argc < 3) {
		cerr << "Usage: SocketServer port drop_percentage" << endl;
		return 1;
	}
	ServerSocket server(atoi(argv[1]), atoi(argv[2]));
	server.Bind();
	server.Accept();

	calculate_round_trip_times(server);
	calculate_bandwidth(server);
	calculate_round_trip_with_drops(server);
	return 0;
}



