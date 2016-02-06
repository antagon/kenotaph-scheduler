/*
 * Copyright (c) 2016, CodeWard.org
 */
#ifndef _EVENT_DATA_H
#define _EVENT_DATA_H

#include <wordexp.h>
#include <time.h>

struct event_data
{
	struct event_data *next;
	char *name;
	char *event_beg;
	char *event_end;
	char *event_err;
	time_t time_begin;
	time_t time_end;
	wordexp_t cmd_evtbeg;
	wordexp_t cmd_evtend;
	wordexp_t cmd_evterr;
};

#endif

