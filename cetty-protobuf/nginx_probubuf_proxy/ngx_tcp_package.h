/*
 * ngx_tcp_package.h
 *
 *  Created on: Dec 11, 2012
 *      Author: chenhl
 */

#ifndef NGX_TCP_PACKAGE_H_
#define NGX_TCP_PACKAGE_H_

#define SERVICE_METHOD_SIZE 128
#define MESSAGE_HEAD_SIZE 4

#include <ngx_tcp.h>

enum reading_state_t {
    S_SIZE_READING,
    S_DATA_READING,
    S_READING_END
};

enum WireType {
    WIRETYPE_VARINT           = 0,
    WIRETYPE_FIXED64          = 1,
    WIRETYPE_LENGTH_DELIMITED = 2,
    WIRETYPE_START_GROUP      = 3,
    WIRETYPE_END_GROUP        = 4,
    WIRETYPE_FIXED32          = 5,
};

typedef struct ngx_tcp_reading_item_s {
    enum reading_state_t        state;        /** 当前的读状态 */
    ngx_uint_t                  nexpected;    /** 还期望读多少字节 */
    ngx_uint_t                  nread;        /** 已经读取了多少字节 */
    ngx_buf_t                  *buffer;       /** 消息缓冲区 */
} ngx_tcp_reading_item_t;

typedef struct ngx_tcp_protobuf_msg_s {
    ngx_buf_t                  *buf;
    ngx_uint_t                  index;
} ngx_tcp_protobuf_msg_t;

void ngx_tcp_update_reading_item (ngx_tcp_reading_item_t *item, int n);
void ngx_tcp_parse_protobuf_msg (ngx_buf_t *buf, ngx_tcp_session_t *s);

void ngx_tcp_protobuf_decode_field(ngx_tcp_protobuf_msg_t *msg,
	 ngx_int_t *wt, ngx_int_t *fn, ngx_int_t *fl);
ngx_int_t ngx_tcp_protobuf_read_bytes (ngx_tcp_protobuf_msg_t *msg,
	 char *dst, ngx_uint_t n);

ngx_int_t ngx_tcp_protobuf_jump_bytes (ngx_tcp_protobuf_msg_t *msg, ngx_uint_t n);
ngx_int_t ngx_tcp_protobuf_decode_varint(ngx_tcp_protobuf_msg_t *msg);

void ngx_tcp_reverse_bytes_order(u_char *src, u_char *dst, ngx_uint_t n);
#endif /* NGX_TCP_PACKAGE_H_ */
