# Design Document – Custom Network Proxy Server

## 1. Introduction
This document describes the design and architecture of the Custom Network Proxy
Server implemented in C++. The proxy is designed to handle HTTP traffic and
HTTPS traffic using the CONNECT tunneling mechanism. It supports multiple
clients concurrently, enforces domain-based access control, and logs all
requests for monitoring and analysis.

---

## 2. Overall Architecture
The proxy server follows a modular, layered architecture. Each major
responsibility of the proxy is isolated into a separate module, making the
system easier to understand, extend, and debug.

At a high level, the proxy operates as an intermediary between clients and
destination servers:

Client → Proxy Server → Destination Server → Proxy Server → Client

---

## 3. Major Components

### 3.1 main.cpp
- Initializes the TCP server socket
- Binds the socket to `127.0.0.1:8888`
- Listens for incoming client connections
- Accepts connections in a loop
- Spawns a new thread for each client connection

This file contains no application logic and is responsible only for server
setup and connection management.

---

### 3.2 client_handler
- Handles a single client connection
- Parses incoming HTTP or CONNECT requests
- Applies domain-based blocking rules
- Routes requests to either HTTP forwarding or HTTPS tunneling
- Logs request activity
- Closes the client connection safely

Each client connection is handled independently in its own thread.

---

### 3.3 http_parser
- Performs minimal HTTP request parsing
- Extracts:
  - Request method (GET, POST, CONNECT)
  - Host name
  - Port number
  - Raw request data
- Stops parsing after HTTP headers are received

This parser is intentionally lightweight and designed to support only the
features required by the proxy.

---

### 3.4 proxy_forward
This module handles communication with destination servers.

It provides two main functionalities:

#### HTTP Forwarding
- Establishes a TCP connection to the destination server
- Sends the raw HTTP request
- Relays the response back to the client

#### HTTPS CONNECT Tunneling
- Handles CONNECT requests
- Establishes a TCP connection to the destination server on port 443
- Responds with `200 Connection Established`
- Transparently forwards encrypted data in both directions
- Does not decrypt or inspect TLS traffic

---

### 3.5 blocklist
- Reads blocked domains from `config/blocked_domains.txt`
- Applies exact string matching on host names
- Enforces blocking for both HTTP and HTTPS requests
- Returns an HTTP 403 response for blocked domains

This ensures that blocked domains cannot be accessed even over encrypted
connections.

---

### 3.6 logger
- Logs all proxy activity to `logs/proxy.log`
- Each log entry includes:
  - Timestamp
  - Client IP address
  - Destination host and port
  - Action taken (ALLOWED, BLOCKED, TUNNEL)

Logging is persistent and append-only.

---

## 4. Concurrency Model
The proxy uses a **thread-per-connection** concurrency model implemented using
`std::thread`.

- Each incoming client connection is handled by a separate thread
- Threads are detached after creation
- This model simplifies request handling and avoids shared-state complexity
- Suitable for moderate workloads and academic use cases

---

## 5. Request Processing Flow

1. Client establishes a TCP connection to the proxy
2. Proxy parses the incoming request
3. Domain blocklist is checked
4. If the domain is blocked:
   - HTTP 403 Forbidden is returned
   - Request is logged as BLOCKED
5. If request method is CONNECT:
   - HTTPS tunnel is established
   - Encrypted data is forwarded bidirectionally
   - Request is logged as TUNNEL
6. Otherwise:
   - HTTP request is forwarded to destination server
   - Response is relayed back to client
   - Request is logged as ALLOWED

---

## 6. HTTPS CONNECT Handling
The proxy supports HTTPS by implementing CONNECT tunneling.

- The proxy does not decrypt HTTPS traffic
- It only establishes a TCP tunnel between client and server
- All encrypted data is forwarded transparently
- Domain-based blocking is enforced before tunneling

This design maintains security and avoids TLS interception.

---

## 7. Error Handling
- Socket errors result in immediate connection termination
- Invalid or incomplete requests are safely dropped
- Failed connections to destination servers are logged
- The proxy remains operational despite individual request failures

---

## 8. Security Considerations
- No inspection or modification of HTTPS payloads
- Domain-based access control applied consistently
- No credential storage or authentication
- Operates only on localhost by default

---

## 9. Limitations
- Only HTTP and HTTPS CONNECT are supported
- No caching of responses
- No authentication mechanism
- No persistent connections (Connection: close)
- Designed for academic and learning purposes

---

## 10. Conclusion
The Custom Network Proxy Server demonstrates core networking concepts such as
TCP socket programming, concurrency, request parsing, access control, and HTTPS
tunneling. The modular design ensures clarity, maintainability, and ease of
extension while remaining aligned with project requirements.
