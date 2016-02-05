/*
 * Copyright (c) 2016, CodeWard.org
 */
#include <stdlib.h>
#include <string.h>

#include "server_data.h"

void
server_data_init (struct server_data *server_data)
{
	memset (server_data, 0, sizeof (struct server_data));
	server_data->fd = -1;
}

void
server_data_free (struct server_data *server_data)
{
	if ( server_data->server_name != NULL )
		free (server_data->server_name);
	if ( server_data->server_host != NULL )
		free (server_data->server_host);
	if ( server_data->server_port != NULL )
		free (server_data->server_port);
}

