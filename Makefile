CC = g++
CFLAGS = -Wall -Wextra -std=c++11 #-DDEBUG_MODE
LDLIBS = -ljsoncpp
SRCDIR = src
OUTDIR = blockchains
SOURCES = $(wildcard $(SRCDIR)/*.cc)
HEADERS = $(wildcard $(SRCDIR)/*.h)
OBJECTS = $(SOURCES:.cc=.o)
EXECUTABLE = simulator

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@ $(LDLIBS)

%.o: %.cc $(HEADERS) args.json
	$(CC) $(CFLAGS) -c $< -o $@

run: all
	mkdir -p $(OUTDIR)
	./$(EXECUTABLE)
	# dot -Tpng $(OUTDIR)/*.dot -O
	dot -Tpng $(OUTDIR)/96.dot $(OUTDIR)/97.dot $(OUTDIR)/98.dot $(OUTDIR)/99.dot $(OUTDIR)/100.dot -O && rm $(OUTDIR)/*.dot

clean:
	rm -f $(SRCDIR)/*.o $(EXECUTABLE)
	rm -rf $(OUTDIR)