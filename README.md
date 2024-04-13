# File Transfer Server

This is a simple file transfer server application written in C++. It allows clients to transfer files to the server over a TCP/IP connection.

## Usage

### Server

To run the server, execute the `server` executable with the desired port number as the only argument:

```bash
./server <port_number>
```

### Client

To transfer a file to the server, execute the `client` executable with the server's IP address, port number, and the filename as arguments:

```bash
./client <server_ip> <port_number> <filename>
```

## Dependencies

- `common.h`: Header file containing common definitions used by both the client and server.

## How It Works

1. The server listens for incoming connections on the specified port.
2. When a client connects, the server accepts the connection and waits for an initialization packet.
3. The client sends an initialization packet containing the filename and size of the file to transfer.
4. Upon receiving the initialization packet, the server sends an acceptance message to the client.
5. The client starts transferring the file data in chunks to the server.
6. The server writes the received data to a file.
7. After receiving all data, the client sends an end message to signal the end of the transfer.
8. The server closes the connection and the file transfer is complete.

## Error Handling

- The server and client perform basic error handling for socket creation, binding, accepting connections, and file transfer.

## Notes

- The client must ensure that the file to transfer exists in the same directory as the client executable.
