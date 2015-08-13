/**
@Author:ZUHD
@Date:2015-08-07
*/
#include <stdio.h>
#include <string.h>
#include "cgi.h"

bool check_cgi_request(struct _zhttp_request * request)
{
	if (request->zr_method == ZRM_POST)
	{
		return true;
	}
	else if (request->zr_method == ZRM_GET)
	{
		// �ж��ַ����Ƿ���?
		return strstr(request->zr_uri, "?") != NULL;
	}
	return false;
}

void on_zhttp_cgi_request(struct _zhttp_request * request, struct _zhttp_cgi* cgi)
{
	// ������get�е�ƴ���ֶΣ�����post�е�body�ֶ�
	// ����key-value���� fork+exec
	char* cgi_content = NULL;
	if (request->zr_method == ZRM_POST)
	{
		cgi_content = request->zr_content;
	}
	else if (request->zr_method == ZRM_GET)
	{
		cgi_content = strstr(request->zr_uri, "?");
		if (cgi_content != NULL)
		{
			// ����ƶ�
			cgi_content += 1;
		}
	}

	// ���request->content�����ݣ���&����
	int n = 0;
	char* content[256];
	char* p = strtok(request->zr_content, "&");
	content[n++] = p;
	while (p != NULL)
	{
		p = strtok(NULL, "&");
		if (n >= 256)
		{
			break;
		}
		content[n++] = p;
	}

	st_cgi_para_head* para_head = cgi->head;
		
	for (int i = 0 ; i < n; i++)
	{
		st_cgi_para_head* head = new st_cgi_para_head;
		char* key = strtok(content[i], "=");
		char* value = strtok(NULL, "=");
		strncpy(head->key, key, strlen(key) + 1);
		strncpy(head->value, value, strlen(value) + 1);
		head->next = NULL;
		if (para_head == NULL)
		{
			para_head = head;
		}
		else
		{
			para_head->next = head;
			para_head = para_head->next;
		}
			
	}
}
