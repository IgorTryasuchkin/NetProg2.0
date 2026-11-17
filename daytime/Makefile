CXX = g++
CXXFLAGS = -std=c++11 -Wall
TARGET = daytime-client
SRC = client.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

test: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) *.o

.PHONY: all test clean
