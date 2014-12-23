/**
@Author:ZUHD
@Date:2014-11-5
*/
#include "sys.h"
#include "config.h"

static char* trim_char(char* buff)
{
	char* index = buff;
	while (*index != '\0')
	{
		switch (*index)
		{
			case ASCII_LF:
			case ASCII_FF:
			case ASCII_CR:
			case ASCII_CM:
			{
				return NULL;
			}	
			case ASCII_SP:
			case ASCII_HT:
			{
				index++;
				break;
			}
			default:
				if (isalnum(*index) || (*index == '<'))
				{
					return index;
				}
				else
				{
					return NULL;
				}
		}
	}
	return NULL;
}

// 读取有效的一行配置
static int load_line(FILE* fp, char* buff, char** index)
{
	while(fgets(buff, MAX_CHAR, fp))
	{
		*index = trim_char(buff);
		if (*index == NULL)
		{
			return 0;
		}
		else
		{
			return 1;
		}
	}
	return -1;
}

static int load_server_sec(FILE* fp)
{
	int ret;
	char* index;
	char buffer[MAX_CHAR];
	while ((ret = load_line(fp, buffer, &index)) >= 0)
	{
		if (strncmp(index, SERVER_END, strlen(SERVER_END)) == 0)
		{
			break;
		}
		else if (strncmp(index, SERVER_IP, strlen(SERVER_IP)) == 0)
		{
			// 去掉尾部的\r\n
			char* str = strchr(index + strlen(SERVER_IP) + 1, '\r');			
			char* sub = strtok(index + strlen(SERVER_IP) + 1, str);
			if (sub != NULL)
			{
				strncpy(zhttp_server.ip, sub, strlen(sub));
			}			
			//strcpy(zhttp_server.ip, index + strlen(SERVER_IP) - 1);
		}
		else if (strncmp(index, SERVER_PORT, strlen(SERVER_PORT)) == 0)
		{
			// 去掉尾部的\r\n			
			zhttp_server.port = atoi(index + strlen(SERVER_PORT) + 1);
		}
		else if (strncmp(index, SERVER_HOME, strlen(SERVER_HOME)) == 0)
		{
			char* str = strchr(index + strlen(SERVER_HOME) + 1, '\r');			
			char* sub = strtok(index + strlen(SERVER_HOME) + 1, str);
			if (sub != NULL)
			{
				strncpy(zhttp_server.home, sub, strlen(sub));
			}			
		}
		else if (strncmp(index, SERVER_DOMAIN, strlen(SERVER_DOMAIN)) == 0)
		{
			char* str = strchr(index + strlen(SERVER_DOMAIN) + 1, '\r');			
			char* sub = strtok(index + strlen(SERVER_DOMAIN) + 1, str);
			if (sub != NULL)
			{
				strncpy(zhttp_server.domain, sub, strlen(sub));
			}			
		}
	}
}

int load_config(const char* file_name)
{
	FILE* fp = fopen(file_name, "rt");
	if (fp == NULL)
	{
		return -1;
	}
	int ret;
	char* index;
	char buffer[MAX_CHAR];
	while ((ret = load_line(fp, buffer, &index)) >= 0)
	{
		if (ret > 0)
		{
			// 有效行
			if (strncmp(index, SERVER_BEG, strlen(SERVER_BEG)) == 0)
			{
				load_server_sec(fp);
				return;
			}
		}
	}
}