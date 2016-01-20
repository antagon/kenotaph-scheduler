/*
 * Copyright (c) 2016, CodeWard.org
 */
#include <stdlib.h>
#include <string.h>

#include "session_data.h"

void
session_data_init (struct session_data *session_data)
{
	memset (session_data, 0, sizeof (struct session_data));
	session_data->fd = -1;
}

void
session_data_free (struct session_data *session_data)
{
	if ( session_data->server_name != NULL )
		free (session_data->server_name);
	if ( session_data->server_host != NULL )
		free (session_data->server_host);
	if ( session_data->server_port != NULL )
		free (session_data->server_port);
}

