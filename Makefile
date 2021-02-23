CC=gcc
CFLAGS="-Wall -Wextra -Werror -pedantic"

debug:clean
	$(CC) $(CFLAGS) -fsanitize=leak,address,undefined -g -o vmd example.c
stable:clean
	$(CC) $(CFLAGS) -O3 -o vmd example.c
clean:
	rm -vfr *~ vmd
