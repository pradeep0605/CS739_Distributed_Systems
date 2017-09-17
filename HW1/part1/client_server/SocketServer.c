#include "../Generic.h"

// TimeSpec zero;
int global_socketid = 0;

int socket_create_and_accept(int port) {
	/* Create a socket for local communication (within the same device) */
	int option = 1;
	int err = 0;
	struct sockaddr_in server;
	int sfd = global_socketid = socket(AF_INET, SOCK_STREAM, 0);

	if (sfd < 0) {
		socketperror("Error %d: at line %d: socket creation\n", sfd, __LINE__);
		exit(1);
	}

	/* Set the socket option of TCP_NODELAY */
	if ((err = setsockopt(sfd, IPPROTO_TCP, TCP_NODELAY, &option, sizeof(int)))
		< 0) {
		socketperror("Error %d: at line %d: setsockopt\n", err, __LINE__);
		/*
		char msg[1000] = {0};
		explain_errno_setsockopt(sfd, IPPROTO_TCP, TCP_NODELAY,
		&option, sizeof(int));
		printf("%s", msg);
		*/
		exit(1);
	}

	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = port;
	server.sin_addr.s_addr = htonl(INADDR_ANY);

	/* Bind */
	if ((err = bind(sfd, (struct sockaddr *) &server, sizeof(server))) < 0) {
		socketperror("Error %d: at line %d: bind\n", err, __LINE__);
		exit(1);
	}

	/* Listen */
	if ((err = listen(sfd, 1)) < 0) {
		socketperror("Error %d: at line %d: listen\n", err, __LINE__);
		exit(1);
	}
	/* accpet */
	if ((err = accept(sfd, NULL, NULL)) < 0) {
		socketperror("Error %d: at line %d: accept\n", err, __LINE__);
		exit(1);
	}

	/* return the client fd */
	return err;
}

int main(int argc, char *argv[]) {
	int fd = -1;
	int i = 0, j = 0;
	int ret = 0;
	uint packet_size = 0;
	uint port = 8080;

	if (argc != 3) {
		socketperror("Usage: ServerSocket port packet_size\n");
		exit(0);
	}
	
	/* Set CPU 3 fixed for Socket Server */
	set_cpu_core(0 ,2);

	port = atoi(argv[1]);
	packet_size = get_packet_size(argv[2]);

	printf("Starting with packet size %u(%s) and port %u\n",packet_size,
		argv[2], port);

	/* Fill the buffers with some data */
	memset(_buffer, 'e', sizeof(uchar) * BUFF_SIZE);

	fd = socket_create_and_accept(port);
	// longtime min = INT_MAX, max = -1, sum = 0, start = 0, end = 0, diff = 0;


	/* Send the packet size to the client and client should send back the same
	 * numebr as the proof that client is following the protocol. This is like
	 * the first handshake between client and server before staring of any other
	 * transactions.
	 */
	if ((ret = write_full(fd, &packet_size, sizeof(packet_size))) < 0) {
		socketperror("Error %d: at line %d: write\n", ret, __LINE__);
		exit(1);
	}
	uint response = 0;
	if ((ret = read_full(fd, &response, sizeof(response))) < 0) {
		socketperror("Error %d: at line %d: read\n", ret, __LINE__);
		exit(1);
	}
	if (packet_size != response) {
		socketperror("Client didn't follow the protocol! response = %d\n",
			response);
		exit(1);
	}
	printf("Handshake successful !\n");
	
	memset(_buffer, 'e', sizeof(uchar) * BUFF_SIZE);

	uint64 min, max, sum;
	max = sum = 0;
	min = ULLONG_MAX;

	/* Calculate lanency */
	for (i = 0; i < NUM_TRIALS; ++i) {
		TimeSpec start, end;

		clock_gettime(CLOCK_REALTIME, &start);
		if ((ret = write_full(fd, _buffer, packet_size)) < 0) {
			socketperror("Error %d: at line %d: i = %d: write pktsize: %d\n",
			ret, __LINE__, i, packet_size);
		}
		clock_gettime(CLOCK_REALTIME, &end);

		while ((ret = read_full(fd, _buffer, packet_size)) < 0) {
			socketperror("Error %d: at line %d: i = %d: read pktsize: %d\n",
			ret, __LINE__, i, packet_size);
		}

		uint64 diff = convert_to_time(end) - convert_to_time(start);             
		sum += diff;                                                             
		if (diff < min) { min = diff;}                                           
		if (diff > max) { max = diff;} 

	}
	printf("=================== LATENCY %s ======================\n", argv[2]);
	printf("Minimum latency = %lld\n", min);
	printf("Maximum latency = %lld\n", max);
	printf("Average latency = %lld\n", (sum) / NUM_TRIALS);

	/* uint n = DATA_SIZE / packet_size;
	
	// To make less page faults
	memset(_buffer, 'e', sizeof(uchar) * BUFF_SIZE);
	sleep(0.5);

	start = get_current_time();
	for (i = 0; i < n; ++i) {
		if ((ret = write_full(fd, _buffer, packet_size)) < 0) {
			socketperror("Error %d: at line %d: i = %d: write pktsize: %d\n",
			ret, __LINE__, i, packet_size);
			socketperror("Reason %s\n", strerror(errno));
		}
	}
	if ((ret = read_full(fd, &response, sizeof(uint))) < sizeof(uint)) {
		socketperror("Error %d: at line %d: i = %d: read pktsize: %d\n",
		ret, __LINE__, i, packet_size);
	}
	end = get_current_time();
	if (response != 0xdeadbeef) {
		socketperror("Something went wrong ! Client response = %x\n", response);
	}
	printf("==================== THROUGHPUT %s ======================\n",
		argv[2]);
	printf("Data_Size = %u\nTime_Taken = %lld nano seconds\n",
		DATA_SIZE, (end - start));
	printf("Throughput = %Lf\n",
	 ( ((long double)(DATA_SIZE)) / (((long double)(end - start)) / BILLION )));
	 */

	close(global_socketid);
	close(fd);
	return 0;
}



