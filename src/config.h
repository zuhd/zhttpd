#ifndef ZHTTP_CONFIG_H
#define ZHTTP_CONFIG_H
/**
1，遵循option=value 的格式
2，忽略#注释行
3，忽略空格 Tab等特殊字符
4，每行的字节数不能超过0xFF个
5，支持域嵌套，但不遵循XML协议
*/
#include <stdio.h>
#include <stdlib.h>
/**
@Author:ZUHD
@Date:2014-11-5
*/
#define ASCII_HT 9	// TAB键
#define ASCII_LF 10 // 换行键
#define ASCII_FF 12 // 换页键
#define ASCII_CR 13 // 回车键
#define ASCII_SP 32 // 空格键
#define ASCII_CM 35 // #注释
#define MAX_CHAR 256 // 每行最多的字符

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