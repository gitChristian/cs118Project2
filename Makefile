CC=g++
CFLAGS=-I.
DEPS = # header file 
OBJ = server.o

server: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)
