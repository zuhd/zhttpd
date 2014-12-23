#ifndef ZHTTP_CONFIG_H
#define ZHTTP_CONFIG_H
/**
1����ѭoption=value �ĸ�ʽ
2������#ע����
3�����Կո� Tab�������ַ�
4��ÿ�е��ֽ������ܳ���0xFF��
5��֧����Ƕ�ף�������ѭXMLЭ��
*/
#include <stdio.h>
#include <stdlib.h>
/**
@Author:ZUHD
@Date:2014-11-5
*/
#define ASCII_HT 9	// TAB��
#define ASCII_LF 10 // ���м�
#define ASCII_FF 12 // ��ҳ��
#define ASCII_CR 13 // �س���
#define ASCII_SP 32 // �ո��
#define ASCII_CM 35 // #ע��
#define MAX_CHAR 256 // ÿ�������ַ�

#define SERVER_BEG "<server>"
#define SERVER_IP "ip"
#define SERVER_IP_VAL 0
#define SERVER_PORT "port"
#define SERVER_PORT_VAL 8900
#define SERVER_HOME "home"
#define SERVER_HOME_VAL "/home/shaman/www"
#define SERVER_DOMAIN "domain"
#define SERVER_DOMAIN_VAL "localhost"
#define SERVER_END "</server>"

struct _zhttp_server
{
	char ip[32];
	short port;
	char home[64];
	char domain[64];
};

struct _zhttp_server zhttp_server;

extern int load_config(const char* file_name);

#endif