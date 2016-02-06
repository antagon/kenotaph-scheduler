/*
 * Copyright (c) 2016, CodeWard.org
 */
#ifndef _CONFIG_H
#define _CONFIG_H

#include "config_server.h"

#define CONF_ERRBUF_SIZE 256

struct config
{
	struct config_server *head;
	struct config_server *tail;
};

extern int config_load (struct config *conf, const char *filename, char *errbuf);

extern void config_unload (struct config *conf);

#endif

