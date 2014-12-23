/**
@Author:ZUHD
@Date:2014-11-25
*/
#include "sys.h"
#include "session.h"
#include "global.h"

int init_session(struct _zhttp_session* session)
{
	if (session == NULL)
	{
		return -1;
	}
	session->zs_fd = -1;
	session->read_offset = 0;
	session->write_offset = 0;
	memset(session->read_buff, 0, sizeof(session->read_buff));
	memset(session->write_buff, 0, sizeof(session->write_buff));
	session->head = NULL;
	return 0;
}

int release_session(struct _zhttp_session* session)
{
	if (session == NULL)
	{
		return -1;
	}
	close(session->zs_fd);
	struct _zhttp_sendlist* head = session->head;
	while (head != NULL)
	{
		struct _zhttp_sendlist* p = head;
		head = head->next;
		p->next = NULL;
		free(p);
	}
	session->head = NULL;
	return 0;
}

int sendlist_insert(struct _zhttp_session* session, struct _zhttp_sendlist* head)
{
	if (session == NULL)
	{
		return -1;
	}
	if (session->head == NULL)
	{
		session->head = head;
	}
	else
	{
		struct _zhttp_sendlist* head = session->head;
		while (head->next != NULL)
		{
			head = head->next;
		}
		head->next = head;
	}
	return 0;
}

int sendlist_free(struct _zhttp_session* session, struct _zhttp_sendlist* head)
{	
	if (session != NULL)
	{
		// 只删除一个节点
		struct _zhttp_sendlist* session_head = session->head;		
		while (session_head != NULL && session_head->next != NULL)
		{
			if (session_head->next == head)
			{
				session_head->next = session_head->next->next;
				head->next = NULL;
				free(head);
				return 0;
			}
			session_head = session_head->next;
		}
		// 如果是头节点被删除
		if (session->head == head)
		{			
			if (session->head == NULL)
			{
				session->head = NULL;
			}
			else
			{
				session->head = session->head->next;
				free(head);
			}						
		}
	}
	else
	{
		// 删除整个链表
		struct _zhttp_sendlist* send_head = head;
		while (send_head != NULL)
		{
			struct _zhttp_sendlist* p = send_head;
			send_head = send_head->next;
			p->next = NULL;
			free(p);
		}
	}
	return 0;
}

int poll_modify(struct _zhttp_session* session, int flag)
{
	if (session == NULL)
	{
		return -1;
	}
	struct epoll_event ev;
	ev.events = flag;
	ev.data.fd = session->zs_fd;
	ev.data.ptr = session;
	epoll_ctl(epoll_fd, EPOLL_CTL_MOD, session->zs_fd, &ev);
	return 0;
}