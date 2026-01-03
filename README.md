# Custom Network Proxy Server

## Overview
This project implements a multithreaded HTTP forward proxy server in C++ using
low-level TCP socket programming. The proxy accepts HTTP client requests,
forwards them to destination servers, relays responses back to clients, and
enforces configurable domain-based access control with logging.

The proxy also supports HTTPS via the CONNECT method by establishing a
transparent TCP tunnel without decrypting TLS traffic.

---

## Features
- TCP-based HTTP forward proxy
- Thread-per-connection concurrency model
- HTTP request parsing (method, host, port)
- Domain-based blocking via configuration file
- HTTP 403 response for blocked domains
- HTTPS CONNECT tunneling (transparent TCP tunnel)
- Request logging with timestamp and client IP
- Modular and extensible codebase

---

## Directory Structure
```
proxy-project/
├── src/
├── include/
├── config/
├── logs/
├── tests/
├── docs/
├── Makefile
└── README.md
```

---

## Build Instructions

Compile the project using:
```bash
make
```

---

## Run

Start the proxy server:
```bash
./proxy
```

The proxy listens on:
```
127.0.0.1:8888
```

---

## Usage Examples

### HTTP request via proxy
```bash
curl -x http://127.0.0.1:8888 http://httpbin.org
```

### HTTPS request via CONNECT tunneling
```bash
curl -x http://127.0.0.1:8888 https://neverssl.com
```

### Blocked domain example
```bash
curl -v -x http://127.0.0.1:8888 http://example.com
```

---

## Domain Blocking

Blocked domains are listed in:
```
config/blocked_domains.txt
```

Each line contains one domain to block. Requests to blocked domains return:
```
HTTP/1.1 403 Forbidden
```

Domain blocking is enforced for both HTTP and HTTPS (CONNECT) requests.

---

## Logging

All proxy activity is logged to:
```
logs/proxy.log
```

Log format:
```
YYYY-MM-DD HH:MM:SS | client_ip | host:port | ACTION
```

Actions include:
- `ALLOWED` – HTTP request successfully forwarded
- `BLOCKED` – Request blocked by domain filter
- `TUNNEL` – HTTPS CONNECT tunnel established

---

## Limitations
- Only HTTP and HTTPS CONNECT are supported
- HTTPS traffic is not decrypted or inspected
- No caching or authentication
- No persistent connections (Connection: close)

---

## Author
Nakshatra Sihotiya
