/*
 * Copyright (c) 2016, CodeWard.org
 */
#include <stdlib.h>
#include <string.h>

#include "config_event.h"

int
event_set_name (struct config_event *event, const char *name)
{
	if ( event->name != NULL )
		free (event->name);

	if ( name == NULL ){
		event->name = NULL;
		return 1;
	}

	event->name = strdup (name);
	
	if ( event->name == NULL )
		return 1;
	
	return 0;
}

int
event_set_onbegin (struct config_event *event, const char *event_begin)
{
	if ( event->event_begin != NULL )
		free (event->event_begin);

	if ( event_begin == NULL ){
		event->event_begin = NULL;
		return 0;
	}

	event->event_begin = strdup (event_begin);
	
	if ( event->event_begin == NULL )
		return 1;
	
	return 0;
}

int
event_set_onerror (struct config_event *event, const char *event_error)
{
	if ( event->event_error != NULL )
		free (event->event_error);

	if ( event_error == NULL ){
		event->event_error = NULL;
		return 0;
	}

	event->event_error = strdup (event_error);
	
	if ( event->event_error == NULL )
		return 1;
	
	return 0;
}

int
event_set_onend (struct config_event *event, const char *event_end)
{
	if ( event->event_end != NULL )
		free (event->event_end);

	if ( event_end == NULL ){
		event->event_end = NULL;
		return 0;
	}

	event->event_end = strdup (event_end);
	
	if ( event->event_end == NULL )
		return 1;
	
	return 0;
}

int
event_set_wday (struct config_event *event, const char *wday)
{
	if ( event->wday != NULL )
		free (event->wday);

	if ( wday == NULL ){
		event->wday = NULL;
		return 0;
	}

	event->wday = strdup (wday);

	if ( event->wday == NULL )
		return 1;

	return 0;
}

int
event_set_timebegin (struct config_event *event, const char *time_begin)
{
	if ( event->time_begin != NULL )
		free (event->time_begin);

	if ( time_begin == NULL ){
		event->time_begin = NULL;
		return 0;
	}

	event->time_begin = strdup (time_begin);

	if ( event->time_begin == NULL )
		return 1;

	return 0;
}

int
event_set_timeend (struct config_event *event, const char *time_end)
{
	if ( event->time_end != NULL )
		free (event->time_end);

	if ( time_end == NULL ){
		event->time_end = NULL;
		return 0;
	}

	event->time_end = strdup (time_end);

	if ( event->time_end == NULL )
		return 1;

	return 0;
}

void
event_destroy (struct config_event *event)
{
	if ( event->name != NULL )
		free (event->name);
	if ( event->event_begin != NULL )
		free (event->event_begin);
	if ( event->event_error != NULL )
		free (event->event_error);
	if ( event->event_end != NULL )
		free (event->event_end);
	if ( event->wday != NULL )
		free (event->wday);
	if ( event->time_begin != NULL )
		free (event->time_begin);
	if ( event->time_end != NULL )
		free (event->time_end);
}

