# Default rule
all: mapper

# Rule to create the 'mapper' executable
mapper: mapper.o data.o
	gcc -o mapper mapper.o data.o

# Rule to create 'mapper.o'
mapper.o: mapper.c data.h
	gcc -c mapper.c

# Rule to create 'data.o'
data.o: data.c data.h
	gcc -c data.c

# Rule to create the 'testgraph' executable
testgraph: testgraph.o graph.o
	gcc -o testgraph testgraph.o graph.o

# Rule to create 'testgraph.o'
testgraph.o: testgraph.c testgraph.h graph.h
	gcc -c testgraph.c

# Rule to create 'graph.o'
graph.o: graph.c graph.h testgraph.h
	gcc -c graph.c

# Rule to create the 'citydata' executable
citydata: citydata.o graph.o data.o
	gcc -o citydata citydata.o graph.o data.o -lm

# Rule to create 'citydata.o'
citydata.o: citydata.c graph.h data.h
	gcc -c citydata.c

# Rule to clean up
clean:
	rm -f mapper mapper.o data.o testgraph testgraph.o graph.o citydata citydata.o

# Phony targets
.PHONY: all clean