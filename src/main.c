/*
 * Copyright (c) 2016, CodeWard.org
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <getopt.h>
#include <poll.h>
#include <confuse.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <netdb.h>
#include <syslog.h>
#include <unistd.h>

#include "kenotaphsched.h"
#include "config.h"
#include "pathname.h"
#include "server_data.h"
#include "nmsg_queue.h"

static const unsigned int RECONNECT_INTERVAL = 30;
static const unsigned int SELECT_TIMEOUT_MS = -1;
static const unsigned int LISTEN_QUEUE_LEN = 8;

struct option_data
{
	int daemon;
};

static int main_loop;
static int reconnect;
static int exitno;

static void
kenotaphsched_help (const char *p)
{
	fprintf (stdout, "Usage: %s [OPTIONS] <config-file>\n\n"
					 "Options:\n"
					 "  -d, --daemon              run as a daemon\n"
					 "  -h, --help                show this usage information\n"
					 "  -v, --version             show version information\n"
					 , p);
}

static void
kenotaphsched_version (const char *p)
{
	fprintf (stdout, "%s %d.%d.%d\n", p, KENOTAPHSCHED_VER_MAJOR, KENOTAPHSCHED_VER_MINOR, KENOTAPHSCHED_VER_PATCH);
}

static void
kenotaphsched_sigdie (int signo)
{
	main_loop = 0;
	exitno = signo;
}

static void
kenotaphsched_sigalarm (int signo)
{
	reconnect = 1;
}


int
main (int argc, char *argv[])
{
	pid_t pid;
	struct pollfd *poll_fd;
	struct config kenotaphsched_conf;
	struct server_data *server_session;
	struct config_server *server_iter;
	struct nmsg_queue nmsg_que;
	struct nmsg_node *nmsg_node;
	struct nmsg_text nmsg_text;
	struct pathname path_config;
	struct option_data opt;
	//char conf_errbuff[CONF_ERRBUF_SIZE];
	char nmsg_buff[8192];
	ssize_t nmsg_len;
	struct sigaction sa;
	struct addrinfo addr_hint;
	socklen_t opt_len;
	unsigned long server_cnt;
	int i, c, opt_index, opt_val, rval, syslog_flags, poll_len;
	struct itimerval timer;
	struct option opt_long[] = {
		{ "daemon", no_argument, NULL, 'd' },
		{ "help", no_argument, NULL, 'h' },
		{ "version", no_argument, NULL, 'v' },
		{ NULL, 0, NULL, 0 }
	};

	poll_fd = NULL;
	server_session = NULL;
	exitno = EXIT_SUCCESS;
	syslog_flags = LOG_PID | LOG_PERROR;

	memset (&opt, 0, sizeof (struct option_data));
	memset (&path_config, 0, sizeof (struct pathname));
	memset (&kenotaphsched_conf, 0, sizeof (struct config));
	memset (&addr_hint, 0, sizeof (struct addrinfo));
	memset (&nmsg_que, 0, sizeof (struct nmsg_queue));

	addr_hint.ai_family = AF_UNSPEC;
	addr_hint.ai_socktype = SOCK_STREAM;
	addr_hint.ai_flags = AI_NUMERICSERV | AI_CANONNAME;

	memset (&timer, 0, sizeof (struct itimerval));

	timer.it_interval.tv_sec = RECONNECT_INTERVAL;
	timer.it_interval.tv_usec = 0;
	timer.it_value.tv_sec = RECONNECT_INTERVAL;
	timer.it_value.tv_usec = 0;

	while ( (c = getopt_long (argc, argv, "dhv", opt_long, &opt_index)) != -1 ){
		switch ( c ){
			case 'd':
				opt.daemon = 1;
				break;

			case 'h':
				kenotaphsched_help (argv[0]);
				exitno = EXIT_SUCCESS;
				goto cleanup;

			case 'v':
				kenotaphsched_version (argv[0]);
				exitno = EXIT_SUCCESS;
				goto cleanup;

			default:
				kenotaphsched_help (argv[0]);
				exitno = EXIT_FAILURE;
				goto cleanup;
		}
	}

	// Check if there are some non-option arguments, these are treated as paths
	// to configuration files.
	if ( (argc - optind) == 0 ){
		fprintf (stderr, "%s: configuration file not specified. Use '--help' to see usage information.\n", argv[0]);
		exitno = EXIT_FAILURE;
		goto cleanup;
	}

	// Change working directory to match the dirname of the config file.
	rval = path_split (argv[optind], &path_config);

	if ( rval != 0 ){
		fprintf (stderr, "%s: cannot split path to a configuration file.\n", argv[0]);
		exitno = EXIT_FAILURE;
		goto cleanup;
	}

	rval = chdir (path_config.dir);

	if ( rval == -1 ){
		fprintf (stderr, "%s: cannot set working directory to '%s': %s\n", argv[0], path_config.dir, strerror (errno));
		exitno = EXIT_FAILURE;
		goto cleanup;
	}

	//
	// Load configuration file
	//
	rval = config_load (&kenotaphsched_conf, path_config.base, &server_cnt);

	switch ( rval ){
		case CFG_FILE_ERROR:
			fprintf (stderr, "%s: cannot load a configuration file '%s': %s\n", argv[0], argv[optind], strerror (errno));
			exitno = EXIT_FAILURE;
			goto cleanup;

		case CFG_PARSE_ERROR:
			exitno = EXIT_FAILURE;
			goto cleanup;
	}

	if ( server_cnt == 0 ){
		fprintf (stderr, "%s: no servers defined, nothing to do...\n", argv[0]);
		exitno = EXIT_FAILURE;
		goto cleanup;
	}

	// No longer needed, free the resources
	path_free (&path_config);

	server_session = (struct server_data*) calloc (server_cnt, sizeof (struct server_data));

	if ( server_session == NULL ){
		fprintf (stderr, "%s: cannot allocate memory: %s\n", argv[0], strerror (errno));
		exitno = EXIT_FAILURE;
		goto cleanup;
	}

	for ( i = 0, server_iter = kenotaphsched_conf.head; server_iter != NULL; i++, server_iter = server_iter->next ){
		server_data_init (&(server_session[i]));

		server_session[i].server_name = strdup (server_iter->name);

		if ( server_session[i].server_name == NULL ){
			fprintf (stderr, "%s: cannot allocate memory: %s\n", argv[0], strerror (errno));
			exitno = EXIT_FAILURE;
			goto cleanup;
		}

		server_session[i].server_host = strdup (server_iter->host);

		if ( server_session[i].server_host == NULL ){
			fprintf (stderr, "%s: cannot allocate memory: %s\n", argv[0], strerror (errno));
			exitno = EXIT_FAILURE;
			goto cleanup;
		}

		server_session[i].server_port = strdup (server_iter->port);

		if ( server_session[i].server_port == NULL ){
			fprintf (stderr, "%s: cannot allocate memory: %s\n", argv[0], strerror (errno));
			exitno = EXIT_FAILURE;
			goto cleanup;
		}

		// TODO: Pass data to lua state
	}

	// We no longer need data stored in config structure. All neccessary data
	// were moved into server_data structure.
	config_unload (&kenotaphsched_conf);

	poll_len = server_cnt;

	poll_fd = (struct pollfd*) malloc (sizeof (struct pollfd) * poll_len);

	if ( poll_fd == NULL ){
		fprintf (stderr, "%s: cannot allocate memory: %s\n", argv[0], strerror (errno));
		exitno = EXIT_FAILURE;
		goto cleanup;
	}

	for ( i = 0; i < poll_len; i++ ){
		poll_fd[i].fd = -1;
		poll_fd[i].events = 0;
		poll_fd[i].revents = 0;
	}

	//
	// Setup signal handler
	//
	sa.sa_handler = kenotaphsched_sigdie;
	sigemptyset (&(sa.sa_mask));
	sa.sa_flags = 0;

	rval = 0;
	rval &= sigaction (SIGINT, &sa, NULL);
	rval &= sigaction (SIGQUIT, &sa, NULL);
	rval &= sigaction (SIGTERM, &sa, NULL);

	sa.sa_handler = kenotaphsched_sigalarm;
	sigemptyset (&(sa.sa_mask));
	sa.sa_flags = 0;

	rval &= sigaction (SIGALRM, &sa, NULL);

	sa.sa_handler = SIG_IGN;
	sigemptyset (&(sa.sa_mask));
	sa.sa_flags = 0;

	rval &= sigaction (SIGCHLD, &sa, NULL);

	if ( rval != 0 ){
		fprintf (stderr, "%s: cannot setup signal handler: %s\n", argv[0], strerror (errno));
		exitno = EXIT_FAILURE;
		goto cleanup;
	}

	//
	// Daemonize the process if the flag was set
	//
	/*if ( opt.daemon == 1 ){
		pid = fork ();

		if ( pid > 0 ){
			exitno = EXIT_SUCCESS;
			goto cleanup;
		} else if ( pid == -1 ){
			fprintf (stderr, "%s: cannot daemonize the process (fork failed).\n", argv[0]);
			exitno = EXIT_FAILURE;
			goto cleanup;
		}

		if ( setsid () == -1 ){
			fprintf (stderr, "%s: cannot daemonize the process (setsid failed).\n", argv[0]);
			exitno = EXIT_FAILURE;
			goto cleanup;
		}

		umask (0);

		freopen ("/dev/null", "r", stdin);
		freopen ("/dev/null", "w", stdout);
		freopen ("/dev/null", "w", stderr);
		syslog_flags = LOG_PID;
	}*/

	openlog ("kenotaphsched", syslog_flags, LOG_DAEMON);

	syslog (LOG_INFO, "kenotaph-scheduler started");

	//
	// Main loop
	//
	main_loop = 1;
	reconnect = 1;

	rval = setitimer (ITIMER_REAL, &timer, NULL);

	if ( rval == -1 ){
		syslog (LOG_ERR, "cannot schedule time interval: %s", strerror (errno));
		exitno = EXIT_FAILURE;
		goto cleanup;
	}

	while ( main_loop ){

		for ( i = 0; reconnect && (i < server_cnt); i++ ){
			struct addrinfo *host_addr, *host_addr_iter;

			if ( server_session[i].fd != -1 )
				continue;

			rval = getaddrinfo (server_session[i].server_host, server_session[i].server_port, &addr_hint, &host_addr);

			if ( rval != 0 ){
				syslog (LOG_WARNING, "cannot resolve hostname '%s': %s", server_session[i].server_host, gai_strerror (rval));
				continue;
			}

			for ( host_addr_iter = host_addr; host_addr_iter != NULL; host_addr_iter = host_addr_iter->ai_next ){

				server_session[i].fd = socket (host_addr_iter->ai_family, host_addr_iter->ai_socktype | SOCK_NONBLOCK, host_addr_iter->ai_protocol);

				if ( server_session[i].fd == -1 )
					continue;

				errno = 0;
				rval = connect (server_session[i].fd, (struct sockaddr*) host_addr_iter->ai_addr, host_addr_iter->ai_addrlen);

				if ( rval == 0 ){
					break;
				} else if ( rval -1 && errno == EINPROGRESS){
					break;
				} else if ( rval == -1 && errno != EINPROGRESS ){
					close (server_session[i].fd);
					server_session[i].fd = -1;
					continue;
				}
			}

			freeaddrinfo (host_addr);

			if ( server_session[i].fd == -1 && errno != EINPROGRESS ){
				syslog (LOG_WARNING, "cannot connect to %s (%s:%s): %s", server_session[i].server_name, server_session[i].server_host, server_session[i].server_port, strerror (errno));
				continue;
			}

			poll_fd[i].fd = server_session[i].fd;
			poll_fd[i].events |= POLLOUT;
		}

		reconnect = 0;

		rval = poll (poll_fd, poll_len, SELECT_TIMEOUT_MS);

		if ( rval == -1 ){
			if ( errno == EINTR )
				continue;

			syslog (LOG_ERR, "poll(2) failed: %s", strerror (errno));
			exitno = EXIT_FAILURE;
			goto cleanup;
		}

		for ( i = 0; i < server_cnt; i++ ){

			if ( poll_fd[i].revents & POLLOUT ){
				opt_val = -1;
				opt_len = sizeof (opt_val);

				rval = getsockopt (server_session[i].fd, SOL_SOCKET, SO_ERROR, &opt_val, &opt_len);

				if ( rval == -1 ){
					syslog (LOG_ERR, "cannot get value of socket option: %s", strerror (errno));
					exitno = EXIT_FAILURE;
					goto cleanup;
				}

				// Do not watch for POLLOUT event anymore.
				poll_fd[i].events &= ~POLLOUT;

				if ( opt_val == 0 ){
					syslog (LOG_INFO, "connected to %s (%s:%s)", server_session[i].server_name, server_session[i].server_host, server_session[i].server_port);

					poll_fd[i].events |= POLLIN | POLLERR | POLLHUP;
				} else {
					syslog (LOG_WARNING, "cannot connect to %s (%s:%s): %s", server_session[i].server_name, server_session[i].server_host, server_session[i].server_port, strerror (opt_val));

					close (server_session[i].fd);
					server_session[i].fd = -1;
					poll_fd[i].fd = server_session[i].fd;
					poll_fd[i].events = 0;
					poll_fd[i].revents = 0;
					continue;
				}
			}

			// Handle incoming data (event).
			if ( poll_fd[i].revents & POLLIN ){
				errno = 0;
				nmsg_len = recv (server_session[i].fd, nmsg_buff, sizeof (nmsg_buff), MSG_DONTWAIT);

				if ( nmsg_len <= 0 ){
					syslog (LOG_WARNING, "disconnected from %s (%s:%s)", server_session[i].server_name, server_session[i].server_host, server_session[i].server_port);

					close (server_session[i].fd);
					server_session[i].fd = -1;
					poll_fd[i].fd = server_session[i].fd;
					poll_fd[i].events = 0;
					continue;
				}

				rval = nmsg_queue_unserialize (&nmsg_que, nmsg_buff, nmsg_len);

				if ( rval == -1 ){
					fprintf (stderr, "%s: cannot allocate memory: %s\n", argv[0], strerror (errno));
					exitno = EXIT_FAILURE;
					goto cleanup;
				}
			}

			nmsg_node = nmsg_que.head;

			while ( nmsg_node != NULL ){
				rval = nmsg_node_text (nmsg_node, &nmsg_text);

				if ( rval == NMSG_ECON ){
					fprintf (stderr, "[!!! incomplete message !!!]\n");
					nmsg_node = nmsg_node->next;
					continue;
				} else if ( rval == NMSG_ESYN ){
					fprintf (stderr, "[!!! syntax error !!!]\n");
				} else if ( rval == NMSG_ECHR ){
					fprintf (stderr, "[!!! characters error !!!]\n");
				} else {
					fprintf (stderr, "%s:%s\n", nmsg_text.id, nmsg_text.type);
					//fprintf (stderr, "%lu B [ complete message ]\n", nmsg_len);
				}

				nmsg_queue_delete (&nmsg_que, &nmsg_node);
			}
		}
	}

	syslog (LOG_INFO, "kenotaph-scheduler shutdown (signal %u)", exitno);

cleanup:
	closelog ();

	if ( server_session != NULL ){
		for ( i = 0; i < server_cnt; i++ )
			server_data_free (&(server_session[i]));
		free (server_session);
	}

	nmsg_queue_free (&nmsg_que);

	if ( poll_fd != NULL )
		free (poll_fd);

	config_unload (&kenotaphsched_conf);

	path_free (&path_config);

	return EXIT_SUCCESS;
}

