CC = gcc
CFLAGS = -g -Wall -Wpedantic -Wshadow -Wformat=2 -Wcast-align -Wnull-dereference -fdiagnostics-color=always -std=c17 -lm

PQ_H = priority_queue/priority_queue.h priority_queue/binary_heap.h priority_queue/leftist_heap.h priority_queue/skew_heap.h priority_queue/binomial_heap.h priority_queue/fibonacci_heap.h priority_queue/cartesian_tree.h
PQ_O = gcc_obj/priority_queue.o gcc_obj/binary_heap.o gcc_obj/leftist_heap.o gcc_obj/skew_heap.o gcc_obj/binomial_heap.o gcc_obj/fibonacci_heap.o gcc_obj/cartesian_tree.o
PQ_O_TRG = priority_queue.o binary_heap.o leftist_heap.o skew_heap.o binomial_heap.o fibonacci_heap.o cartesian_tree.o

MAP_H = map/map.h map/hash_set.h map/dynamic_array.h map/binary_search_tree.h map/trie.h
MAP_O = gcc_obj/map.o gcc_obj/hash_set.o gcc_obj/dynamic_array.o gcc_obj/binary_search_tree.o gcc_obj/trie.o
MAP_O_TRG = map.o hash_set.o dynamic_array.o binary_search_tree.o trie.o

OTHER_O = gcc_obj/logger.o gcc_obj/readers.o gcc_obj/department.o gcc_obj/utility.o

VPATH = gcc_obj

all: main pq_test map_test config_generator input_generator

# CONFIG GENERATOR
config_generator: config_generator.o utility.o
	$(CC) $(CFLAGS) -o config_gen gcc_obj/config_generator.o gcc_obj/utility.o -lm

config_generator.o: config_generator.c utility.h
	$(CC) $(CFLAGS) -c config_generator.c -o gcc_obj/config_generator.o -lm

# INPUT GENERATOR
input_generator: input_generator.o utility.o readers.o
	$(CC) $(CFLAGS) -o input_gen gcc_obj/input_generator.o $(MAP_O) $(PQ_O) $(OTHER_O) -lm

input_generator.o: input_generator.c utility.h readers.h
	$(CC) $(CFLAGS) -c input_generator.c -o gcc_obj/input_generator.o -lm


# MAP TESTS
map_test: map_test.o $(MAP_O_TRG) $(PQ_O_TRG) department.o utility.o
	$(CC) $(CFLAGS) -o map_test gcc_obj/map_test.o $(MAP_O) $(PQ_O) gcc_obj/department.o gcc_obj/utility.o -lm

map_test.o: map/map_test.c map/map.h department.h utility.h
	$(CC) $(CFLAGS) -c map/map_test.c -o gcc_obj/map_test.o -lm

# PRIORITY QUEUE TESTS
pq_test: pq_test.o $(PQ_O_TRG) utility.o
	$(CC) $(CFLAGS) -o pq_test gcc_obj/pq_test.o $(PQ_O) gcc_obj/utility.o -lm

pq_test.o: priority_queue/pq_test.c priority_queue/priority_queue.h utility.h
	$(CC) $(CFLAGS) -c priority_queue/pq_test.c -o gcc_obj/pq_test.o -lm


# MAIN EXECUTABLE
main: task09.o $(MAP_O_TRG) $(PQ_O_TRG) readers.o department.o utility.o logger.o
	$(CC) $(CFLAGS) -o main gcc_obj/task09.o $(MAP_O) $(PQ_O) $(OTHER_O) -lm

task09.o: task09.c priority_queue/priority_queue.h priority_queue/binomial_heap.h utility.h
	$(CC) $(CFLAGS) -c task09.c -o gcc_obj/task09.o -lm

# MAP
map.o: map/map.c map/map.h department.h utility.h
	$(CC) $(CFLAGS) -c map/map.c -o gcc_obj/map.o -lm

hash_set.o: map/hash_set.c map/hash_set.h department.h utility.h
	$(CC) $(CFLAGS) -c map/hash_set.c -o gcc_obj/hash_set.o -lm

dynamic_array.o: map/dynamic_array.c map/dynamic_array.h department.h utility.h
	$(CC) $(CFLAGS) -c map/dynamic_array.c -o gcc_obj/dynamic_array.o -lm

binary_search_tree.o: map/binary_search_tree.c map/binary_search_tree.h department.h utility.h
	$(CC) $(CFLAGS) -c map/binary_search_tree.c -o gcc_obj/binary_search_tree.o -lm

trie.o: map/trie.c map/trie.h department.h utility.h
	$(CC) $(CFLAGS) -c map/trie.c -o gcc_obj/trie.o -lm

# PRIORITY QUEUE
priority_queue.o: priority_queue/priority_queue.c priority_queue/priority_queue.h utility.h
	$(CC) $(CFLAGS) -c priority_queue/priority_queue.c -o gcc_obj/priority_queue.o -lm

binary_heap.o: priority_queue/binary_heap.c priority_queue/binary_heap.h utility.h
	$(CC) $(CFLAGS) -c priority_queue/binary_heap.c -o gcc_obj/binary_heap.o -lm

leftist_heap.o: priority_queue/leftist_heap.c priority_queue/leftist_heap.h utility.h
	$(CC) $(CFLAGS) -c priority_queue/leftist_heap.c -o gcc_obj/leftist_heap.o -lm

skew_heap.o: priority_queue/skew_heap.c priority_queue/skew_heap.h utility.h
	$(CC) $(CFLAGS) -c priority_queue/skew_heap.c -o gcc_obj/skew_heap.o -lm

binomial_heap.o: priority_queue/binomial_heap.c priority_queue/binomial_heap.h utility.h
	$(CC) $(CFLAGS) -c priority_queue/binomial_heap.c -o gcc_obj/binomial_heap.o -lm

fibonacci_heap.o: priority_queue/fibonacci_heap.c priority_queue/fibonacci_heap.h utility.h
	$(CC) $(CFLAGS) -c priority_queue/fibonacci_heap.c -o gcc_obj/fibonacci_heap.o -lm
	
cartesian_tree.o: priority_queue/cartesian_tree.c priority_queue/cartesian_tree.h utility.h
	$(CC) $(CFLAGS) -c priority_queue/cartesian_tree.c -o gcc_obj/cartesian_tree.o -lm

# OTHER
logger.o: logger.c logger.h utility.h department.h
	$(CC) $(CFLAGS) -c logger.c -o gcc_obj/logger.o -lm

readers.o: readers.c readers.h utility.h priority_queue/priority_queue.h map/map.h
	$(CC) $(CFLAGS) -c readers.c -o gcc_obj/readers.o -lm

department.o: department.c department.h utility.h priority_queue/priority_queue.h
	$(CC) $(CFLAGS) -c department.c -o gcc_obj/department.o -lm

utility.o: utility.c utility.h
	$(CC) $(CFLAGS) -c utility.c -o gcc_obj/utility.o -lm
	
.PHONY: clean

clean:
	rm gcc_obj/*
	rm main
	rm pq_test
	rm map_test