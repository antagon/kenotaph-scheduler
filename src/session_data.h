/*
 * Copyright (c) 2016, CodeWard.org
 */
#ifndef _SESSION_DATA_H
#define _SESSION_DATA_H

#include "session_event.h"

struct session_data
{
	int fd;
	char *server_name;
	char *server_host;
	char *server_port;
};

extern void session_data_init (struct session_data *session_data);

extern void session_data_free (struct session_data *session_data);

#endif

