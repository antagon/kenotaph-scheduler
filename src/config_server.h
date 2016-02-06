/*
 * Copyright (c) 2016, CodeWard.org
 */
#ifndef _CONFIG_SERVER_H
#define _CONFIG_SERVER_H

#include "config_event.h"

struct config_server
{
	char *name;
	char *hostname;
	char *port;
	struct config_event *event;
	struct config_server *next;
};

extern int server_set_name (struct config_server *server, const char *name);

extern int server_set_hostname (struct config_server *server, const char *hostname);

extern int server_set_port (struct config_server *server, const char *port);

extern int server_append_event (struct config_server *server, struct config_event *event);

extern void server_destroy (struct config_server *server);

#endif

