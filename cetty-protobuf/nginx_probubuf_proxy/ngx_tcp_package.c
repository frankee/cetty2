/*
 * ngx_tcp_package.c
 *
 *  Created on: Dec 11, 2012
 *      Author: chenhl
 */

#include <ngx_tcp_package.h>

#define TAG_TYPE_BITS 3
#define TAG_TYPE_MASK ((1 << TAG_TYPE_BITS) - 1)

#define ngx_tcp_get_tag_wire_type(tag) (tag & TAG_TYPE_MASK)
#define ngx_tcp_get_tag_field_number(tag) (tag >> TAG_TYPE_BITS)

void
ngx_tcp_update_reading_item(ngx_tcp_reading_item_t *item, int n)
{
	if(n > 0) {
		item->nread += n;
		item->nexpected -= n;

		return;
	}

	if(n == 0) {
		if(item->state == S_SIZE_READING) {
			item->state = S_DATA_READING;
			item->nexpected = 0;
			ngx_tcp_reverse_bytes_order(item->buffer->pos, (u_char *)&item->nexpected, MESSAGE_HEAD_SIZE);
			item->nread = 0;
		} else if(item->state == S_DATA_READING) {
			item->state = S_READING_END;
			item->nread = 0;
			item->nexpected = MESSAGE_HEAD_SIZE;
		}
	}
}

void
ngx_tcp_protobuf_decode_field(ngx_tcp_protobuf_msg_t *msg,
	ngx_int_t *wt, ngx_int_t *fn, ngx_int_t *fl) {

	ngx_int_t         tag;

	tag = ngx_tcp_protobuf_decode_varint(msg);
	*wt = ngx_tcp_get_tag_wire_type(tag);
	*fn = ngx_tcp_get_tag_field_number(tag);

	if (*wt == WIRETYPE_LENGTH_DELIMITED) {
		*fl = ngx_tcp_protobuf_decode_varint(msg);
	}
}

void
ngx_tcp_parse_protobuf_msg (ngx_buf_t *buf, ngx_tcp_session_t *s) {
	ngx_tcp_protobuf_msg_t    pfmt;
	ngx_int_t                 mark, wt, fn, fl;

	ngx_memzero(&pfmt, sizeof(ngx_tcp_protobuf_msg_t));
	pfmt.buf = buf;
	pfmt.index = 0;

	s->name.len = 0;
	mark = 0;

	ngx_tcp_protobuf_jump_bytes(&pfmt, 8);
    while ((pfmt.buf->last - pfmt.index) > 0 && mark < 2) {
    	ngx_tcp_protobuf_decode_field(&pfmt, &wt, &fn, &fl);
        switch (fn) {
        case 1:
            ngx_tcp_protobuf_decode_varint(&pfmt);
            break;

        case 2:
            ngx_tcp_protobuf_jump_bytes(&pfmt, 8);
            break;

        case 3:
            ngx_tcp_protobuf_read_bytes(&pfmt, s->name.data + s->name.len, fl);
            s->name.data[fl] = '.';
            s->name.len += (fl + 1);
			ngx_log_error(NGX_LOG_ERR, s->connection->log, 0,
            			  "Parse protobuf message error");
            mark ++;
            break;

        case 4:
            ngx_tcp_protobuf_read_bytes(&pfmt, s->name.data + s->name.len, fl);
            s->name.len += fl;
            mark ++;
            break;

        default:
            ngx_log_error(NGX_LOG_ERR, s->connection->log, 0,
            			  "Parse protobuf message error");
            return;
        }
    }
}

ngx_int_t
ngx_tcp_protobuf_read_bytes (ngx_tcp_protobuf_msg_t *msg,
		char *dst, ngx_uint_t n) {
	ngx_int_t          i;
	u_char            *current;

	current = msg->buf->pos + msg->index;
	if(msg->buf->last - current < n) {
	    return NGX_ERROR;
	}

    for(i = 0; i < n; ++i) {
    	dst[i] = msg->buf->pos[msg->index ++];
    }

    return NGX_OK;
}

ngx_int_t
ngx_tcp_protobuf_jump_bytes (ngx_tcp_protobuf_msg_t *msg, ngx_uint_t n) {
	u_char            *current;

	current = msg->buf->pos + msg->index;
    if(msg->buf->last - current < n) {
        return NGX_ERROR;
    }

    msg->index += n;

    return NGX_OK;
}

ngx_int_t
ngx_tcp_protobuf_decode_varint(ngx_tcp_protobuf_msg_t *msg) {
    ngx_int_t             temp = 0, ret = 0, i = 0, off = 0;
    u_char                ch;

    while(1) {
    	ngx_tcp_protobuf_read_bytes(msg, &ch, 1);
        temp = ch;

    	if(temp & 0x80) {
    		ret |= (temp << 1 >> 1) << (7 * i);
    		++i;
    	} else {
    		off = 7 * i;
    		ret = ret + (temp << off);
    		break;
    	}
    }

    return ret;
}

void
ngx_tcp_reverse_bytes_order(u_char *src, u_char *dst, ngx_uint_t n) {
	ngx_uint_t           i;

	for(i = 0; i <= n; ++i) {
		dst[i] = src[n - i - 1];
	}
}
