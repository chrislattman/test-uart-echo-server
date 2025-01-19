default:
	gcc -Wall -Wextra -pedantic -std=c99 -o posix_client posix_client.c

windows:
	gcc -Wall -Wextra -pedantic -std=c99 -o win_client win_client.c

clean:
	rm -rf *_client
