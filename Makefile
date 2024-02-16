CC = g++
CFLAGS = -Wall -Wextra -std=c++11 #-DDEBUG_MODE
SRCDIR = src
OUTDIR = blockchains
SOURCES = $(wildcard $(SRCDIR)/*.cc)
HEADERS = $(wildcard $(SRCDIR)/*.h)
OBJECTS = $(SOURCES:.cc=.o)
EXECUTABLE = simulator

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@

%.o: %.cc $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

run: all
	mkdir -p $(OUTDIR)
	./$(EXECUTABLE)
	dot -Tpng $(OUTDIR)/*.dot -O
	rm -f $(OUTDIR)/*.dot

clean:
	rm -f $(SRCDIR)/*.o $(EXECUTABLE)
	rm -rf $(OUTDIR)