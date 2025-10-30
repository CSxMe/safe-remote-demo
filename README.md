# Safe Remote Demo

A simple **Client–Server educational project** built in **C++ (Client)** and **Python (Server)**.  
The goal is to understand how network communication works — how a client sends commands to a server and receives responses — **in a safe and local environment** (between your own devices only).

---

## ⚙️ Project Structure


safe-remote-demo/
│
├── cstop-client/ # C++ Client project (Visual Studio)
│ ├── main.cpp
│ ├── client.cpp
│ └── client.h
│
└── server/ # Python Server (macOS)

---

## 🚀 How It Works

1. **Server (Python)**  
   - Runs on macOS.  
   - Listens for incoming connections on a specific port.  
   - Authenticates clients using a secret token.  
   - Safely handles received commands (only within a local sandbox directory).

2. **Client (C++)**  
   - Runs on Windows (e.g. inside Parallels).  
   - Connects to the server via IP address and port.  
   - Sends commands entered by the user to the server.  
   - Displays the server’s response in the console.

---

## 🧩 How to Run

### 1️⃣ Start the Server (on macOS)
```bash
cd ~/Desktop/safe-remote-demo/server
python3 server.py --port 5050 --auth my_secret_token_123 --sandbox ~/sandbox --log server.log
