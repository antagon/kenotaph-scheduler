#ifndef _CONFIG_H
#define _CONFIG_H

struct config_limit
{
	char *wday;
	char *time_from;
	char *time_to;
	struct config_limit *next;
};

struct config_action
{
	char *on_begin;
	char *on_end;
	char *on_error;
	struct config_limit *limit;
	struct config_action *next;
};

struct config_device
{
	char **id;
	struct config_action *action;
	struct config_device *next;
};

struct config_server
{
	char *name;
	char *host;
	char *port;
	struct config_device *dev;
	struct config_server *next;
};

struct config
{
	struct config_server *head;
	struct config_server *tail;
};

extern int config_load (struct config *conf, const char *filename, unsigned long *server_cnt);

extern void config_unload (struct config *conf);

#endif

