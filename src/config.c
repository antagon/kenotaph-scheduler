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
	struct config_server *server;
	const char *str_val;
	int i, server_cnt;

	config_init (&libconfig);

	if ( config_read_file (&libconfig, filename) == CONFIG_FALSE ){
		snprintf (errbuf, CONF_ERRBUF_SIZE, "%s on line %d", config_error_text (&libconfig), config_error_line (&libconfig));
		config_destroy (&libconfig);
		return -1;
	}

	root_setting = config_root_setting (&libconfig);
	server_cnt = config_setting_length (root_setting);

	/*if ( filter_cnt > CONF_FILTER_MAXCNT ){
		snprintf (errbuf, CONF_ERRBUF_SIZE, "too many filters defined (max %d)", CONF_FILTER_MAXCNT);
		config_destroy (&libconfig);
		return -1;
	}*/

	memset (errbuf, 0, sizeof (CONF_ERRBUF_SIZE));

	for ( i = 0; i < server_cnt; i++ ){
		server = (struct config_server*) calloc (1, sizeof (struct config_server));

		if ( server == NULL ){
			snprintf (errbuf, CONF_ERRBUF_SIZE, "cannot allocate memory for filter");
			config_destroy (&libconfig);
			return -1;
		}

		server_setting = config_setting_get_elem (root_setting, i);

		// Just in case... we do not want to touch the NULL pointer
		if ( server_setting == NULL ){
			snprintf (errbuf, CONF_ERRBUF_SIZE, "no filters defined");
			free (server);
			config_destroy (&libconfig);
			return -1;
		}

		str_val = config_setting_name (server_setting);

		if ( str_val == NULL ){
			snprintf (errbuf, CONF_ERRBUF_SIZE, "in %d. filter, missing filter name", i + 1);
			free (server);
			config_unload (conf);
			config_destroy (&libconfig);
			return -1;
		}

		if ( strlen (str_val) > CONF_FILTER_NAME_MAXLEN ){
			snprintf (errbuf, CONF_ERRBUF_SIZE, "filter name too long");
			free (server);
			config_destroy (&libconfig);
			return -1;
		}

		server_set_name (server, str_val);

		if ( config_setting_lookup_string (server_setting, "hostname", &str_val) == CONFIG_FALSE ){
			snprintf (errbuf, CONF_ERRBUF_SIZE, "in server '%s', missing option 'hostname'", server->name);
			server_destroy (server);
			free (server);
			config_unload (conf);
			config_destroy (&libconfig);
			return -1;
		}

		server_set_hostname (server, str_val);

		if ( config_setting_lookup_string (server_setting, "port", &str_val) == CONFIG_FALSE ){
			snprintf (errbuf, CONF_ERRBUF_SIZE, "in server '%s', missing option 'port'", server->name);
			server_destroy (server);
			free (server);
			config_unload (conf);
			config_destroy (&libconfig);
			return -1;
		}

		server_set_port (server, str_val);

		/*if ( config_setting_lookup_string (filter_setting, "match", &str_val) == CONFIG_FALSE ){
			str_val = NULL;
		}

		filter_set_matchrule (filter, str_val);

		if ( config_setting_lookup_int (filter_setting, "session_timeout", &num) == CONFIG_FALSE ){
			snprintf (errbuf, CONF_ERRBUF_SIZE, "in filter '%s', missing option 'session_timeout'", filter->name);
			free (filter);
			config_unload (conf);
			config_destroy (&libconfig);
			return -1;
		}

		if ( num == 0 ){
			snprintf (errbuf, CONF_ERRBUF_SIZE, "in filter '%s', 'session_timeout' must be greater than 0", filter->name);
			free (filter);
			config_unload (conf);
			config_destroy (&libconfig);
			return -1;
		}
	
		filter_set_session_timeout (filter, ((num < 0)? (num * -1):num));

		if ( config_setting_lookup_bool (filter_setting, "monitor_mode", &num) == CONFIG_FALSE ){
			num = 0;
		}

		filter_set_monitor_mode (filter, num);

		if ( config_setting_lookup_string (filter_setting, "interface", &str_val) == CONFIG_FALSE ){
			snprintf (errbuf, CONF_ERRBUF_SIZE, "in filter '%s', missing option 'interface'", filter->name);
			free (filter);
			config_unload (conf);
			config_destroy (&libconfig);
			return -1;
		}

		if ( strlen (str_val) == 0 ){
			snprintf (errbuf, CONF_ERRBUF_SIZE, "in filter '%s', empty option 'interface'", filter->name);
			free (filter);
			config_unload (conf);
			config_destroy (&libconfig);
			return -1;
		}

		filter_set_interface (filter, str_val);

		if ( config_setting_lookup_string (filter_setting, "link_type", &str_val) == CONFIG_FALSE ){
			str_val = NULL;
		}

		filter_set_link_type (filter, str_val);*/

		if ( conf->head == NULL ){
			conf->head = server;
			conf->tail = conf->head;
		} else {
			conf->tail->next = server;
			conf->tail = server;
		}
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

