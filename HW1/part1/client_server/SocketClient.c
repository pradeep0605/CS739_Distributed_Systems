#include "../Generic.h"

int socket_create_and_connect(int port) {
	int fd = 0;
	int ret = 0;
	char *server_name = (char*) "rockhopper-07.cs.wisc.edu";
	struct in_addr server_addr;
	char *addr = (char*)"128.105.37.197";
	struct hostent *server_ptr;
	struct sockaddr_in server;

	if (inet_aton(addr, &server_addr) == 0) {
		fprintf(stderr, "Invalid address\n");
		exit(EXIT_FAILURE);
	}

	if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		socketperror("Error %d: at line %d: connect\n", fd, __LINE__);
		return fd;
	}

	if ((server_ptr = (struct hostent *) gethostbyaddr((const void *)&server_addr,
	sizeof(server_addr), AF_INET)) == NULL) {
		socketperror("Error %d: at line %d: gethostbyname\n", 0, __LINE__);
		return -1;
	}

	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = port;
	server.sin_addr.s_addr = ((struct in_addr *) 
		server_ptr->h_addr_list[0])->s_addr;

	if ((ret = connect(fd, (struct sockaddr *)&server, sizeof(server))) < 0) {
		socketperror("Error %d: at line %d: gethostbyname\n", ret, __LINE__);
		return ret;
	}

	return fd;
}

int main(int argc, char *argv[]) {
	int fd = 0;
	int ret = 0, i = 0;
	uint packet_size = 0;
	uint port = 0;

	if (argc != 2) {
		socketperror("Usage: ServerSocket port\n");
		exit(0);
	}
	
	/* Set CPU 4 fixed for Socket Server */
	set_cpu_core(getpid(), 3);
	
	port = atoi(argv[1]);

	
	if ((fd = socket_create_and_connect(port)) < 0) {
		socketperror("Error %d: at line %d: socket_create_and_connect\n", fd,
			__LINE__);
		exit(1);
	}

	/* Perform handshake protocol to get the packet size */
	read_full(fd, &packet_size, sizeof(uint));
	write_full(fd, &packet_size, sizeof(uint));

	/* ========================= Latency ===================== */
	/* Calculate lanency */
	memset(_buffer, 'e', sizeof(uchar) * BUFF_SIZE);
	for (i = 0; i < NUM_TRIALS; ++i) {
		if ((ret = read_full(fd, _buffer, packet_size)) < 0) {
			socketperror("Error %d: at line %d: i = %d: read pktsize: %d\n",
			ret, __LINE__, i, packet_size);
		}
		if ((ret = write_full(fd, _buffer, packet_size)) < 0) {
			socketperror("Error %d: at line %d: i = %d: read pktsize: %d\n",
			ret, __LINE__, i, packet_size);
		}
	}
	printf("Done with Latency transactions! \n"); 

	/* To make less page faults 
	memset(_buffer, 'e', sizeof(uchar) * BUFF_SIZE);

	int n = DATA_SIZE / packet_size;
	uint sum = 0;
	for (i = 0; i < n; ++i) {
		while ((ret = read_full(fd, _buffer, packet_size)) < 0) {
			socketperror("Error %d: at line %d: ret = %d\n",
			i, __LINE__, ret);
		}
		sum += packet_size;
	}
	uint response = 0xdeadbeef;
	if ((ret = write_full(fd, &response, sizeof(uint))) < 0) {
		socketperror("Error %d: at line %d: i = %d: read pktsize: %d\n",
		ret, __LINE__, i, packet_size);
	}
	printf("Done with throughput transactions! Bytes received = %u\n", sum);*/

	close(fd);
	return 0;
}
