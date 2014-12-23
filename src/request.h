/**
@Author:ZUHD
@Date:2014-11-17
*/
#ifndef ZHTTP_REQUEST_H
#define ZHTTP_REQUEST_H

#include "defines.h"

struct _zhttp_request
{
	enum _zhttp_req_method zr_method;
	enum _zhttp_status_code zr_code;
	int zr_httpver;
	int zr_keepalive;
	int zr_sendfile;
	char zr_uri[MAX_URI];
	char zr_domain[MAX_DOMAIN];
	char zr_useragent[MAX_USERAGENT];
};

struct _zhttp_session;
int on_zhttp_request(struct _zhttp_session* session, struct _zhttp_request* request);

#endif