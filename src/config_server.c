/*
 * Copyright (c) 2016, CodeWard.org
 */
#include <stdlib.h>
#include <string.h>

#include "config_server.h"

int
server_set_name (struct config_server *server, const char *name)
{
	if ( server->name != NULL )
		free (server->name);

	if ( name == NULL ){
		server->name = NULL;
		return 1;
	}

	server->name = strdup (name);
	
	if ( server->name == NULL )
		return 1;
	
	return 0;
}

int
server_set_hostname (struct config_server *server, const char *hostname)
{
	if ( server->hostname != NULL )
		free (server->hostname);

	if ( hostname == NULL ){
		server->hostname = NULL;
		return 1;
	}

	server->hostname = strdup (hostname);
	
	if ( server->hostname == NULL )
		return 1;
	
	return 0;
}

int
server_set_port (struct config_server *server, const char *port)
{
	if ( server->port != NULL )
		free (server->port);

	if ( port == NULL ){
		server->port = NULL;
		return 1;
	}

	server->port = strdup (port);
	
	if ( server->port == NULL )
		return 1;
	
	return 0;
}

int
server_append_event (struct config_server *server, struct config_event *event)
{
	struct config_event **event_iter;

	event_iter = &(server->event);

	while ( *event_iter != NULL ){
		*event_iter = (*event_iter)->next;
	}

	*event_iter = event;

	return 1;
}

void
server_destroy (struct config_server *server)
{
	struct config_event *event, *event_next;

	if ( server->name != NULL )
		free (server->name);
	if ( server->hostname != NULL )
		free (server->hostname);
	if ( server->port != NULL )
		free (server->port);

	event = server->event;

	while ( event != NULL ){
		event_next = event->next;
		event_destroy (event);
		free (event);
		event = event_next;
	}
}

