/**
@Author:ZUHD
@Date:2014-12-09
*/

#include "sys.h"
#include "response.h"
#include "config.h"
#include "global.h"
#include "defines.h"
#include "log.h"
#include "session.h"
#include "request.h"
#include "cgi.h"

static int get_relative_uri(const char* uri, char* relative)
{
	if (uri == NULL ||
	relative == NULL)
	{
		return -1;
	}
	if (strncasecmp(uri, "http://", 7) == 0)
	{
		strncpy(relative, uri + 7, strlen(uri) - 7);
		log(uri);
		return 0;
	}
	strncpy(relative, uri, strlen(uri));
	return 0;
}

static int get_absolute_path(const char* uri, char* absolute)
{
	if (uri == NULL ||
	absolute == NULL)
	{
		return -1;
	}
	
	strncpy(absolute, zhttp_server.home, strlen(zhttp_server.home));
	char* p = absolute;
	int len = strlen(p);
	if (len <= 0)
	{
		return -1;
	}
	p += len;
	if (*p == '/')
	{
		p--;
	}
	char relative[MAX_URI] = {0};
	get_relative_uri(uri, relative);
	if (strlen(relative) == 1 &&
	(strncmp(relative, "/", 1) == 0))
	{
		strncpy(p, "/index.html", 11);
		return 0;
	}
	len = strlen(relative);
	strncpy(p, relative, len);
	if (*(p + len - 1) == '/')
	{
		// 不能是一个目录
		return -1;
	}
	return 0;
}

static int get_content_type(char* file, char* type)
{
	if (file == NULL ||
	type == NULL)
	{
		return -1;
	}
	
	char* str = strchr(file, '.');
	int len;
	// 只枚举几种常用的情况
	if (str == NULL)
	{
		len = strlen("Content-Type: unknown\r\n");
		strncpy(type, "Content-Type: unknown\r\n", len);
	}
	else if (strncmp(str, ".html", 5) == 0)
	{
		len = strlen("Content-Type: text/html\r\n");
		strncpy(type, "Content-Type: text/html\r\n", len);
	}
	else if (strncmp(str, ".gif", 4) == 0)
	{
		len = strlen("Content-Type: image/gif\r\n");
		strncpy(type, "Content-Type: image/gif\r\n", len);
	}
	else if (strncmp(str, ".jpg", 4) == 0)
	{
		len = strlen("Content-Type: image/jpg\r\n");
		strncpy(type, "Content-Type: image/jpg\r\n", len);
	}
	else if (strncmp(str, ".png", 4) == 0)
	{
		len = strlen("Content-Type: image/png\r\n");
		strncpy(type, "Content-Type: image/png\r\n", len);
	}
	else
	{
		len = strlen("Content-Type: unknown\r\n");
		strncpy(type, "Content-Type: unknown\r\n", len);
	}
	return len;
}

static int get_date(char* date)
{
	if (date == NULL)
	{
		return -1;
	}
	char buff[256] = {0};
	struct tm cur;
	struct tm *cur_p;
	time_t t = time( NULL );
	gmtime_r( &t, &cur );
	cur_p = &cur;
	strftime(buff, sizeof(buff), "Date: %a, %d %b %Y %H:%M:%S GMT\r\n", cur_p);	
	strncpy(date, buff, strlen(buff));
	return strlen(buff);
}

static int get_server(char* server)
{
	if (server == NULL)
	{
		return -1;
	}
	int len = strlen("Server: zhttpd\r\n");
	strncpy(server, "Server: zhttpd\r\n", len);
	return len;
}

static int get_content_length(int len, char* length)
{
	if (length == NULL)
	{
		return -1;
	}
	char buff[256] = {0};
	sprintf(buff, "Content-Length: %d\r\n", len);
	strncpy(length, buff, strlen(buff));
	return strlen(buff);
}

int response_header(struct _zhttp_session* session, struct _zhttp_request* request)
{
	char header[256] = {0};
	char send_file[MAX_URI] = {0};
	if (session == NULL ||
	request == NULL)
	{
		return -1;
	}
	
	int fd = session->zs_fd;
	switch(request->zr_httpver)
	{
		case HTTP_VER_I09:
			{
				strncpy(header, HTTP_VER_C09, strlen(HTTP_VER_C09));
			}
			break;
		case HTTP_VER_I10:
			{
				strncpy(header, HTTP_VER_C10, strlen(HTTP_VER_C10));
			}
			break;
		case HTTP_VER_I11:
			{
				strncpy(header, HTTP_VER_C11, strlen(HTTP_VER_C11));
			}
			break;
	}
	
	switch(request->zr_code)
	{
		case ZSC_200:
			{
				strncat(header, ZSC_200_DESC, strlen(ZSC_200_DESC));
			}
			break;
		case ZSC_201:
			{
				strncat(header, ZSC_201_DESC, strlen(ZSC_201_DESC));
			}
			break;
		case ZSC_202:
			{
				strncat(header, ZSC_202_DESC, strlen(ZSC_202_DESC));
			}
			break;
		case ZSC_204:
			{
				strncat(header, ZSC_204_DESC, strlen(ZSC_204_DESC));
			}
			break;
		case ZSC_300:
			{
				strncat(header, ZSC_300_DESC, strlen(ZSC_300_DESC));
			}
			break;
		case ZSC_301:
			{
				strncat(header, ZSC_301_DESC, strlen(ZSC_301_DESC));
			}
			break;
		case ZSC_302:
			{
				strncat(header, ZSC_302_DESC, strlen(ZSC_302_DESC));
			}
			break;
		case ZSC_304:
			{
				strncat(header, ZSC_304_DESC, strlen(ZSC_304_DESC));
			}
			break;
		case ZSC_400:
			{
				strncat(header, ZSC_400_DESC, strlen(ZSC_400_DESC));
			}
			break;
		case ZSC_401:
			{
				strncat(header, ZSC_401_DESC, strlen(ZSC_401_DESC));
			}
			break;
		case ZSC_403:
			{
				strncat(header, ZSC_403_DESC, strlen(ZSC_403_DESC));
			}
			break;
		case ZSC_404:
			{
				strncat(header, ZSC_404_DESC, strlen(ZSC_404_DESC));
			}
			break;
		case ZSC_405:
			{
				strncat(header, ZSC_405_DESC, strlen(ZSC_405_DESC));
			}
			break;
		case ZSC_406:
			{
				strncat(header, ZSC_406_DESC, strlen(ZSC_406_DESC));
			}
			break;
		case ZSC_500:
			{
				strncat(header, ZSC_500_DESC, strlen(ZSC_500_DESC));
			}
			break;
		case ZSC_501:
			{
				strncat(header, ZSC_501_DESC, strlen(ZSC_501_DESC));
			}
			break;
		case ZSC_502:
			{
				strncat(header, ZSC_502_DESC, strlen(ZSC_502_DESC));
			}
			break;
		case ZSC_503:
			{
				strncat(header, ZSC_503_DESC, strlen(ZSC_503_DESC));
			}
			break;
		default:
			break;
	}
	
	memcpy(session->write_buff + session->write_offset, header, strlen(header));
	session->write_offset += strlen(header);
	memcpy(session->write_buff + session->write_offset, HTTP_EOF, strlen(HTTP_EOF));
	session->write_offset += strlen(HTTP_EOF);
	struct epoll_event ev;
	ev.events = EPOLLET | EPOLLIN | EPOLLOUT;
	ev.data.fd = fd;
	ev.data.ptr = session;
	epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &ev);
	return 0;
}

static int response_content(struct _zhttp_session* session, struct _zhttp_request* request)
{
	if (session == NULL ||
	request == NULL)
	{
		return -1;
	}
	char send_file[MAX_URI] = {0};
	int fd = session->zs_fd;
	int ret = get_absolute_path(request->zr_uri, send_file);
	int len = 0;
	char header[256] = {0};	
	if (ret < 0)
	{
		request->zr_code = ZSC_503;
		response_header(session, request);
		return -1;
	}
	// 先判断是否是cgi类型
	if (check_cgi_request(request))
	{
		_zhttp_cgi cgi;
		char* p = strtok(send_file, "?");
		if (p != NULL)
		{
			strncpy(cgi.script, p, strlen(p) + 1);
		}
		on_zhttp_cgi_request(request, &cgi);
		// TODO run script + para ,释放cgi中的资源
#ifdef TEST_DEMO		
		switch(request->zr_httpver)
		{
			case HTTP_VER_I09:
			{
				strncpy(header, HTTP_VER_C09, strlen(HTTP_VER_C09));
			}
			break;
			case HTTP_VER_I10:
			{
				strncpy(header, HTTP_VER_C10, strlen(HTTP_VER_C10));
			}
			break;
			case HTTP_VER_I11:
			{
				strncpy(header, HTTP_VER_C11, strlen(HTTP_VER_C11));
			}
			break;
		}
	
		strncat(header, ZSC_200_DESC, strlen(ZSC_200_DESC));
		len = strlen(header);
		memcpy(session->write_buff + session->write_offset, header, len);
		session->write_offset += len;
		len = strlen(HTTP_EOF);
		memcpy(session->write_buff + session->write_offset, HTTP_EOF, len);
		session->write_offset += len;
		char key_value[1024] = {0};
		char buff[1024] = {0};
		st_cgi_para_head* head = cgi.head;
		// 同时在这里释放节点内存
		st_cgi_para_head* node = NULL;
		while (head != NULL)
		{
			node = head->next;
			strcat(key_value, head->key, strlen(head->key));
			strcat(key_value, head->value, strlen(head->value));
			free(head);
			head = node;
		}
		len = strlen(key_value);
		sprintf(buff, "Content-Type: text/plain\r\nContent-Length: %d\r\nServer: zhttpd/0.1.0\r\n\r\n", len);
		memcpy(session->write_buff + session->write_offset, buff, strlen(buff));
		session->write_offset += strlen(buff);
		// 拷贝内容
		memcpy(session->write_buff + session->write_offset, key_value, strlen(key_value));
		session->write_offset += strlen(key_value);
#endif
		return;
	}
	int file_fd = open(send_file, O_RDONLY);
	enum _zhttp_status_code code = ZSC_200;
	if (file_fd < 0)
	{
		switch(errno)
		{
			case EACCES:
				{
					code = ZSC_403;
				}
				break;
			case ENOENT:
				{
					code = ZSC_404;
				}
				break;
			case ENAMETOOLONG:
			case ETXTBSY:
			case EFAULT:
			case EISDIR:
				{
					code = ZSC_406;
				}
				break;
			case ENOMEM:
			case EFBIG:
			case EMFILE:
			case ENFILE:
			default:
				{
					code = ZSC_500;
				}
				break;
				
		}
	}
	
	if (code != ZSC_200)
	{
		request->zr_code = code;
		response_header(session, request);
		return -1;
	}	
	
	if (session->write_offset >= MAX_MSG_LEN)
	{
		code = ZSC_500;
		request->zr_code = code;
		response_header(session, request);
		return -1;
	}	
	
	// 把数据文件放在chunk中，把包头放在write_buff中
	// 这样做的话，尽管会浪费一些空间，但是代码方便管理
	struct _zhttp_sendlist* head = NULL;
	struct _zhttp_sendlist* next = NULL;
	struct _zhttp_sendlist* list = (struct _zhttp_sendlist*)malloc(sizeof(struct _zhttp_sendlist));
	int content_len = 0;
	head = list;
	if (list != NULL)
	{
		list->chunk_len = 0;
		memset(list->chunk_buff, 0, sizeof(list->chunk_buff));
		list->next = NULL;
		while(1)
		{			
			int offset = 0;
			// 对于大数据包应该支持续传
			while ((len = read(file_fd, list->chunk_buff + offset, MAX_MSG_LEN - offset)) > 0)
			{				
				offset += len;
				content_len += offset;
				if (offset >= MAX_MSG_LEN)
				{		
					break;
				}
			}
			list->chunk_len = offset;
			printf("insert chunk, len=%d\n", offset);
		
			if (len < 0)
			{
				if (errno == EINTR ||
				errno == EAGAIN)
				{
					printf("errno=%d\n", errno);
				}
				else
				{
					code = ZSC_500;
					request->zr_code = code;
					response_header(session, request);
					sendlist_free(NULL, head);	// 释放资源
					return -1;
				}
			}
			else if (len == 0)
			{
				// EOF 
				break;
			}
			next = (struct _zhttp_sendlist*)malloc(sizeof(struct _zhttp_sendlist));
			next->next = NULL;
			list->next = next;
			list = next;			
		}				
	}
	
	sendlist_insert(session, head);	
	
	// response_header(session, request);
	// 正确的报文，特殊处理	
	switch(request->zr_httpver)
	{
		case HTTP_VER_I09:
			{
				strncpy(header, HTTP_VER_C09, strlen(HTTP_VER_C09));
			}
			break;
		case HTTP_VER_I10:
			{
				strncpy(header, HTTP_VER_C10, strlen(HTTP_VER_C10));
			}
			break;
		case HTTP_VER_I11:
			{
				strncpy(header, HTTP_VER_C11, strlen(HTTP_VER_C11));
			}
			break;
	}
	
	strncat(header, ZSC_200_DESC, strlen(ZSC_200_DESC));
	len = strlen(header);
	memcpy(session->write_buff + session->write_offset, header, len);
	session->write_offset += len;
	len = get_content_type(send_file, session->write_buff + session->write_offset);
	session->write_offset += len;
	len = get_date(session->write_buff + session->write_offset);
	session->write_offset += len;
	len = get_server(session->write_buff + session->write_offset);
	session->write_offset += len;
	len = get_content_length(content_len, session->write_buff + session->write_offset);
	session->write_offset += len;
	len = strlen(HTTP_EOF);
	memcpy(session->write_buff + session->write_offset, HTTP_EOF, len);
	session->write_offset += len;
	// 到此为止 正确报文头部已组建完毕	
	// 报文在链表中
	poll_modify(session, EPOLLET | EPOLLIN | EPOLLOUT);
	return 0;
}

int on_zhttp_response(struct _zhttp_session* session, struct _zhttp_request* request)
{
	if (session == NULL ||
	request == NULL)
	{
		return -1;
	}
	int fd = session->zs_fd;	
	int ret = response_content(session, request);
	free(request);
	return ret;
}