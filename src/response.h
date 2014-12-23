/**
@Author:ZUHD
@Date:2014-11-20
*/
#ifndef ZHTTP_RESPONSE_H
#define ZHTTP_RESPONSE_H

struct _zhttp_session;
struct _zhttp_request;
extern int response_header(struct _zhttp_session* session, struct _zhttp_request* request);
extern int on_zhttp_response(struct _zhttp_session* session, struct _zhttp_request* request);
#endif