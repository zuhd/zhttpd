/**
@Author:ZUHD
@Date:2014-11-21
*/
#ifndef ZHTTP_SESSION_H
#define ZHTTP_SESSION_H
#include "defines.h"

struct _zhttp_session
{
	int zs_fd;
	int read_offset;
	int write_offset;
	char read_buff[MAX_MSG_LEN];
	char write_buff[MAX_MSG_LEN];
	struct _zhttp_sendlist* head;
};

// 用链表实现大数据的Data
struct _zhttp_sendlist
{
	int chunk_len;
	char chunk_buff[MAX_MSG_LEN];
	struct _zhttp_sendlist* next;
};

int init_session(struct _zhttp_session* session);
int release_session(struct _zhttp_session* session);
int sendlist_insert(struct _zhttp_session* session, struct _zhttp_sendlist* head);
int sendlist_free(struct _zhttp_session* session, struct _zhttp_sendlist* head);
int poll_modify(struct _zhttp_session* session, int flag);

#endif