# Lab4: Process Synchronization via File-Based Ring Buffer

This project demonstrates process synchronization in a Windows environment using a shared binary file as a **ring buffer (FIFO)** for message passing between processes.

## Key Components
- **Receiver**:  
  - Creates and initializes a binary file as a ring buffer (max 20-character messages)  
  - Launches multiple Sender processes  
  - Reads messages from the file upon user command

- **Sender**:  
  - Opens the shared file-based ring buffer  
  - Sends messages (entered via console) to the Receiver

## Synchronization Features  
✔ **File-Based IPC** – Uses a binary file as a shared ring buffer  
✔ **FIFO Queue** – Processes messages in the order they are sent  
✔ **Robust Error Handling** – Manages full/empty buffer and message length issues

**Build System**: CMake  
**Testing Framework**: Google Test

---

## Result:
![image](https://github.com/user-attachments/assets/b0c70bf2-0daf-44ba-b8d4-f9c62b089532)
