CC = g++
CFLAGS = -g -Wall -std=c++11 -I.
OBJS = main.o node.o scanner.o parser.o semantics.o
TARGET = P4

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $<

clean:
	del -f $(OBJS) $(TARGET)