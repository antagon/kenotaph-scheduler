/*
 * Copyright (c) 2016, CodeWard.org
 */
#ifndef _CONFIG_EVENT_H
#define _CONFIG_EVENT_H

#define CONF_EVENT_NAME_MAXLEN 128

struct config_event
{
	char *name;
	char *event_begin;
	char *event_end;
	char *event_error;
	char *wday;
	char *time_begin;
	char *time_end;
	struct config_event *next;
};

extern int event_set_name (struct config_event *event, const char *name);

extern int event_set_onbegin (struct config_event *event, const char *event_begin);

extern int event_set_onerror (struct config_event *event, const char *event_error);

extern int event_set_onend (struct config_event *event, const char *event_end);

extern int event_set_wday (struct config_event *event, const char *wday);

extern int event_set_timebegin (struct config_event *event, const char *time_begin);

extern int event_set_timeend (struct config_event *event, const char *time_end);

extern void event_destroy (struct config_event *event);

#endif

