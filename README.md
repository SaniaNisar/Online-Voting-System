# 🗳️ Online Voting System
This repository contains an implementation of a secure online voting system using a TCP multithreaded server-client program written in C. The system ensures that each registered voter can log in and cast their vote once and only once. The server validates voter credentials against a list of registered voters and allows authenticated voters to cast their votes securely.


# Features
### 🔒 Authentication:
Voters are authenticated using their name and CNIC number against a pre-defined list.
### 🔁 One Vote Per Voter: 
Each voter can cast their vote only once.
### 🛡️ Secure Voting: 
Only authenticated voters can access the voting interface.
### ⚙️ Multithreaded Server: 
The server can handle multiple clients concurrently.


# 📂 Files
### server.c: 
* The server-side program that handles client connections, voter authentication, and vote recording.
### client.c: 
* The client-side program that allows voters to connect to the server and cast their votes.
### Voters_List.txt: 
* A text file containing the list of registered voters in the format Name/CNIC number.
### Candidates_List.txt: 
* A text file containing the list of candidates and their poll symbols.
### Votes_Recorded.txt:
* A text file where the votes are recorded.


# 🛠️ Requirements
* GCC compiler (for compiling the C programs)
* Standard C libraries
