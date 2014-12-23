/**
@Author:ZUHD
@Data:2014-11-5
*/

/*
version 1.0
1，只支持单服务器，不支持虚拟主机
2，不使用多线程技术，用epoll处理并发
3，处理request和response，暂不支持CGI
4，支持日志系统

version 2.0
1，支持多主机，虚拟主机
2，支持CGI
*/

#include "sys.h"
#include "config.h"
#include "defines.h"
#include "request.h"
#include "global.h"
#include "session.h"
#include "log.h"

int backlog = 1024;
int max_fd = 1024;
int g_running = 1;
int curfds = 0;
int epoll_fd = 0;

void exit_signal(int signo)
{
	g_running = 0;
	return;
} 

int setnonblocking(int fd)
{
	int flag = fcntl(fd, F_GETFL, 0);
	if (flag < 0)
	{
		return -1;
	}
	fcntl(fd, F_SETFL, flag | O_NONBLOCK);
	return 0;
}

int accept_cb(int fd)
{
	int connfd;
	struct sockaddr_in client_addr;
	socklen_t socklen = sizeof(client_addr);
	while ((connfd = accept(fd, (struct sockaddr*)&client_addr, &socklen)) > 0)
	{
		printf("accept client from[%s], fd=%d\n", inet_ntoa(client_addr.sin_addr), connfd);
		if (curfds >= max_fd)
		{
			fprintf(stderr, "too many connections \n");
			close(connfd);
			return -1;
		}
		struct epoll_event ev;
		ev.events = EPOLLIN | EPOLLET;
		ev.data.fd = connfd;
	  struct _zhttp_session* session = (struct _zhttp_session*)malloc(sizeof(struct _zhttp_session));
	  init_session(session);
		session->zs_fd = connfd;
		ev.data.ptr = session;
		int ret = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, connfd, &ev);
		if (ret < 0)
		{
			perror("epoll_ctl connfd error");
			close(connfd);
			return -1;
		}
		curfds++;
		setnonblocking(connfd);
	}
	if (connfd < 0)
	{
		if (errno == EAGAIN ||
		errno == EINTR)
		{
			//printf("accept_cb errno=%d\n", errno);
		}
		else
		{
			perror("accept error");
			return -1;
		}
	}
	return 0;
}

// http是短连接 只考虑断包的情况 
// 不用考虑粘包的情况
int read_cb(struct _zhttp_session* session)
{
	if (session == NULL)
	{
		return -1;
	}
	if (session->read_offset < 0 ||
	session->read_offset > MAX_MSG_LEN)
	{
		fprintf(stderr, "read_offset error \n");
		return -1;
	}
	
	int fd = session->zs_fd;
	int len = 0;
	int total = 0;
	while((len = read(fd, session->read_buff + session->read_offset, MAX_MSG_LEN - session->read_offset)) > 0)
	{
		session->read_offset += len;
		total += len;
		if (session->read_offset >= MAX_MSG_LEN)
		{
			// 如果是超级大的包怎么办?丢弃吗？
			break;
		}
	}
	if (len < 0)
	{		
		if (errno == EAGAIN || 
		errno == EINTR)
		{
			printf("read_cb errno=%d\n", errno);
		}
		else
		{
			// error
			perror("read error");
			epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
			release_session(session);
			return -1;
		}
	}
	else if (len == 0)
	{
		// 正常关闭
		// 服务器被动优雅关闭
		printf("socket fd = %d, close\n", fd);
		epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
		release_session(session);
		return 0;
	}	
	// 处理业务层逻辑
	// 先解析到request后，根据request去home中
	// 查找相应的文件，然后再发给客户端
	// 这里分配内存，使用者释放内存
	struct _zhttp_request* request = (struct _zhttp_request*)malloc(sizeof(struct _zhttp_request));
	if (request != NULL)
	{
		request->zr_httpver = 0;
		request->zr_keepalive = 0;
		request->zr_sendfile = 0;
		memset(request->zr_uri, 0, sizeof(request->zr_uri));
		memset(request->zr_domain, 0, sizeof(request->zr_domain));
		memset(request->zr_useragent, 0, sizeof(request->zr_useragent));
		on_zhttp_request(session, request);
	}		
	return total;
}

int write_cb(struct _zhttp_session* session)
{
	if (session == NULL)
	{
		return -1;
	}
	
	if (session->write_offset < 0 ||
	session->write_offset > MAX_MSG_LEN)
	{
		fprintf(stderr, "write_offset error \n");
		return -1;
	}
	
	if (session->write_offset == 0 &&
	session->head == NULL)
	{
		return 0;
	}
	int fd = session->zs_fd;
	int len = 0;
	int total = 0;
	while ((len = write(fd, session->write_buff, session->write_offset)) > 0)
	{		
		memmove(session->write_buff, session->write_buff + len, session->write_offset - len);
		session->write_offset -= len;
		total += len;
		if (session->write_offset == 0)
		{
			break;
		}
	}
	if (len < 0)
	{
		if (errno == EAGAIN ||
		errno == EINTR)
		{
			printf("write_buff errno=%d\n", errno);
			poll_modify(session, EPOLLET | EPOLLIN | EPOLLOUT);
			return total;
		}
		else
		{
			perror("write error");
			epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
			release_session(session);
			return -1;
		}
	}		
	// 发送chunk中的数据
	struct _zhttp_sendlist* head = session->head;
	while (head != NULL)
	{
		int write_len = 0;
		struct _zhttp_sendlist* next = head->next;
		while ((len = write(fd, head->chunk_buff, head->chunk_len)) > 0)
		{
			memmove(head->chunk_buff, head->chunk_buff + len, head->chunk_len - len);
			head->chunk_len -= len;
			total += len;
			write_len += len;
			if (head->chunk_len == 0)
			{
				break;
			}
		}		
		// 有可能内核缓冲区已满
		if (len < 0)
		{
			printf("send chunk len=%d, not free!!\n", write_len);
			if (errno == EAGAIN ||
			errno == EINTR)
			{
				printf("chunk_buff error, fd=%d, errno=%d\n", session->zs_fd, errno);
				poll_modify(session, EPOLLET | EPOLLIN | EPOLLOUT);
				return total;
			}
			else
			{
				perror("write error");
				epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
				release_session(session);
				return -1;
			}
		}		
		printf("free chunk len=%d\n", write_len);
		sendlist_free(session, head);
		head = next;
	}
	
	return total;
}

int main(int argc, char** argv)
{	
	signal(SIGTERM, exit_signal);
	// 加载配置文件
	// 服务器启动
	if (argc > 1 && strcmp(argv[1], "-D") == 0)
	{
		daemon(1, 0);
	}
	if (load_config("config") < 0)
	{
		return -1;
	}
	int listenfd;
	int nfds, connfd;
	struct sockaddr_in server_addr;
	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family  = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(zhttp_server.ip);
	server_addr.sin_port = htons(zhttp_server.port);
	
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (listenfd < 0)
	{
		perror("create socket error");
		return -1;
	}
	// 设置复用
	int opt = 1;
	int ret = setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	if (ret < 0)
	{
		perror("setsockopt error");
		return -1;
	}
	
	ret = setnonblocking(listenfd);
	if (ret < 0)
	{
		perror("setnonblocking error");
		return -1;
	}
	
	// bind
	ret = bind(listenfd, (struct sockaddr*)&server_addr, sizeof(struct sockaddr));
	if (ret < 0)
	{
		perror("bind error");
		return -1;
	}
	
	// listen
	ret = listen(listenfd, backlog);
	if (ret < 0)
	{
		perror("listen error");
		return -1;
	}
	
	// 使用ET模式
	epoll_fd = epoll_create(max_fd);
	struct epoll_event ev;
	ev.events = EPOLLIN | EPOLLET;
	ev.data.fd = listenfd;
	ret = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listenfd, &ev);
	if (ret < 0)
	{
		perror("epoll_ctl error");
		return -1;
	}
	while(g_running)
	{
		struct epoll_event events[1024];
		nfds = epoll_wait(epoll_fd, events, max_fd, -1);
		if (nfds < 0)
		{
			perror("epoll_wait error");
			return -1;
		}
		else if (nfds == 0)
		{
			// nothing to do
			usleep(1000);
		}
		else
		{
			int i = 0;
			for (i = 0; i < nfds; i++)
			{
				if (events[i].data.fd == listenfd)
				{
					// 处理accept
					accept_cb(events[i].data.fd);
				}
				else
				{
					if (events[i].events & EPOLLIN)
					{
						read_cb(( struct _zhttp_session*)events[i].data.ptr);
					}
					if (events[i].events & EPOLLOUT)
					{
						write_cb(( struct _zhttp_session*)events[i].data.ptr);
					}
				}
			}
		}
	}
	return 0;
}
