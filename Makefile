CC = g++
CFLAGS = -Wall -Wextra -std=c++11 -DDEBUG_MODE
SRCDIR = src
SOURCES = $(wildcard $(SRCDIR)/*.cc)
HEADERS = $(wildcard $(SRCDIR)/*.h)
OBJECTS = $(SOURCES:.cc=.o)
EXECUTABLE = simulator

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@

%.o: %.cc $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(SRCDIR)/*.o $(EXECUTABLE)