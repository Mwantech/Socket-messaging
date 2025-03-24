# Multi-Client Socket Chat Application

## Overview

This is a simple multi-client socket chat application written in C, demonstrating basic network programming concepts using sockets and threading. The application allows multiple clients to connect to a server and exchange messages in real-time.

## Features

- Concurrent multi-client support
- Server-side broadcasting of messages
- Simple client-server communication
- Graceful client disconnection
- Lightweight and minimal dependencies

## Prerequisites

- GCC compiler
- POSIX threads (pthread) library
- Linux/Unix-based operating system

## Installation

1. Clone the repository:
```bash
git clone https://github.com/yourusername/socket-chat-app.git
cd socket-chat-app
```

2. Compile the application:
```bash
gcc -o socket_app socket_app.c -pthread
```

## Usage

### Running the Server
```bash
./socket_app server
```

### Running a Client
```bash
./socket_app client
```

You can run multiple client instances in different terminal windows to test multi-client communication.

## Supported Commands

- Type your message and press Enter to send
- Type `exit` to disconnect from the server

## Technical Details

- Uses TCP sockets for communication
- Implements thread-per-client model
- Supports up to 10 concurrent client connections
- Broadcasts messages to all connected clients
- Runs on localhost (127.0.0.1) on port 8080

## Limitations

- No persistent message storage
- No authentication mechanism
- Basic error handling
- Limited to local network communication

## Potential Improvements

- Add username support
- Implement private messaging
- Enhance error handling
- Add message encryption
- Create a more interactive client interface

## License

[Specify your license here, e.g., MIT License]

## Contributing

Contributions, issues, and feature requests are welcome. Feel free to check [issues page](https://github.com/yourusername/socket-chat-app/issues).

## Author

[Your Name]
