/**
@Author:ZUHD
@Date:2015-08-07
*/
#ifndef CGI_H_
#define CGI_H_

#include "defines.h"

// 在header之后/r/n/r/n之后还有数据就是content
// 长度就是Content Length的值
struct _zhttp_request;

// 脚本参数 来自uri或是content
typedef struct _zhttp_cgi_para
{
	char key[MAX_KEY_LEN];
	char value[MAX_VALUE_LEN];
	struct _zhttp_cgi* next;
} st_cgi_para_head;

// script来自uri的头部
typedef struct _zhttp_cgi
{
	char script[MAX_SCRIPT_LEN];
	st_cgi_para_head* head;
} st_cgi;

bool check_cgi_request(struct _zhttp_request* request);
void on_zhttp_cgi_request(struct _zhttp_request* request, struct _zhttp_cgi* cgi);
#endif
