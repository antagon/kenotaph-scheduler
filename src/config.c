/*
 * Copyright (c) 2016, CodeWard.org
 */
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <libconfig.h>

#include "config.h"

int
config_load (struct config *conf, const char *filename, char *errbuf)
{
	config_t libconfig;
	config_setting_t *root_setting;
	config_setting_t *server_setting;
	config_setting_t *server_setting_elem;
	struct config_server *server;
	struct config_event *event;
	const char *str_val;
	int i, j, server_cnt, option_cnt, int_val, error;

	error = 0;
	server = NULL;

	config_init (&libconfig);

	if ( config_read_file (&libconfig, filename) == CONFIG_FALSE ){
		snprintf (errbuf, CONF_ERRBUF_SIZE, "%s on line %d", config_error_text (&libconfig), config_error_line (&libconfig));
		error = 1;
		goto cleanup;
	}

	root_setting = config_root_setting (&libconfig);
	server_cnt = config_setting_length (root_setting);

	memset (errbuf, 0, sizeof (CONF_ERRBUF_SIZE));

	for ( i = 0; i < server_cnt; i++ ){
		server = (struct config_server*) calloc (1, sizeof (struct config_server));

		if ( server == NULL ){
			snprintf (errbuf, CONF_ERRBUF_SIZE, "cannot allocate memory");
			error = 1;
			goto cleanup;
		}

		server_setting = config_setting_get_elem (root_setting, i);

		if ( server_setting == NULL ){
			snprintf (errbuf, CONF_ERRBUF_SIZE, "cannot get a server definition");
			error = 1;
			goto cleanup;
		}

		str_val = config_setting_name (server_setting);

		if ( str_val == NULL ){
			snprintf (errbuf, CONF_ERRBUF_SIZE, "server definition %d, has no name", i + 1);
			error = 1;
			goto cleanup;
		}

		server_set_name (server, str_val);

		option_cnt = config_setting_length (server_setting);

		for ( j = 0; j < option_cnt; j++ ){
			server_setting_elem = config_setting_get_elem (server_setting, j);

			if ( server_setting_elem == NULL ){
				snprintf (errbuf, CONF_ERRBUF_SIZE, "cannot get an option inside a server definition");
				error = 1;
				goto cleanup;
			}

			str_val = config_setting_name (server_setting_elem);

			if ( str_val == NULL ){
				snprintf (errbuf, CONF_ERRBUF_SIZE, "server '%s', missing name for an event rule", server->name);
				error = 1;
				goto cleanup;
			}

			if ( strlen (str_val) > CONF_EVENT_NAME_MAXLEN ){
				snprintf (errbuf, CONF_ERRBUF_SIZE, "event rule %d, name too long", i + 1);
				error = 1;
				goto cleanup;
			}

			int_val = config_setting_type (server_setting_elem);

			if ( (int_val == CONFIG_TYPE_STRING) && (strcmp (str_val, "hostname") == 0) ){
				str_val = config_setting_get_string (server_setting_elem);
				server_set_hostname (server, str_val);
				continue;

			} else if ( (int_val == CONFIG_TYPE_STRING) && (strcmp (str_val, "port") == 0) ){
				str_val = config_setting_get_string (server_setting_elem);
				server_set_port (server, str_val);
				continue;

			} else if ( (int_val != CONFIG_TYPE_GROUP) && (int_val != CONFIG_TYPE_LIST) ){
				snprintf (errbuf, CONF_ERRBUF_SIZE, "server '%s', event rule '%s' is not a list or a group", server->name, str_val);
				error = 1;
				goto cleanup;
			}

			event = (struct config_event*) calloc (1, sizeof (struct config_event));

			if ( event == NULL ){
				snprintf (errbuf, CONF_ERRBUF_SIZE, "cannot allocate memory");
				error = 1;
				goto cleanup;
			}

			event_set_name (event, str_val);

			if ( int_val == CONFIG_TYPE_GROUP ){

				if ( config_setting_lookup_string (server_setting_elem, "event_begin", &str_val) == CONFIG_FALSE ){
					str_val = NULL;
				}

				event_set_onbegin (event, str_val);

				if ( config_setting_lookup_string (server_setting_elem, "event_end", &str_val) == CONFIG_FALSE ){
					str_val = NULL;
				}

				event_set_onend (event, str_val);

				if ( config_setting_lookup_string (server_setting_elem, "event_error", &str_val) == CONFIG_FALSE ){
					str_val = NULL;
				}

				event_set_onerror (event, str_val);

				if ( config_setting_lookup_string (server_setting_elem, "wday", &str_val) == CONFIG_FALSE ){
					str_val = NULL;
				}

				event_set_wday (event, str_val);

				if ( config_setting_lookup_string (server_setting_elem, "time_begin", &str_val) == CONFIG_FALSE ){
					str_val = NULL;
				}

				event_set_timebegin (event, str_val);

				if ( config_setting_lookup_string (server_setting_elem, "time_end", &str_val) == CONFIG_FALSE ){
					str_val = NULL;
				}

				event_set_timeend (event, str_val);
			}

			server_append_event (server, event);
		}

		if ( server->hostname == NULL ){
			snprintf (errbuf, CONF_ERRBUF_SIZE, "server '%s', cannot obtain value of an option 'hostname'", server->name);
			error = 1;
			goto cleanup;
		}

		if ( server->port == NULL ){
			snprintf (errbuf, CONF_ERRBUF_SIZE, "server '%s', cannot obtain value of an option 'port'", server->name);
			error = 1;
			goto cleanup;
		}

		if ( conf->head == NULL ){
			conf->head = server;
			conf->tail = conf->head;
		} else {
			conf->tail->next = server;
			conf->tail = server;
		}

		server = NULL;
	}

cleanup:
	if ( error ){
		if ( server != NULL ){
			server_destroy (server);
			free (server);
		}

		config_unload (conf);
		server_cnt = -1;
	}

	config_destroy (&libconfig); // destroy libconfig object

	return server_cnt;
}

void
config_unload (struct config *conf)
{
	struct config_server *server, *server_next;

	server = conf->head;

	while ( server != NULL ){
		server_next = server->next;
		server_destroy (server);
		free (server);
		server = server_next;
	}

	conf->head = NULL;
	conf->tail = conf->head;
}

