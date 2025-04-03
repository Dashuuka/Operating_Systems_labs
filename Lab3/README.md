# Lab3: Thread Synchronization with Critical Sections and Events  

This project demonstrates thread synchronization in a Windows environment using **critical sections** and **events** from the Windows API. The program consists of a **main thread** and multiple **marker threads** that compete to mark elements in a shared array.  

## Key Components  
- **Main Thread**:  
  - Allocates and initializes an integer array  
  - Launches multiple marker threads  
  - Manages thread synchronization using events  
  - Handles thread termination requests  

- **Marker Threads**:  
  - Attempt to mark array elements with their thread IDs  
  - Use critical sections for thread-safe array access  
  - Signal when unable to proceed (due to contention)  
  - Clean up marked elements upon termination  

## Synchronization Features  
✔ **Critical Sections** – Protect shared array from race conditions  
✔ **Events** – Coordinate thread execution and termination  
✔ **Deadlock Prevention** – Safe resource access patterns  

**Build System**: CMake  
**Testing Framework**: Google Test  

---

## Result:
![image](https://github.com/user-attachments/assets/ee67a7c4-52d1-4bc3-9cdf-d7dc7a01ebfd)
