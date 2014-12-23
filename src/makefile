zhttpd : log.c request.c response.c session.c config.c main.c
	gcc -g -Wall -o $@ $^ -lpthread

all:
	cp zhttpd ../test
clean:
	rm zhttpd