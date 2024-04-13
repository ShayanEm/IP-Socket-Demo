/**
 * @file client.cpp
 * @author Shayan Eram
 */

#include "common.h"
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <fstream>

using namespace std;

/**
 * @brief Main function of the client program.
 *
 * @param argc Number of command-line arguments.
 * @param argv Array of command-line arguments.
 * @return int 0 if successful, non-zero otherwise.
 */
int main(int argc, char* argv[]) {

    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    socklen_t addr_len;

    // Check if the number of command-line arguments is sufficient
    if (argc < 4) {
        perror("Argements failed\0");
        return 0;
    }

    // Check if the length of the filename exceeds the maximum size
    if (strlen(argv[3]) > FILENAME_MAX_SIZE)
    {
        perror("Length of the filename exceeds the maximum size FILENAME_MAX_SIZE");
    }

    // Initialize the server address structure
    serv_addr.sin_family = AF_INET;
    inet_aton(argv[1], &serv_addr.sin_addr);
    portno = atoi(argv[2]);
    serv_addr.sin_port = htons(portno);

    // Create a socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // Check if socket creation failed
    if (sockfd < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Store the size of the server address structure for later use
    addr_len = sizeof(serv_addr);

    // Connect to the server
    int connectVerf = connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    // Check if connection failed
    if (connectVerf < 0) {
        perror("Connection Failed \n");
        exit(EXIT_FAILURE);
    }

    // Open the file for reading
    ifstream ifs(string(argv[3]).c_str(), ios::in | ios::binary);

    // Initialize the initialization packet
    init_packet ipkt;
    memset(&ipkt, 0, sizeof(init_packet));
    ipkt.msg = MSG_INIT;
    ipkt.filesize = getFileSize(argv[3]);
    string(argv[3]).copy(ipkt.filename, strlen(argv[3]));

    // Send the initialization packet to the server
    int writeVer = write(sockfd, &ipkt, sizeof(init_packet));

    // Receive the acceptance message from the server
    MESSAGE msg;
    int valread = recv(sockfd, &msg, sizeof(MESSAGE), MSG_WAITALL);

    // Check for errors in receiving the message
    if (valread < 0) {
        perror("Error receiving the message");
    }

    // Transfer the file
    long long int bytes_sent = 0;
    char buffer[BUFFER_SIZE];
    while (bytes_sent != ipkt.filesize)
    {
        ifs.read(buffer, BUFFER_SIZE);
        if (ifs.eof()) {
            n = write(sockfd, buffer, ipkt.filesize - bytes_sent);
        }
        else
            n = write(sockfd, buffer, BUFFER_SIZE);
        bytes_sent += n;
    }

    // Send the end message to the server to close the connection
    msg = MSG_END;
    int writeVerf = write(sockfd, &msg, sizeof(MESSAGE));
    if (writeVerf < 0) {
        perror("write return value is negative");
        exit(EXIT_FAILURE);
    }

    // Close the file and socket
    ifs.close();
    close(sockfd);

    return 0;
}
