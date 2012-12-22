/*
 * ngx_tcp_generic_proxy_module.h
 *
 *  Created on: Dec 7, 2012
 *      Author: chenhl
 */

#ifndef NGX_TCP_GENERIC_PROXY_MODULE_H_
#define NGX_TCP_GENERIC_PROXY_MODULE_H_

#include <ngx_tcp.h>

typedef struct ngx_tcp_proxy_srv_conf_s ngx_tcp_proxy_srv_conf_t;
typedef struct ngx_tcp_proxy_loc_conf_s ngx_tcp_proxy_loc_conf_t;

struct ngx_tcp_proxy_srv_conf_s{
    ngx_array_t                locations;  /* ngx_tcp_proxy_loc_conf_t */
    void                      *ctx;
    size_t                     buffer_size;
};

struct ngx_tcp_proxy_loc_conf_s{
    ngx_tcp_upstream_conf_t   upstream;

    ngx_str_t                 url;
    size_t                    buffer_size;

    ngx_str_t                 name;        /* location name */
};

extern ngx_module_t ngx_tcp_proxy_module;


#endif /* NGX_TCP_GENERIC_PROXY_MODULE_H_ */
