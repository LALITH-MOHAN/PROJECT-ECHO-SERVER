# **Mini Project: Echo Server**

## **Project Overview**
The goal of this project is to develop an **Echo Server** in C that responds to client messages with the same message, the **current timestamp**, and a **count of total messages processed**. A client program will also be developed to interact with the server.

## **Features**
- **Client-Server Communication:** Server listens for client messages and echoes them back.
- **Timestamping:** Each response includes a **timestamp** from the client.
- **Message Counter:** The server tracks the **number of messages processed**.
- **Multi-client Support (Optional):** Can handle **multiple clients** using threading.

## **Technologies Used**
- **Language:** C  
- **Networking:** Sockets  

## **Implementation Details**
- **Server:** Listens for connections, receives messages, appends timestamp and count, and sends the response.
- **Client:** Connects to the server, sends messages, receives responses, and displays them.

