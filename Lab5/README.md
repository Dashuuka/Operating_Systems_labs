# Lab5: Named Pipes File Access Manager

## Overview
This project implements a system for managing concurrent access to a file through named pipes in Windows. It demonstrates the "readers-writers problem" concurrency pattern, where multiple clients can read simultaneously, but write operations require exclusive access.

## Features
- Server manages file access and handles client requests through named pipes
- Multiple clients can connect simultaneously to read and modify employee records
- Concurrency control:
  - Multiple readers can access records simultaneously
  - Writers get exclusive access to records
  - Readers are blocked when a writer has access
  - Writers are blocked when any reader has access
- Windows-compatible named pipes implementation

## Components
- **Server**: Manages the file and client connections
- **Client**: Connects to the server to read and modify records
- **FileManager**: Handles the file operations and access control
- **Employee**: Data structure for employee records

## How to Use
1. Build the project using CMake
2. Run the server first: `server.exe`
3. The server will automatically launch client processes
4. Follow the prompts in the client to read or modify employee records

## Testing
The project includes comprehensive unit tests using Google Test framework that verify:
- File operations (create, read, update)
- Concurrency control mechanisms
- Multi-threaded access patterns

## Result
![image](https://github.com/user-attachments/assets/59bbcd98-146f-4944-80b5-f47f78f784f8)
