/*
 * Copyright (c) 2016, CodeWard.org
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "nmsg_queue.h"

void
nmsg_queue_push (struct nmsg_queue *res, struct nmsg_node *node)
{
	if ( res->head == NULL ){
		res->head = node;
		res->tail = res->head;
	} else {
		res->tail->next = node;
		res->tail = node;
	}

	res->len += node->len;
}

ssize_t
nmsg_queue_unserialize (struct nmsg_queue *res, const char *buff, size_t buff_len)
{
	struct nmsg_node *node;
	char *node_buff;
	size_t i, len, maxlen;

	node = NULL;

	for ( i = 0; i < buff_len; i++ ){

		if ( node == NULL ){
			node = (struct nmsg_node*) malloc (sizeof (struct nmsg_node));

			if ( node == NULL )
				return -1;

			memset (node, 0, sizeof (struct nmsg_node));

			nmsg_queue_push (res, node);

			len = 0;
			maxlen = NMSG_ID_MAXLEN;
			node_buff = node->id;
		}

		if ( buff[i] == ':' ){
			len = 0;
			maxlen = NMSG_TYPE_MAXLEN;
			node_buff = node->type;
			//fprintf (stderr, ":");
			continue;
		} else if ( buff[i] == '\n' ){
			node = NULL;
			//fprintf (stderr, "\n");
			continue;
		}

		if ( len > maxlen )
			continue;

		node_buff[len++] = buff[i];
		node->len++;
		//fprintf (stderr, "%c", node_buff[len]);
	}

	return i;
}

void
nmsg_queue_free (struct nmsg_queue *res)
{
	struct nmsg_node *node, *node_next;

	node = res->head;

	while ( node != NULL ){
		node_next = node->next;
		free (node);
		node = node_next;
	}

	memset (res, 0, sizeof (struct nmsg_queue));
}

