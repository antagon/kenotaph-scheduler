/*
 * Copyright (c) 2016, CodeWard.org
 */
#ifndef _CONFIG_FILTER_H
#define _CONFIG_FILTER_H

#define CONF_FILTER_NAME_MAXLEN 128

struct config_filter
{
	char *name;
	char *on_begin;
	char *on_error;
	char *on_end;
	struct config_filter *next;
};

extern int filter_set_name (struct config_filter *filter, const char *name);

extern int filter_set_onbegin (struct config_filter *filter, const char *on_begin);

extern int filter_set_onerror (struct config_filter *filter, const char *on_error);

extern int filter_set_onend (struct config_filter *filter, const char *on_end);

extern void filter_destroy (struct config_filter *filter);

#endif

