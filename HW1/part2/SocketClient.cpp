#include "Socket.h"

void calculate_round_trip_times(ClientSocket& client) {
	int packet_sizes[] = {1024 * 1,	 1024 * 2,	1024 * 4, 1024 * 8,
												1024 * 16, 1024 * 32,	1024 * 63};

	int total_buffer_size = 0;
	for (int pkt_size : packet_sizes) {
		total_buffer_size += pkt_size;
	}

	const int iterations = 500;
	
	unique_ptr<char[]> buffer =	make_unique<char[]>(total_buffer_size);

	int skip_size = 0;
	for (int pkt_size : packet_sizes) {
		for(int i = 0; i < iterations; ++i) {
			client.ReadData(buffer.get() + skip_size, pkt_size);
		}
		skip_size += pkt_size;
	}
}

void calculate_bandwidth(ClientSocket& client) {
	int packet_sizes[] = {1024, 1024 *2, 1024 *4, 1024 * 8, 1024 * 16, 1024 * 32,
									1024 * 63};
	int pkt_sizes_len = (sizeof(packet_sizes) / sizeof(int));
	TimeSpec start, end;

	int pkt_size = packet_sizes[pkt_sizes_len - 1];
	unique_ptr<char[]> buffer = make_unique<char[]>(pkt_size);
	const int iterations = 2000;
	for(int i = 0; i < iterations; ++i) {
		client.ReadData(buffer.get(), pkt_size);
	}
}

void calculate_round_trip_with_drops(ClientSocket& client) {
	// seems to be best from the graph.
	int pkt_size = 1024 * 8;
	int iterations = 200;
	
	uint total_buffer_size = pkt_size * iterations;
	unique_ptr<char[]> buffer = make_unique<char[]>(total_buffer_size);

	int skip_size = 0;
	for (int drop_percent = 5; drop_percent < 95; drop_percent += 5) {
		client.SetDropPercentage(drop_percent);
		for(int i = 0; i < iterations; ++i) {
			client.ReadData(buffer.get() + skip_size, pkt_size);
		}
		skip_size += pkt_size;
	}
}

int main(int argc, char *argv[]) {
	if (argc < 4) {
		cerr << "Usage: SocketClient port server_ip_address drop_percentage" << endl;
		return 1;
	}
	ClientSocket client(atoi(argv[1]), argv[2], atoi(argv[3]));
	client.Connect();
	calculate_round_trip_times(client);
	calculate_bandwidth(client);
	calculate_round_trip_with_drops(client);
	return 0;
}
