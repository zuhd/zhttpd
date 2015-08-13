/**
@Author:ZUHD
@Date:2014-11-18
*/

#include "sys.h"
#include "defines.h"
#include "request.h"
#include "config.h"
#include "log.h"
#include "session.h"

// 当前的游标
static int get_method(char* data, int len, struct _zhttp_request* request)
{
	if (data == NULL)
	{
		return -1;
	}
	// 获取方法
	if (len > 4 && strncmp(data, "GET ", 4) == 0)
	{
		request->zr_method = ZRM_GET;
		return 4;
	}
	if (len > 5 && strncmp(data, "HEAD ", 5) == 0)
	{
		request->zr_method = ZRM_HEAD;
		return 5;
	}
	if (len > 4 && strncmp(data, "PUT ", 4) == 0)
	{
		request->zr_method = ZRM_PUT;
		return 4;
	}
	if (len > 5 && strncmp(data, "POST ", 5) == 0)
	{
		request->zr_method = ZRM_POST;
		return 5;
	}
	return -1;
}

// 返回当前的游标
static int get_uri(char* data, int len, struct _zhttp_request* request)
{
	// 注意%后的转义字符
	// 比如%30转义后就是0
	// 转义后的字符范围为 32-126
	// 第一个字符<=7 第二个全字符
	if (data == NULL)
	{
		return -1;
	}
	memset(request->zr_uri, 0, sizeof(request->zr_uri));
	int offset = 0;
	int index = 0;
	char* p = data;
	while (index < MAX_URI &&
	(*p != ASCII_SP))
	{
		if (*p < 32 || *p > 126)
		{
			log("illegal character");
			return -1;
		}
		if (*p == '%')
		{
			// 处理转义字符
			p++;
			int n = 0;
			if (*p >= '0' && *p <= '7')
			{
				n = (*p - 48) * 16;
			}
			else
			{
				return -1;
			}
			p++;
			if (*p >= '0' && *p <= '9')
			{
				n += (*p - 48);
			}
			else if (*p >= 'A' && *p <= 'F')
			{
				n += (*p - 55);
			}
			else if (*p >= 'a' && *p <= 'f')
			{
				n += (*p - 87);
			}
			
			if (n < 32 || n > 126)
			{
				return -1;
			}
			request->zr_uri[index] = n;
			index++;
			p++;
			offset += 3;
		}
		else
		{
			request->zr_uri[index] = *p;
			index++;
			p++;
			offset++;
		}
	}
	if (*p == ASCII_SP)
	{
		offset++;
	}
	return offset;
}

// 返回当前的游标
static int get_httpver(char* data, int len, struct _zhttp_request* request)
{
	if (data == NULL)
	{
		return -1;
	}
	
	if (strncmp(data + 8, HTTP_EOF, 2) != 0)
	{
		return -1;
	}
	
	if (strncmp(data, HTTP_VER_C09, 8) == 0)
	{
		request->zr_httpver = HTTP_VER_I09;
	}		
	else if (strncmp(data, HTTP_VER_C10, 8) == 0)
	{
		request->zr_httpver = HTTP_VER_I10;
	}	
	else if (strncmp(data, HTTP_VER_C11, 8) == 0)
	{
		request->zr_httpver = HTTP_VER_I11;
	}
	else
	{
		return -1;
	}
	return 10;
}

// 不再移动指针
static int get_content_len(char* data, int len, struct _zhttp_request* request)
{
	if (data == NULL)
	{
		return -1;
	}
	const char* cl = "Content-Length: ";
	int cl_len = strlen(cl);
	char* p1 = strstr(data, cl);	
	char* p2 = strstr(data, HTTP_EOF);
	if (p1 == NULL ||
		p2 == NULL)
	{
		return -1;
	}
	char len[16] = {0};
	strncpy(len, p1 + cl_len, p2 - p1 - cl_len);
	request->zr_content_len = atoi(len);
	return request->zr_content_len;
}

// 获取post的content
static int get_content(char* data, int len, struct _zhttp_request* request)
{
	if (data == NULL)
	{
		return -1;
	}
	if (request->zr_content_len <= 0 ||
		request->zr_content_len > MAX_CONTENT_LEN)
	{
		return -1;
	}
	const char* header_eof = "\r\n\r\n";
	int eof_len = strlen(header_eof);
	char* p = strstr(data, header_eof);
	strncpy(request->content, p + eof_len, request->zr_content_len);
	return 0;
}

int on_zhttp_request(struct _zhttp_session* session, struct _zhttp_request* request)
{
	if (session == NULL)
	{
		return -1;
	}
	
	if (request == NULL)
	{
		return -1;
	}
	int fd = session->zs_fd;
	char* data = session->read_buff;
	int len = session->read_offset;
	// 不管该包是否合法，都丢弃掉
	// http请求不存在粘包情况，并
	// 假设一次请求的数据都能接收
	session->read_offset = 0;
	// 先对协议做合法性检查
	// GET<SP>/<SP>HTTP/1.1<\r><\n>
	// 至少16个字符
	if (len < 16)
	{
		request->zr_code = ZSC_400;
		log("bad request");
		response_header(session, request);
		return -1;		
	}
	
	// get method 判断是否合法
	char* p = data;
	int ret = 0;
	while (*p == ASCII_LF || *p == ASCII_CR)
	{
		p++;
	}
	if ((ret = get_method(p, len, request)) < 0)
	{
		request->zr_code = ZSC_400;
		log("bad request");
		response_header(session, request);
		return -1;
	}
	
	p += ret;
	
	if ((ret = get_uri(p, len, request)) < 0)
	{
		request->zr_code = ZSC_400;
		log("bad request");
		response_header(session, request);
		return -1;
	}
	
	p += ret;
	
	if ((ret = get_httpver(p, len, request)) < 0)
	{
		request->zr_code = ZSC_405;
		log("http version error");
		response_header(session, request);
		return -1;
	}
	
	p += ret;

	get_content_len(p, len, request);
	get_content(p, len, request);
	
	// TODO more works about the body
	on_zhttp_response(session, request);
	return 0;
}