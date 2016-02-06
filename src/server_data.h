/*
 * Copyright (c) 2016, CodeWard.org
 */
#ifndef _SERVER_DATA_H
#define _SERVER_DATA_H

#include "event_data.h"

struct server_data
{
	int fd;
	char *server_name;
	char *server_host;
	char *server_port;
	struct event_data *event;
};

extern void server_data_init (struct server_data *server_data);

extern void server_data_free (struct server_data *server_data);

#endif

