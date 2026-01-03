CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pthread -Iinclude
SRC = src/main.cpp \
      src/client_handler.cpp \
      src/http_parser.cpp \
      src/proxy_forward.cpp \
      src/blocklist.cpp \
      src/logger.cpp
TARGET = proxy
all: $(TARGET)
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)
clean:
	rm -f $(TARGET)
.PHONY: all clean
