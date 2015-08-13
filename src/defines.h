/**
@Author:ZUHD
@Date:2014-11-17
*/

#ifndef ZHTTP_DEFINES_H
#define ZHTTP_DEFINES_H

#define MAX_MSG_LEN (1024 * 10)
#define MIN_REQ_LEN 16
#define MAX_URI 200
#define MAX_DOMAIN 128
#define MAX_USERAGENT 128
#define MAX_CONTENT_LEN 2048
#define MAX_KEY_LEN 256
#define MAX_VALUE_LEN 1024
#define MAX_SCRIPT_LEN 1024

#define HTTP_VER_C09 ("HTTP/0.9")
#define HTTP_VER_I09 (9)
#define HTTP_VER_C10 ("HTTP/1.0")
#define HTTP_VER_I10 (10)
#define HTTP_VER_C11 ("HTTP/1.1")
#define HTTP_VER_I11 (11)
#define HTTP_EOF ("\r\n")

#define ZSC_200_DESC (" 200 OK\r\n")
#define ZSC_201_DESC (" 201 Created\r\n")
#define ZSC_202_DESC (" 202 Accepted\r\n")
#define ZSC_204_DESC (" 204 No Content\r\n")

#define ZSC_300_DESC (" 300 Multiple Choices\r\n")
#define ZSC_301_DESC (" 301 Moved Permanently\r\n")
#define ZSC_302_DESC (" 302 Moved Temporarily\r\n")
#define ZSC_304_DESC (" 304 Not Modified\r\n")

#define ZSC_400_DESC (" 400 Bad Request\r\n")
#define ZSC_401_DESC (" 401 Unauthorized\r\n")
#define ZSC_403_DESC (" 403 Forbidden\r\n")
#define ZSC_404_DESC (" 404 Not Found\r\n")
#define ZSC_405_DESC (" 405 Http Version Undefined\r\n")
#define ZSC_406_DESC (" 406 URI Unacceptable\r\n")

#define ZSC_500_DESC (" 500 Internal Server Error\r\n")
#define ZSC_501_DESC (" 501 Not Implemented\r\n")
#define ZSC_502_DESC (" 502 Bad Gateway\r\n")
#define ZSC_503_DESC (" 503 Service Unavailable\r\n")

#define TEST_DEMO

enum _zhttp_status_code
{
	// 成功 2XX
	ZSC_200,
	ZSC_201, 
	ZSC_202,
	ZSC_204,
	// 重定向 3XX
	ZSC_300,
	ZSC_301,
	ZSC_302,
	ZSC_304,
	// 客户端错误 4XX
	ZSC_400,
	ZSC_401,
	ZSC_403,
	ZSC_404,
	ZSC_405,
	ZSC_406,
	// 服务器错误 5XX
	ZSC_500,
	ZSC_501,
	ZSC_502,
	ZSC_503,
};


enum _zhttp_req_method
{
	ZRM_UNKNOWN,
	ZRM_GET,
	ZRM_HEAD,
	ZRM_PUT,
	ZRM_POST,
};
#endif
