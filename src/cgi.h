/**
@Author:ZUHD
@Date:2015-08-07
*/
#ifndef CGI_H_
#define CGI_H_

#include "defines.h"

// ��header֮��/r/n/r/n֮�������ݾ���content
// ���Ⱦ���Content Length��ֵ
struct _zhttp_request;

// �ű����� ����uri����content
typedef struct _zhttp_cgi_para
{
	char key[MAX_KEY_LEN];
	char value[MAX_VALUE_LEN];
	struct _zhttp_cgi* next;
} st_cgi_para_head;

// script����uri��ͷ��
typedef struct _zhttp_cgi
{
	char script[MAX_SCRIPT_LEN];
	st_cgi_para_head* head;
} st_cgi;

bool check_cgi_request(struct _zhttp_request* request);
void on_zhttp_cgi_request(struct _zhttp_request* request, struct _zhttp_cgi* cgi);
#endif
