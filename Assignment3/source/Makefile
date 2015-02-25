CC	= gcc   
CFLAGS    =  -O3 -fopenmp -lm

all: bucketsort bucketsort_serial

bucketsort:	bucketsort.c quicksort.c
	$(CC) $(CFLAGS) -o bucketsort bucketsort.c quicksort.c $(LIBS)

bucketsort_serial:	bucketsort_serial.c quicksort.c
	$(CC) $(CFLAGS) -o bucketsort_serial bucketsort_serial.c quicksort.c $(LIBS)