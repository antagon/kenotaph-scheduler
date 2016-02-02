#
# Copyright (c) 2016, CodeWard.org
#

CC = gcc
CFLAGS = -std=gnu99 -O2 -pedantic -Wall -ggdb

NAME = kenotaphsched
INSTALL_DIR = /usr/local/sbin

.PHONY: clean install

all: $(NAME)

install: $(NAME)
	install --mode 755 $(NAME) $(INSTALL_DIR)

uninstall:
	rm -f $(INSTALL_DIR)/$(NAME)

clean:
	rm -f *.o $(NAME)

$(NAME): main.o config.o config_filter.o config_server.o session_data.o pathname.o nmsg_queue.o
#hostport_parser.o
	$(CC) $(CFLAGS) $^ -o $(NAME) -lconfig

main.o: main.c
	$(CC) $(CFLAGS) -c $<

config.o: config.c config.h
	$(CC) $(CFLAGS) -c $<

config_filter.o: config_filter.c config_filter.h
	$(CC) $(CFLAGS) -c $<

config_server.o: config_server.c config_server.h
	$(CC) $(CFLAGS) -c $<

session_data.o: session_data.c session_data.h
	$(CC) $(CFLAGS) -c $<

pathname.o: pathname.c pathname.h
	$(CC) $(CFLAGS) -c $<

nmsg_queue.o: nmsg_queue.c nmsg_queue.h
	$(CC) $(CFLAGS) -c $<

