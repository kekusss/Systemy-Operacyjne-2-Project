CXXFLAGS =	-Wall -std=c++11 -g

OBJS =		main.o Car.o

LIBS =		-pthread -lncurses

TARGET =	cars

$(TARGET):	$(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS)

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
