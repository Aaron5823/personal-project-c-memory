JBOD (Just a Bunch Of Disks) System
This repository contains the implementation of a JBOD (Just a Bunch Of Disks) system in C. JBOD is a storage system that uses multiple disks to provide increased storage capacity.

Cache Implementation
The cache.c file contains the implementation of a cache system that enhances data access speed by storing frequently accessed disk blocks in memory. The cache uses a clock replacement policy to manage entries efficiently.

Functions
cache_create(int num_entries): Creates a cache with a specified number of entries.
cache_destroy(void): Destroys the cache and frees allocated memory.
cache_lookup(int disk_num, int block_num, uint8_t *buf): Looks up a block in the cache and returns it if found.
cache_update(int disk_num, int block_num, const uint8_t *buf): Updates a cache entry with new data.
cache_insert(int disk_num, int block_num, const uint8_t *buf): Inserts a new block into the cache.
cache_enabled(void): Checks if the cache is enabled.
cache_print_hit_rate(void): Prints the hit rate of the cache.
cache_resize(int new_num_entries): Resizes the cache to the specified number of entries.
MDADM (Multiple Devices Administration) Operations
The mdadm.c file contains the implementation of MDADM operations, which provide functionalities related to the management of multiple devices in the JBOD system.

Functions
mdadm_mount(void): Mounts the JBOD system.
mdadm_unmount(void): Unmounts the JBOD system.
mdadm_write_permission(void): Grants write permission for data modification.
mdadm_revoke_write_permission(void): Revokes write permission.
mdadm_read(uint32_t start_addr, uint32_t read_len, uint8_t *read_buf): Reads data from the JBOD system.
mdadm_write(uint32_t start_addr, uint32_t write_len, const uint8_t *write_buf): Writes data to the JBOD system.
Network Communication
The net.c file handles the network communication between the JBOD client and server.

Functions
nread(int fd, int len, uint8_t *buf): Attempts to read a specified number of bytes from a file descriptor.
nwrite(int fd, int len, uint8_t *buf): Attempts to write a specified number of bytes to a file descriptor.
recv_packet(int fd, uint32_t *op, uint8_t *ret, uint8_t *block): Attempts to receive a packet from a file descriptor.
send_packet(int sd, uint32_t op, uint8_t *block): Attempts to send a packet to a file descriptor.
jbod_connect(const char *ip, uint16_t port): Connects the JBOD client to the server.
jbod_disconnect(void): Disconnects the JBOD client from the server.
jbod_client_operation(uint32_t op, uint8_t *block): Performs a JBOD client operation.

Usage
Compile the JBOD system using the provided Makefile:
make

Run the JBOD client:
./jbod_client <server_ip> <server_port>
Note: Replace <server_ip> and <server_port> with the IP address and port number of the JBOD server.

Network Interface
net.h and net.c
The network interface provides basic functionality for connecting to and disconnecting from a server. Key features include:

Connection:

jbod_connect establishes a connection to the server.
jbod_disconnect disconnects from the server.
Packet Sending/Receiving:

nread and nwrite handle reading and writing data to a socket.
recv_packet and send_packet handle packet-level communication.
Key Observations:
Socket-based communication is used for connecting to and disconnecting from the server.
Packets are sent and received for higher-level communication.
Tester Component
tester.c
The tester component is a standalone program for testing the JBOD system with a specified workload. Key features include:

Command Line Arguments:

Accepts command line arguments for specifying cache size and workload file.
Workload Execution:

Reads commands from the workload file and executes corresponding operations.
Supports mounting, unmounting, read, and write operations.

JBOD System Tester
This tester is designed to evaluate the functionality of the JBOD (Just a Bunch Of Disks) system. It reads a workload file and performs various operations such as mounting, unmounting, read, write, and signaling.

Usage
./tester [-h] [-w workload-file] [-s cache_size]
Options
-h: Help mode (displays this message).
-w workload-file: Path to the workload file specifying the operations to be performed.
-s cache_size: Size of the cache (optional).
Workload Commands
The workload file should contain commands specifying the desired operations. Each line in the file corresponds to a single command. The supported commands are:

MOUNT: Mounts the JBOD system.
UNMOUNT: Unmounts the JBOD system.
WRITE_PERMIT: Grants write permission for data modification.
WRITE_PERMIT_REVOKE: Revokes write permission.
SIGNALL: Reads and prints all blocks from all disks.
For read and write operations, the following commands are used:

READ <start_addr> <read_len>: Reads data from the JBOD system starting from the specified address (start_addr) with the given length (read_len).
WRITE <start_addr> <write_len> <char>: Writes data to the JBOD system starting from the specified address (start_addr) with the given length (write_len) and character (char).
Example
./tester -w workload.txt -s 256
This command will run the tester using the workload specified in the workload.txt file and a cache size of 256.







