CC=g++
CFLAGS=-I.
DEPS = # header file 
OBJ = webserver.o

webserver: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)
