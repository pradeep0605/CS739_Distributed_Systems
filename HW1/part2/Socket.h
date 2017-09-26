#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <stdio.h>
#include <sched.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <time.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <errno.h>
#include <sys/resource.h>
#include <sys/mman.h>
#include <string>
#include <arpa/inet.h>
#include <iostream>
#include <memory>

using namespace std;

#define RD 0
#define WR 1

char output[1000] = {0};

#define socketperror(format, ...) \
	sprintf(output, format, ##__VA_ARGS__); \
	if (write(STDERR_FILENO, output, strlen(output)) == -1 || \
		fflush(stderr) < 0) \
		perror("Error in writing to STDERR\n");

#define SERVER_PORT 3141
typedef unsigned char uchar;
typedef unsigned int uint;
typedef unsigned int uint32;
typedef struct timespec TimeSpec;
typedef long long int longtime;
typedef long long int longint;
typedef unsigned long long int uint64;

#define BUFF_SIZE (1024 * 1024)
#define DATA_SIZE (1024 * 1024 * 64)
#define NUM_TRIALS (2000)
#define BILLION (1000000000)


/* Optimized macro */
#define get_current_time() \
	({ longtime global_time = 0;\
		TimeSpec global_res;\
	 clock_gettime(CLOCK_REALTIME, &global_res);\
	 global_time = (global_res.tv_sec * BILLION) + global_res.tv_nsec;\
	 global_time;\
	})

inline uint64 convert_to_time(const TimeSpec& time) {
	return (time.tv_sec * BILLION) + time.tv_nsec;
}

uint RandomNumber() {
	// srand(time(NULL));
	return rand() * get_current_time();
}

class ServerSocket {
	int sock_fd_;
	const int port_;
	struct sockaddr_in server_addr_;
	struct sockaddr_in client_addr_;
	bool accepted_connection_;
	int percent_drop_;

	bool DropPacket() {
		uint ranum = (RandomNumber() % 100);
		if (ranum < percent_drop_) {
			return true;
		}
		return false;
	}

	public:
	ServerSocket(int port, int percent_drop = 0) :
		port_(port),
		percent_drop_(percent_drop) {
		memset(&client_addr_, 0, sizeof(client_addr_));
		accepted_connection_ = false;
	}

//-----------------------------------------------------------------------------

	~ServerSocket() {
		Close();
	}

//-----------------------------------------------------------------------------

void SetDropPercentage(int percentage) {
	if (percentage < 0 || percentage > 99) {
		cerr << __LINE__ << ": Invalid packet drop percentage" << endl;
		return;
	}
	percent_drop_ = percentage;
}

//-----------------------------------------------------------------------------

	int	Bind() {
		int socket_type = SOCK_DGRAM;
		sock_fd_ = socket(AF_INET, socket_type, 0);
		if (sock_fd_ < 0) {
			cerr << __LINE__ << ": Unable to create socket. Error = " << sock_fd_
					<< endl;
			return sock_fd_;
		}
	
		int opt = 0;
		if (setsockopt(sock_fd_, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
					&opt, sizeof(opt))) {
			cerr << __LINE__ << " : setsockopt " << endl;
			exit(1);
		}

		int err = 0;
		memset(&server_addr_, 0, sizeof(server_addr_));
		server_addr_.sin_family = AF_INET;
		server_addr_.sin_port = htons(port_);
		server_addr_.sin_addr.s_addr = htonl(INADDR_ANY);

		if ((err = bind(sock_fd_, (struct sockaddr *) &server_addr_,
			sizeof(server_addr_))) < 0) {
			cerr << __LINE__ << ": Error binding to socket" << endl;
			return err;
		}
	}

//-----------------------------------------------------------------------------

	int Accept() {
		uint32 handshake;
		int ret = 0;
		accepted_connection_ = true;

		if (ret = ReadData(reinterpret_cast<char *>(&handshake), sizeof(uint32))
			< 0) {
			cerr << __LINE__ <<" : Unable to accept the connection" << endl;
			accepted_connection_ = false;
		}

		if (handshake != 0xdeadbeef) {
			cerr << __LINE__ << " : Handshake failed" << endl;
			accepted_connection_ = false;
			return -1;
		}

		return 0;
	}

//-----------------------------------------------------------------------------

	int WriteData(const char *buff, uint size) {
		if (accepted_connection_ == false) {
			cerr << __LINE__
				<< " : Unable to write since no connection is established."
				<< endl;
			return -1;
		}

		int ret = 0;
		bool ack_recvd = false;
		struct timeval tv;
		tv.tv_sec = 0;
		// 50 Miliseconds.
		tv.tv_usec = 50000;
		if (setsockopt(sock_fd_, SOL_SOCKET, SO_RCVTIMEO, &tv,sizeof(tv)) < 0) {
			cerr << __LINE__ << ": Error in setting timeout" << endl;
		}

		do {
			if (!DropPacket()) {
				if (ret = sendto(sock_fd_, buff, size, 0, (struct sockaddr *)
					&client_addr_, sizeof(client_addr_)) < 0) {
					cerr << __LINE__ << ": Error in send the packet" << endl;
					return ret;
				}
			}

			uint ack = 0;
			uint sock_len = sizeof(client_addr_);
			int recv_len = recvfrom(sock_fd_, reinterpret_cast<char *> (&ack),
				sizeof(uint), 0, (struct sockaddr *)&client_addr_, &sock_len);

			if (recv_len < 0) {
				// cout << __LINE__ << ": Receive timeout. Retrying.." << endl;
			}


			if (ack == 0xaced) {
				ack_recvd = true;
			}
		} while(!ack_recvd);

		// 50 thousand seconds;
		tv.tv_sec = 50000;
		tv.tv_usec = 0;
		if (setsockopt(sock_fd_, SOL_SOCKET, SO_RCVTIMEO, &tv,sizeof(tv)) < 0) {
			cerr << __LINE__ << ": Error in setting timeout" << endl;
		}

		return ret;
	}

//-----------------------------------------------------------------------------

	int ReadData(char *buff, uint size) {
		if (accepted_connection_ == false) {
			cerr << __LINE__ << ": Unable to read since no connection is established."
				<< endl;
			return -1;
		}

		uint sock_len = sizeof(client_addr_);;
		int recv_len = recvfrom(sock_fd_, buff, size, 0, (struct sockaddr *)
			&client_addr_, &sock_len);

		if (recv_len < 0) {
			cerr << __LINE__ << " : Error in receiving data" << endl;
			return recv_len;
		}
		
		/*
		printf("Received packet from %s:%d\n", inet_ntoa(client_addr_.sin_addr),
			ntohs(client_addr_.sin_port));
		*/

		uint ack = 0xaced;
		int ret = 0;
		if ((ret = sendto(sock_fd_, reinterpret_cast<char *> (&ack), sizeof(uint),
				0, (struct sockaddr *) &client_addr_, sizeof(client_addr_))) < 0) {
			cerr << __LINE__ << ": Error in sending the ack" << endl;
			return ret;
		}

		return recv_len;
	}

//-----------------------------------------------------------------------------

	void Close(void) {
		close(sock_fd_);
	}

};

//-----------------------------------------------------------------------------

class ClientSocket {
	int sock_fd_;
	const int server_port_;
	struct sockaddr_in server_addr_;
	struct sockaddr_in client_addr_;
	string server_ip_;
	bool accepted_connection_;
	int percent_drop_;

	int hostname_to_ip(const char* hostname, char* ip)	{
		struct hostent *he;
		struct in_addr **addr_list;
		int i;

		if ((he = gethostbyname( hostname ) ) == NULL) {
			// get the host info
			cerr << "gethostbyname" << endl;
			return 1;
		}
		addr_list = (struct in_addr **) he->h_addr_list;
		for(i = 0; addr_list[i] != NULL; i++) {
			//Return the first one;
			strcpy(ip , inet_ntoa(*addr_list[i]) );
			return 0;
		}
		return 1;
	}

//-----------------------------------------------------------------------------

	bool DropPacket() {
		uint ranum = (RandomNumber() % 100);
		if (ranum < percent_drop_) {
			return true;
		}
		return false;
	}

//-----------------------------------------------------------------------------

	public:
	// Default packet drop percentage is 10.
	ClientSocket(int port, string server_ip, int percent_drop = 0) :
		server_port_(port),
		server_ip_(server_ip),
		percent_drop_(percent_drop) {
		accepted_connection_ = false;
	}

//-----------------------------------------------------------------------------

	~ClientSocket() {
		Close();
	}

//-----------------------------------------------------------------------------

	void SetDropPercentage(int percentage) {
		if (percentage < 0 || percentage > 99) {
			cerr << __LINE__ << ": Invalid packet drop percentage" << endl;
			return;
		}
		percent_drop_ = percentage;
	}

	int	Connect() {
		int socket_type = SOCK_DGRAM;
		sock_fd_ = socket(AF_INET, socket_type, 0);
		int ret = 0;
		if (sock_fd_ < 0) {
			cerr << __LINE__ << ": Unable to create socket. Error = " << sock_fd_
					<< endl;
			return sock_fd_;
		}

		memset(&client_addr_, 0, sizeof(client_addr_));
		client_addr_.sin_family = AF_INET;
		client_addr_.sin_port = htons(0 /* server_port_ */ );
		client_addr_.sin_addr.s_addr = htonl(INADDR_ANY);

		if ((ret = bind(sock_fd_, (const struct sockaddr *) &client_addr_,
					sizeof(client_addr_))) < 0)	{
			cerr << __LINE__ << ": Client socket bind error" << endl;
			return -1;
		}

		memset(&server_addr_, 0, sizeof(server_addr_));
		server_addr_.sin_family = AF_INET;
		server_addr_.sin_port = htons(server_port_);

		if (inet_aton(server_ip_.c_str(), &server_addr_.sin_addr) == 0) {
			char ip[20] = {0};
			// If IP address not given directly, then convert the host name to IP
			// address format.
			if (hostname_to_ip(server_ip_.c_str(), ip) != 0) {
				cerr << __LINE__ << ": hostname to ip failed" << endl;
			}
			// Check again with the gotten IP address.
			if (inet_aton(ip, &server_addr_.sin_addr) == 0) {
				cerr << __LINE__ << ": inet_aton failed!" << endl;
				return -1;
			}
		}

		uint32 handshake = 0xdeadbeef;
		accepted_connection_ = true;
		if (ret = WriteData(reinterpret_cast<const char *>(&handshake),
			sizeof(uint32)) < 0) {
			cerr << __LINE__ <<": Write failed" << endl;
			accepted_connection_ = false;
			return ret;
		}
		return 0;
	}

//-----------------------------------------------------------------------------

	int WriteData(const char *buff, uint size) {
		if (accepted_connection_ == false) {
			cerr << __LINE__
				<< " : Unable to write since no connection is established."
				<< endl;
			return -1;
		}

		int ret = 0;
		bool ack_recvd = false;
		struct timeval tv;
		tv.tv_sec = 0;
		// 50 Miliseconds.
		tv.tv_usec = 50000;

		if (setsockopt(sock_fd_, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
			cerr << __LINE__ << ": Error in setting timeout" << endl;
		}

		do {
			if (!DropPacket()) {
				if (ret = sendto(sock_fd_, buff, size, 0, (struct sockaddr *)
					&server_addr_, sizeof(server_addr_)) < 0) {
					cerr << __LINE__ << ": Error in send the packet" << endl;
					return ret;
				}
			}

			uint ack = 0;
			uint sock_len = sizeof(server_addr_);
			int recv_len = recvfrom(sock_fd_, reinterpret_cast<char *> (&ack),
				sizeof(uint), 0, (struct sockaddr *)&server_addr_, &sock_len);

			if (recv_len < 0) {
				// cout << __LINE__ << ": Receive ack timeout. Retrying.." << endl;
			}

			if (ack == 0xaced) {
				ack_recvd = true;
			}
		} while (!ack_recvd);

		// 50 thousand seconds;
		tv.tv_sec = 50000;
		tv.tv_usec = 0;
		if (setsockopt(sock_fd_, SOL_SOCKET, SO_RCVTIMEO, &tv,sizeof(tv)) < 0) {
			cerr << __LINE__ << ": Error in setting timeout" << endl;
		}
		return ret;
	}

//-----------------------------------------------------------------------------

	int ReadData(char *buff, uint size) {
		if (accepted_connection_ == false) {
			cerr << __LINE__ << " : Unable to read since no connection is established."
				<< endl;
			return -1;
		}

		uint sock_len = sizeof(server_addr_);
		int recv_len = recvfrom(sock_fd_, buff, size, 0, (struct sockaddr *)
			&server_addr_, &sock_len);

		if (recv_len < 0) {
			cerr << __LINE__ << " : Error in receiving data" << endl;
			return recv_len;
		}

		uint ack = 0xaced;
		int ret = 0;
		if (ret = sendto(sock_fd_, reinterpret_cast<char *> (&ack), sizeof(uint),
				0, (struct sockaddr *) &server_addr_, sizeof(server_addr_)) < 0) {
			cerr << __LINE__ << ": Error in send the ack" << endl;
			return ret;
		}

		return recv_len;
	}

//-----------------------------------------------------------------------------

	void Close(void) {
		close(sock_fd_);
	}
};


int get_packet_size(char *s) {
	uint len = strlen(s);
	uint res = 0;
	if (s[len - 1] == 'm' || s[len - 1] == 'M') {
		s[len - 1] = '\0';
		res = ((atoi(s)) * 1024 * 1024);
		s[len - 1] = 'M';
		return res;
	} else if (s[len - 1] == 'k' || s[len - 1] == 'K') {
		s[len - 1] = '\0';
		res = ((atoi(s)) * 1024);
		s[len - 1] = 'K';
		return res;
	} else {
		return atoi(s);
	}
}



