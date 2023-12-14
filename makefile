CFLAGS = -Wall

build:
	$(CC) $(CFLAGS) -o run "Day $$day/code.c"

clean:
	rm -f run test_result.txt result.txt
