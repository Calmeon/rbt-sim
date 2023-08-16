CC = g++
FLAGS = -g -Wall

MAIN = main

# Build mediator and player program with all dependencies
all: $(MAIN).out

$(MAIN).out: $(MAIN).o
	$(CC) $(FLAGS) $^ -o $@

$(MAIN).o: $(MAIN).cpp
	$(CC) $(FLAGS) -c $^ -o $@

# Run simulation
run: $(MAIN).out
	./$(MAIN).out

# Run simulation with valgrind
memcheck: $(MAIN).out
	valgrind --leak-check=yes --track-origins=yes ./$(MAIN).out

# Delete files generated in process
clean:
	rm -f *.o  *.out
