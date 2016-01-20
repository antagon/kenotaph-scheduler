/*
 * Copyright (c) 2016, CodeWard.org
 */
#include <stdlib.h>
#include <string.h>

#include "config_filter.h"

int
filter_set_name (struct config_filter *filter, const char *name)
{
	if ( filter->name != NULL )
		free (filter->name);

	if ( name == NULL ){
		filter->name = NULL;
		return 1;
	}

	filter->name = strdup (name);
	
	if ( filter->name == NULL )
		return 1;
	
	return 0;
}

int
filter_set_onbegin (struct config_filter *filter, const char *on_begin)
{
	if ( filter->on_begin != NULL )
		free (filter->on_begin);

	if ( on_begin == NULL ){
		filter->on_begin = NULL;
		return 0;
	}

	filter->on_begin = strdup (on_begin);
	
	if ( filter->on_begin == NULL )
		return 1;
	
	return 0;
}

int
filter_set_onerror (struct config_filter *filter, const char *on_error)
{
	if ( filter->on_error != NULL )
		free (filter->on_error);

	if ( on_error == NULL ){
		filter->on_error = NULL;
		return 0;
	}

	filter->on_error = strdup (on_error);
	
	if ( filter->on_error == NULL )
		return 1;
	
	return 0;
}

int
filter_set_onend (struct config_filter *filter, const char *on_end)
{
	if ( filter->on_end != NULL )
		free (filter->on_end);

	if ( on_end == NULL ){
		filter->on_end = NULL;
		return 0;
	}

	filter->on_end = strdup (on_end);
	
	if ( filter->on_end == NULL )
		return 1;
	
	return 0;
}

void
filter_destroy (struct config_filter *filter)
{
	if ( filter->name != NULL )
		free (filter->name);
	if ( filter->on_begin != NULL )
		free (filter->on_begin);
	if ( filter->on_error != NULL )
		free (filter->on_error);
	if ( filter->on_end != NULL )
		free (filter->on_end);
}

