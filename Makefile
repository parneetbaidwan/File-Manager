CXX := clang++
CXXFLAGS := -std=c++17 -Wall -Wextra -pedantic -O2
WXFLAGS := $(shell wx-config --cxxflags)
WXLIBS  := $(shell wx-config --libs)

TARGET := filemanager
SRC := src/FileManagerApp.cpp src/MainFrame.cpp
OBJ := $(SRC:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) $(WXFLAGS) -o $@ $^ $(WXLIBS)

src/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) $(WXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean
