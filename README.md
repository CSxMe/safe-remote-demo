#  Safe Remote Demo

A lightweight and secure **remote command execution system** built with Python.  
It includes a **server** that listens for authenticated clients and a **client** that sends commands safely to be executed inside a sandboxed environment.

---

##  Features

-  **Authentication Token** — only trusted clients can connect.  
-  **Sandboxed Execution** — commands are executed in a restricted folder.  
-  **Logging System** — all actions are logged to `server.log`.  
-  **Multi-client Support** — multiple clients can connect at once.  
-  **Cross-platform** — works on macOS, Linux, and Windows.

---

##  Project Structure
safe-remote-demo/ server/server.py
safe-remote-demo/cstop-client/cstop-client.sln # C# client solution (optional)

---

##  Installation & Setup

###  Clone the Repository
```bash
git clone https://github.com/CSxMe/safe-remote-demo.git
cd safe-remote-demo


2️⃣ Create a Sandbox Folder
mkdir ~/sandbox
3️⃣ Start the Server
cd server
python3 server.py --port 5050 --auth my_secret_token_123 --sandbox ~/sandbox --log server.log


Expected output:

[INFO] Sandbox directory: /Users/yourname/sandbox
[INFO] Server listening on 0.0.0.0:5050. Waiting for clients...

💻 Run the Client
Example (Python client)
cd cstop-client
python3 client.py --host 127.0.0.1 --port 5050 --auth my_secret_token_123


You can now type safe commands (like ls, pwd, echo hello)
and the server will execute them inside the sandbox folder.

⚙️ Command Line Options
Server
Option	Description	Example
--port	Port number	--port 5050
--auth	Authentication token	--auth my_secret_token_123
--sandbox	Path to sandbox directory	--sandbox ~/sandbox
--log	Log file path	--log server.log
Client
Option	Description	Example
--host	Server address	--host 127.0.0.1
--port	Server port	--port 5050
--auth	Authentication token	--auth my_secret_token_123
🧠 Security Notes

Do NOT run this server outside a sandbox environment.

Use strong authentication tokens to prevent unauthorized access.

Avoid running privileged or destructive commands.

🧰 Future Improvements

Add encrypted communication (SSL/TLS)

Add file upload/download support

Create a web-based dashboard

Add Docker sandbox support
















