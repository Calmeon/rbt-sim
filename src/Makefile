CC = g++
FLAGS = -g -Wall

all: main.out

main.out: main.o car.o roundabout.o
	$(CC) $(FLAGS) $^ -o $@


main.o: main.cpp
	$(CC) $(FLAGS) -c $^ -o $@

car.o: car.cpp
	$(CC) $(FLAGS) -c $^ -o $@

roundabout.o: roundabout.cpp
	$(CC) $(FLAGS) -c $^ -o $@


# Run simulation
run: main.out
	./main.out

# Run simulation with valgrind
memcheck: main.out
	valgrind --leak-check=yes --track-origins=yes ./main.out

# Delete files generated in process
clean:
	rm -f *.o  *.out
