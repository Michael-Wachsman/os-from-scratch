# Network File Server

## Overview
This project implements a multi-threaded, secure network file server that allows clients to interact with a file system via encrypted network messages. The server supports concurrent file operations while maintaining filesystem consistency and security.

## Features
- **Secure Communication**: All messages encrypted using user passwords
- **Multi-threaded Design**: Handles concurrent client requests efficiently 
- **Session Management**: Prevents replay attacks using session/sequence numbers
- **File Operations**: Create, read, append, and delete files
- **Concurrency Control**: Optimized synchronization for parallel operations

## Technical Architecture

### Communication Protocol
The server handles five request types:
- `FS_SESSION`: Establishes a new session
- `FS_READ`: Reads data from a file at specified offset
- `FS_APPEND`: Appends data to an existing file
- `FS_CREATE`: Creates a new file
- `FS_DELETE`: Deletes an existing file

All requests and responses are encrypted, with each message preceded by a cleartext header containing metadata necessary for decryption.

### File System Structure
- Single-level directory (non-hierarchical)
- Inode-based architecture
- Fixed-size disk blocks
- In-memory caching of directory metadata and free block list

### Concurrency Model
- Reader-writer synchronization for filesystem operations
- Multiple parallel reads allowed on the same file
- Parallel operations on different files
- Write operations properly serialized to ensure consistency

## Usage

### Server
```
fs [port_number] < passwords
```
Where `passwords` is a file containing username/password pairs.

### Client
Applications must include `fs_client.h` and link with `libfs_client.a`:

```cpp
#include "fs_client.h"

// Initialize connection
fs_clientinit(server, server_port);

// Establish session
unsigned int session, seq = 0;
fs_session("user1", "password1", &session, seq++);

// Create file
fs_create("user1", "password1", session, seq++, "myfile");

// Append data
fs_append("user1", "password1", session, seq++, "myfile", data, size);

// Read data
fs_read("user1", "password1", session, seq++, "myfile", offset, buffer, size);

// Delete file
fs_delete("user1", "password1", session, seq++, "myfile");
```

## Implementation Details

### Encryption
The system supports two encryption modes:
- `CLEAR`: For debugging (set with `export FS_CRYPT=CLEAR`)
- `AES`: For secure operation (set with `export FS_CRYPT=AES`)

### Building
```
# Server
g++ fs.cc libfs_server.a -pthread -ldl -std=c++11

# Client application
g++ app.cc libfs_client.a -std=c++11
```

### Utilities
- `createfs`: Creates an empty filesystem
- `showfs`: Displays filesystem contents

## Performance Considerations
- Minimizes disk I/O through strategic caching
- Ordered write operations ensure filesystem consistency
- Optimized concurrency model balances parallelism with data integrity

# Acknowledgements
- Ibrahim Musaddequr Rahman 
- Ryan Chua