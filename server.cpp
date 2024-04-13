/**
 * @file server.cpp
 * @author Shayan Eram
 */

#include "common.h"
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <cstring>
#include <fstream>

using namespace std;

/**
 * @brief Main function of the server program.
 *
 * @param argc Number of command-line arguments.
 * @param argv Array of command-line arguments.
 * @return int 0 if successful, non-zero otherwise.
 */
int main(int argc, char* argv[]) {

	int sockfd, newsockfd, portno;
	struct sockaddr_in serv_addr, cli_addr;
	socklen_t addr_len;
	int n;

	// Check if the number of command-line arguments is sufficient
	if (argc < 2) {
		perror("Pas assez argements\0");
		return 0;
	}

	// Convert the port number from string to integer
	portno = atoi(argv[1]);

	// Initialize server address structure
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);

	addr_len = sizeof(serv_addr);

	// Create a socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	// Check if socket creation failed
	if (sockfd < 0) {
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	// Bind the socket to the server address
	int bidVerif = bind(sockfd, (struct sockaddr*)&serv_addr, addr_len);

	// Check if binding failed
	if (bidVerif < 0) {
		perror("Bind failed");
		exit(EXIT_FAILURE);
	}

	// Listen for incoming connections
	listen(sockfd, 0);

	// Accept the client connection
	newsockfd = accept(sockfd, (struct sockaddr*)&cli_addr, &addr_len);

	// Check if accepting connection failed
	if (newsockfd < 0) {
		perror("Connection failed");
		exit(EXIT_FAILURE);
	}

	// Receive the initialization packet from the client
	init_packet ipkt;
	int valread = recv(newsockfd, &ipkt, sizeof(init_packet), MSG_WAITALL);

	// Check if received packet is an initialization packet
	if (ipkt.msg != MSG_INIT) {
		perror("Problem Initialization packet");
		close(newsockfd);
		close(sockfd);
	}

	// Open a binary ofstream for writing received data to a file
	ofstream ofs(string(ipkt.filename).c_str(), ios::out | ios::binary);

	// Send an acceptance message to the client
	MESSAGE msg = MSG_ACCEPT;
	write(newsockfd, &msg, sizeof(MESSAGE));

	// Receive and write data until all data is received
	long long int bytes_received = 0;
	char buffer[BUFFER_SIZE];
	while (bytes_received != ipkt.filesize)
	{
		if (BUFFER_SIZE > ipkt.filesize - bytes_received) {
			n = read(newsockfd, buffer, ipkt.filesize - bytes_received);
		}
		else {
			n = read(newsockfd, buffer, BUFFER_SIZE);
		}

		ofs.write(buffer, n);
		bytes_received += n;
	}

	// Receive end message
	int valread2 = recv(newsockfd, &msg, sizeof(MESSAGE), MSG_WAITALL);

	// Check if received message is the end message
	if (msg != MSG_END) {
		perror("problem end message");
	}

	cout << "Closing socket..." << endl;

	// Close the ofstream and the socket
	ofs.close();
	close(newsockfd);
	close(sockfd);

	return 0;
}
