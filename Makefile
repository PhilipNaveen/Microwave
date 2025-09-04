CXX = g++
CXXFLAGS = -std=c++14 -Wall -Wextra -O2
TARGET = microwave
SRCDIR = src
SOURCES = $(SRCDIR)/main.cpp $(SRCDIR)/tokenizer.cpp $(SRCDIR)/parser.cpp $(SRCDIR)/codegen.cpp

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES)

clean:
	rm -f $(TARGET) *.o

.PHONY: all clean
