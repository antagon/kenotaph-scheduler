#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <confuse.h>

#include "config.h"

static cfg_opt_t limit_opts[] = {
	CFG_STR ("wday", NULL, CFGF_NODEFAULT),
	CFG_STR ("time_from", NULL, CFGF_NODEFAULT),
	CFG_STR ("time_to", NULL, CFGF_NODEFAULT),
	CFG_END ()
};

static cfg_opt_t action_opts[] = {
	CFG_STR ("on_begin", NULL, CFGF_NODEFAULT),
	CFG_STR ("on_end", NULL, CFGF_NODEFAULT),
	CFG_STR ("on_error", NULL, CFGF_NODEFAULT),
	CFG_SEC ("limit", limit_opts, CFGF_MULTI),
	CFG_END ()
};

static cfg_opt_t device_opts[] = {
	CFG_STR_LIST ("id", NULL, CFGF_NODEFAULT),
	CFG_SEC ("action", action_opts, CFGF_TITLE | CFGF_MULTI | CFGF_NO_TITLE_DUPES),
	CFG_END ()
};

static cfg_opt_t server_opts[] = {
	CFG_STR ("name", NULL, CFGF_NODEFAULT),
	CFG_STR ("host", NULL, CFGF_NODEFAULT),
	CFG_STR ("port", NULL, CFGF_NODEFAULT),
	CFG_SEC ("device", device_opts, CFGF_MULTI),
	CFG_END ()
};

static cfg_opt_t conf_opts[] = {
	CFG_SEC ("server", server_opts, CFGF_TITLE | CFGF_MULTI | CFGF_NO_TITLE_DUPES),
	CFG_END ()
};

static int
cfg_validate_server (cfg_t *cfg, cfg_opt_t *opt)
{
	cfg_t *serv;

	serv = cfg_opt_getnsec (opt, cfg_opt_size (opt) - 1);

	if ( cfg_size (serv, "host") == 0 ){
		cfg_error (cfg, "undefined option 'host'");
		return -1;
	}

	if ( cfg_size (serv, "port") == 0 ){
		cfg_error (cfg, "undefined option 'port'");
		return -1;
	}

	return 0;
}

static int
cfg_validate_device (cfg_t *cfg, cfg_opt_t *opt)
{
	cfg_t *dev;

	dev = cfg_opt_getnsec (opt, cfg_opt_size (opt) - 1);

	if ( cfg_size (dev, "id") == 0 ){
		cfg_error (cfg, "undefined option 'id'");
		return -1;
	}

	return 0;
}

static void
config_limit_free (struct config_limit *conf_limit)
{
	if ( conf_limit->wday != NULL )
		free (conf_limit->wday);
	if ( conf_limit->time_from != NULL )
		free (conf_limit->time_from);
	if ( conf_limit->time_to != NULL )
		free (conf_limit->time_to);
}

static void
config_action_free (struct config_action *conf_action)
{
	struct config_limit *limit, *limit_next;

	if ( conf_action->on_begin != NULL )
		free (conf_action->on_begin);
	if ( conf_action->on_end != NULL )
		free (conf_action->on_end);
	if ( conf_action->on_error != NULL )
		free (conf_action->on_error);

	for ( limit = conf_action->limit; limit != NULL; ){
		limit_next = limit->next;
		config_limit_free (limit);
		free (limit);
		limit = limit_next;
	}
}

static void
config_device_free (struct config_device *conf_dev)
{
	struct config_action *action, *action_next;
	int i;

	i = 0;

	while ( conf_dev->id[i] != NULL )
		free (conf_dev->id[i++]);

	free (conf_dev->id);

	for ( action = conf_dev->action; action != NULL; ){
		action_next = action->next;
		config_action_free (action);
		free (action);
		action = action_next;
	}
}

static void
config_server_free (struct config_server *conf_serv)
{
	struct config_device *dev, *dev_next;

	if ( conf_serv->name != NULL )
		free (conf_serv->name);
	if ( conf_serv->host != NULL )
		free (conf_serv->host);
	if ( conf_serv->port != NULL )
		free (conf_serv->port);

	for ( dev = conf_serv->dev; dev != NULL; ){
		dev_next = dev->next;
		config_device_free (dev);
		free (dev);
		dev = dev_next;
	}
}

int
config_load (struct config *conf, const char *filename, unsigned long *server_cnt)
{
	cfg_t *cfg, *cfg_serv, *cfg_dev, *cfg_action, *cfg_limit;
	struct config_server *conf_server;
	struct config_device *conf_dev, **conf_dev_tail;
	struct config_action *conf_action, **conf_action_tail;
	struct config_limit *conf_limit, **conf_limit_tail;
	unsigned long id_cnt;
	char *str_val;
	int serv_idx, dev_idx, devid_idx, action_idx, limit_idx, exitno;

	if ( server_cnt != NULL )
		*server_cnt = 0;

	conf_server = NULL;
	conf_dev = NULL;
	conf_action = NULL;
	conf_limit = NULL;

	cfg = cfg_init (conf_opts, CFGF_NONE);

	cfg_set_validate_func (cfg, "server", cfg_validate_server);
	cfg_set_validate_func (cfg, "server|device", cfg_validate_device);

	exitno = cfg_parse (cfg, filename);

	switch ( exitno ){
		case CFG_FILE_ERROR:
			goto cleanup;

		case CFG_PARSE_ERROR:
			goto cleanup;
	}

	//
	// Process server section
	//
	for ( serv_idx = 0; serv_idx < cfg_size (cfg, "server"); serv_idx++ ){
		cfg_serv = cfg_getnsec (cfg, "server", serv_idx);

		conf_server = (struct config_server*) calloc (1, sizeof (struct config_server));

		if ( conf_server == NULL ){
			exitno = CFG_FILE_ERROR;
			goto cleanup;
		}

		conf_server->name = strdup (cfg_title (cfg_serv));

		if ( conf_server->name == NULL ){
			exitno = CFG_FILE_ERROR;
			goto cleanup;
		}

		conf_server->host = strdup (cfg_getstr (cfg_serv, "host"));

		if ( conf_server->host == NULL ){
			exitno = CFG_FILE_ERROR;
			goto cleanup;
		}

		conf_server->port = strdup (cfg_getstr (cfg_serv, "port"));

		if ( conf_server->port == NULL ){
			exitno = CFG_FILE_ERROR;
			goto cleanup;
		}

		conf_dev_tail = &(conf_server->dev);

		//
		// Process server|device section
		//
		for ( dev_idx = 0; dev_idx < cfg_size (cfg_serv, "device"); dev_idx++ ){
			cfg_dev = cfg_getnsec (cfg_serv, "device", dev_idx);

			conf_dev = (struct config_device*) calloc (1, sizeof (struct config_device));

			if ( conf_dev == NULL ){
				exitno = CFG_FILE_ERROR;
				goto cleanup;
			}

			id_cnt = cfg_size (cfg_dev, "id");

			conf_dev->id = (char**) calloc (id_cnt + 1, sizeof (char*));

			if ( conf_dev->id == NULL ){
				exitno = CFG_FILE_ERROR;
				goto cleanup;
			}

			//
			// Process server|device|id list
			//
			for ( devid_idx = 0; devid_idx < cfg_size (cfg_dev, "id"); devid_idx++ ){
				conf_dev->id[devid_idx] = strdup (cfg_getnstr (cfg_dev, "id", devid_idx));

				if ( conf_dev->id[devid_idx] == NULL ){
					exitno = CFG_FILE_ERROR;
					goto cleanup;
				}
			}

			conf_action_tail = &(conf_dev->action);

			//
			// Process server|device|action section
			//
			for ( action_idx = 0; action_idx < cfg_size (cfg_dev, "action"); action_idx++ ){
				cfg_action = cfg_getnsec (cfg_dev, "action", action_idx);

				conf_action = (struct config_action*) calloc (1, sizeof (struct config_action));

				if ( conf_action == NULL ){
					exitno = CFG_FILE_ERROR;
					goto cleanup;
				}

				str_val = cfg_getstr (cfg_action, "on_begin");

				if ( str_val != NULL ){
					conf_action->on_begin = strdup (str_val);

					if ( conf_action->on_begin == NULL ){
						exitno = CFG_FILE_ERROR;
						goto cleanup;
					}
				}

				str_val = cfg_getstr (cfg_action, "on_end");

				if ( str_val != NULL ){
					conf_action->on_end = strdup (str_val);

					if ( conf_action->on_end == NULL ){
						exitno = CFG_FILE_ERROR;
						goto cleanup;
					}
				}

				str_val = cfg_getstr (cfg_action, "on_error");

				if ( str_val != NULL ){
					conf_action->on_error = strdup (str_val);

					if ( conf_action->on_error == NULL ){
						exitno = CFG_FILE_ERROR;
						goto cleanup;
					}
				}

				conf_limit_tail = &(conf_action->limit);

				// Process server|device|action|limit
				for ( limit_idx = 0; limit_idx < cfg_size (cfg_action, "limit"); limit_idx++ ){
					cfg_limit = cfg_getnsec (cfg_action, "limit", limit_idx);

					conf_limit = (struct config_limit*) calloc (1, sizeof (struct config_limit));

					if ( conf_limit == NULL ){
						exitno = CFG_FILE_ERROR;
						goto cleanup;
					}

					str_val = cfg_getstr (cfg_limit, "wday");

					if ( str_val != NULL ){
						conf_limit->wday = strdup (str_val);

						if ( conf_limit->wday == NULL ){
							exitno = CFG_FILE_ERROR;
							goto cleanup;
						}
					}

					str_val = cfg_getstr (cfg_limit, "time_from");

					if ( str_val != NULL ){
						conf_limit->time_from = strdup (str_val);

						if ( conf_limit->time_from == NULL ){
							exitno = CFG_FILE_ERROR;
							goto cleanup;
						}
					}

					str_val = cfg_getstr (cfg_limit, "time_to");

					if ( str_val != NULL ){
						conf_limit->time_to = strdup (str_val);

						if ( conf_limit->time_to == NULL ){
							exitno = CFG_FILE_ERROR;
							goto cleanup;
						}
					}

					*conf_limit_tail = conf_limit;
					conf_limit_tail = &((*conf_limit_tail)->next);
				}

				*conf_action_tail = conf_action;
				conf_action_tail = &((*conf_action_tail)->next);
			}

			*conf_dev_tail = conf_dev;
			conf_dev_tail = &((*conf_dev_tail)->next);
		}

		if ( server_cnt != NULL )
			(*server_cnt)++;

		if ( conf->head == NULL ){
			conf->head = conf_server;
			conf->tail = conf->head;
		} else {
			conf->tail->next = conf_server;
			conf->tail = conf_server;
		}
	}

	// Set to null so we do not remove the last added node.
	conf_server = NULL;
	conf_dev = NULL;
	conf_action = NULL;
	conf_limit = NULL;

cleanup:
	if ( conf_server != NULL ){
		config_server_free (conf_server);
		free (conf_server);
	}

	if ( conf_dev != NULL ){
		config_device_free (conf_dev);
		free (conf_dev);
	}

	if ( conf_action != NULL ){
		config_action_free (conf_action);
		free (conf_action);
	}

	if ( conf_limit != NULL ){
		config_limit_free (conf_limit);
		free (conf_limit);
	}

	cfg_free (cfg);

	return exitno;
}

void
config_unload (struct config *conf)
{
	struct config_server *conf_server, *conf_server_next;

	for ( conf_server = conf->head; conf_server != NULL; ){
		conf_server_next = conf_server->next;
		config_server_free (conf_server);
		free (conf_server);
		conf_server = conf_server_next;
	}

	conf->head = NULL;
	conf->tail = NULL;
}

