#!/bin/bash

echo "=== Allowed HTTP ==="
curl -x http://127.0.0.1:8888 http://httpbin.org

echo ""
echo "=== Blocked HTTP ==="
curl -v -x http://127.0.0.1:8888 http://example.com

echo ""
echo "=== Allowed HTTPS (CONNECT) ==="
curl -x http://127.0.0.1:8888 https://neverssl.com

echo ""
echo "=== Logs ==="
cat logs/proxy.log
