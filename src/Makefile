CC = g++
FLAGS = -g -Wall

all: main.out

main.out: main.o car.o roundabout.o utils.o settings.o agent.o
	$(CC) $(FLAGS) $^ -o $@


main.o: main.cpp
	$(CC) $(FLAGS) -c $^ -o $@

car.o: car.cpp
	$(CC) $(FLAGS) -c $^ -o $@

roundabout.o: roundabout.cpp
	$(CC) $(FLAGS) -c $^ -o $@

utils.o: utils.cpp
	$(CC) $(FLAGS) -c $^ -o $@

settings.o: settings.cpp
	$(CC) $(FLAGS) -c $^ -o $@

agent.o: agent.cpp
	$(CC) $(FLAGS) -c $^ -o $@

# Run simulation
run: main.out
	./main.out

# Run simulation with valgrind
memcheck: main.out
	valgrind --leak-check=yes --track-origins=yes -s ./main.out

# Delete files generated in process
clean:
	rm -f *.o  *.out

# Delete history files
clean_history:
	rm -r -f ../history
