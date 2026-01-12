CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++23
LDFLAGS = -lglfw -lGL -ldl

INCLUDES = -I./src/


TARGET = main
SRC = $(wildcard src/*.cpp)

all: $(TARGET)

$(TARGET) : $(SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(TARGET) $(SRC) $(LDFLAGS)

.PHONY : clean test
clean :
	rm $(TARGET)

test : main
	./main 01302019.NASDAQ_ITCH50
