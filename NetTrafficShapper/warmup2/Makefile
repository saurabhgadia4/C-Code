warmup2: driver.o my402queue.o
	gcc -o warmup2 -g driver.o my402queue.o -lpthread -lm

driver.o: driver.c driver.h my402list.h cs402.h
	gcc -g -c -Wall driver.c

my402queue.o: my402queue.c my402queue.h
	gcc -g -c -Wall my402queue.c
	
clean:
	rm -f *.o warmup2
