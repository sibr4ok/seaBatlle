CXX = g++

CXXFLAGS = -std=c++17 -Wall

LDFLAGS = -lsfml-graphics -lsfml-window -lsfml-system

SRCS = main.cpp Game.cpp Player.cpp Field.cpp Ship.cpp Cell.cpp Graphics.cpp

OBJS = $(SRCS:.cpp=.o)

TARGET = battleship

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

run: $(TARGET)
	./$(TARGET)