CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++23
LDFLAGS = -ldl

INCLUDES = -I./src/


FILE = 01302019.NASDAQ_ITCH50
TARGET = main
PROFILE = test

SRC = $(wildcard src/*.cpp)
HEADERS = $(wildcard src/*.h)

$(TARGET) : $(SRC) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(TARGET) $(SRC) $(LDFLAGS)

$(PROFILE) : $(SRC) $(HEADERS)
	$(CXX) $(CXXFLAGS) -pg $(INCLUDES) -o $(PROFILE) $(SRC) $(LDFLAGS)


.PHONY : clean run profile
clean :
	rm $(TARGET)

run : main
	./main $(FILE)

profile : test
	./test $(FILE)
#gprof -b ./test gmon.out | c++filt > a.txt

clean_p :
	rm $(PROFILE)
