#
# Copyright (c) 2016, CodeWard.org
#

CC = gcc
CFLAGS = -std=gnu99 -O2 -pedantic -Wall -ggdb

NAME = kenotaph-sched
INSTALL_DIR = /usr/local/sbin

.PHONY: clean install

all: $(NAME)

install: $(NAME)
	install --mode 755 $(NAME) $(INSTALL_DIR)

uninstall:
	rm -f $(INSTALL_DIR)/$(NAME)

clean:
	rm -f *.o $(NAME)

$(NAME): main.o config.o server_data.o event_data.o pathname.o nmsg_queue.o
	$(CC) $(CFLAGS) $^ -o $(NAME) -lconfuse

main.o: main.c
	$(CC) $(CFLAGS) -c $<

config.o: config.c config.h
	$(CC) $(CFLAGS) -c $<

server_data.o: server_data.c server_data.h
	$(CC) $(CFLAGS) -c $<

event_data.o: event_data.c event_data.h
	$(CC) $(CFLAGS) -c $<

pathname.o: pathname.c pathname.h
	$(CC) $(CFLAGS) -c $<

nmsg_queue.o: nmsg_queue.c nmsg_queue.h
	$(CC) $(CFLAGS) -c $<

