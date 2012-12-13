

#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_tcp.h>



typedef struct ngx_tcp_proxy_s {
    ngx_peer_connection_t    *upstream;
    ngx_buf_t                *buffer;
} ngx_tcp_proxy_ctx_t;

static void ngx_tcp_proxy_init_session(ngx_tcp_session_t *s); 
static void ngx_tcp_proxy_init_upstream(ngx_tcp_session_t *s, void *conf);
static void ngx_tcp_upstream_init_proxy_handler(ngx_tcp_session_t *s, 
    ngx_tcp_upstream_t *u);
static char *ngx_tcp_proxy_pass(ngx_conf_t *cf, ngx_command_t *cmd,
		void *conf);
static void ngx_tcp_proxy_handler(ngx_event_t *ev);
static void ngx_tcp_proxy_peer_handler(ngx_event_t *ev);

static void *ngx_tcp_proxy_create_srv_conf(ngx_conf_t *cf);
static char *ngx_tcp_proxy_merge_srv_conf(ngx_conf_t *cf, void *parent,
    void *child);

static void *ngx_tcp_proxy_create_loc_conf(ngx_conf_t *cf);

static ngx_tcp_upstream_t *ngx_tcp_proxy_get_session_upstream(ngx_tcp_session_t *s);
static ngx_tcp_upstream_t *ngx_tcp_procy_get_server_upstream(ngx_tcp_session_t *s);
static ngx_tcp_upstream_t *ngx_tcp_proxy_get_upstream(ngx_tcp_session_t *s);

static ngx_uint_t ngx_tcp_check_has_active_upstream(ngx_tcp_session_t *s);

static ngx_tcp_protocol_t  ngx_tcp_generic_protocol = {

    ngx_string("tcp_generic"),
    { 0, 0, 0, 0 },
    NGX_TCP_GENERIC_PROTOCOL,
    ngx_tcp_proxy_init_session,
    NULL,
    NULL,
    ngx_string("500 Internal server error" CRLF)

};


static ngx_command_t  ngx_tcp_proxy_commands[] = {

    { ngx_string("proxy_pass"),
      NGX_TCP_LOC_CONF|NGX_CONF_TAKE1,
      ngx_tcp_proxy_pass,
      NGX_TCP_LOC_CONF_OFFSET,
      0,
      NULL },

    { ngx_string("buffer_size"),
      NGX_TCP_SRV_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_size_slot,
      NGX_TCP_SRV_CONF_OFFSET,
      offsetof(ngx_tcp_proxy_srv_conf_t, buffer_size),
      NULL },

    { ngx_string("proxy_buffer"),
      NGX_TCP_LOC_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_size_slot,
      NGX_TCP_LOC_CONF_OFFSET,
      offsetof(ngx_tcp_proxy_loc_conf_t, buffer_size),
      NULL },

    { ngx_string("proxy_connect_timeout"),
      NGX_TCP_LOC_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_msec_slot,
      NGX_TCP_LOC_CONF_OFFSET,
      offsetof(ngx_tcp_proxy_loc_conf_t, upstream.connect_timeout),
      NULL },

    { ngx_string("proxy_read_timeout"),
      NGX_TCP_LOC_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_msec_slot,
      NGX_TCP_LOC_CONF_OFFSET,
      offsetof(ngx_tcp_proxy_loc_conf_t, upstream.read_timeout),
      NULL },

    { ngx_string("proxy_send_timeout"),
      NGX_TCP_LOC_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_msec_slot,
      NGX_TCP_LOC_CONF_OFFSET,
      offsetof(ngx_tcp_proxy_loc_conf_t, upstream.send_timeout),
      NULL },

    ngx_null_command
};


static ngx_tcp_module_t  ngx_tcp_proxy_module_ctx = {
    &ngx_tcp_generic_protocol,             /* protocol */

    NULL,                                  /* create main configuration */
    NULL,                                  /* init main configuration */

    ngx_tcp_proxy_create_srv_conf,         /* create server configuration */
    ngx_tcp_proxy_merge_srv_conf,          /* merge server configuration */

    ngx_tcp_proxy_create_loc_conf,         /* create location configuration */
    NULL                                   /* merge location configuration */
};


ngx_module_t  ngx_tcp_proxy_module = {
    NGX_MODULE_V1,
    &ngx_tcp_proxy_module_ctx,             /* module context */
    ngx_tcp_proxy_commands,                /* module directives */
    NGX_TCP_MODULE,                        /* module type */
    NULL,                                  /* init master */
    NULL,                                  /* init module */
    NULL,                                  /* init process */
    NULL,                                  /* init thread */
    NULL,                                  /* exit thread */
    NULL,                                  /* exit process */
    NULL,                                  /* exit master */
    NGX_MODULE_V1_PADDING
};

static void 
ngx_tcp_proxy_init_session(ngx_tcp_session_t *s) 
{
    ngx_connection_t             *c;
    ngx_tcp_proxy_loc_conf_t     *plcf;
    ngx_tcp_proxy_srv_conf_t     *pscf;
    ngx_tcp_core_srv_conf_t      *cscf;

    c = s->connection;

    s->name.data = ngx_pcalloc(c->pool, SERVICE_METHOD_SIZE);

    ngx_log_debug0(NGX_LOG_DEBUG_TCP, c->log, 0, "tcp proxy init session");

    cscf = ngx_tcp_get_module_srv_conf(s, ngx_tcp_core_module);
    pscf = ngx_tcp_get_module_srv_conf(s, ngx_tcp_proxy_module);

    s->buffer = ngx_create_temp_buf(s->connection->pool, pscf->buffer_size);
    if (s->buffer == NULL) {
        ngx_tcp_finalize_session(s);
        return;
    }

    s->out.len = 0;

    c->write->handler = ngx_tcp_proxy_peer_handler;
    c->read->handler = ngx_tcp_proxy_handler;

    if(ngx_add_conn){
    	if(ngx_add_conn(c) == NGX_ERROR){
    		ngx_tcp_finalize_session(s);
    		return;
    	}
    }

    ngx_add_timer(c->read, cscf->timeout);

    return;
}

static void
ngx_tcp_proxy_handler(ngx_event_t *ev)
{
	ngx_tcp_core_srv_conf_t  *cscf;
    ngx_connection_t         *c, *pc;
    ngx_tcp_session_t        *s;
    ngx_tcp_upstream_t       *upstream;
    ngx_buf_t                *b, *pb;
    ngx_tcp_reading_item_t   *item;
    size_t                    size;
    ssize_t                   n;
    ngx_uint_t                do_write, do_read, i, flag;
    ngx_err_t                 err;
    ngx_str_t                *name;

    c = ev->data;
    s = c->data;

    cscf = ngx_tcp_get_module_srv_conf(s, ngx_tcp_core_module);

    do_write = ev->write ? 1 : 0;
    do_read = !do_write;

    if(ev->timedout) {
    	if(do_read) {
    		fprintf(stdout, "client timedout\n");
    		c->log->action = "proxying";
    		ngx_log_error(NGX_LOG_INFO, c->log, NGX_ETIMEDOUT,
    				      "proxy timed out");

    		ngx_tcp_finalize_session(s);
    		return;
    	}
    }

    item = c->data2;
    b = item->buffer;

    do_write = ev->write ? 1 : 0;

    c = s->connection;

    if(do_write) {
        pc = ev->data;
        pb = pc->buffer;
    }

    for(;;) {
    	if(do_write){
    		size = pb->last - pb->pos;

    		if(size && pc->write->ready) {
    			n = pc->send(pc, pb->pos, size);

    			err = ngx_socket_errno;
    			if (n == NGX_ERROR) {
    			    ngx_tcp_finalize_session(s);
    			    return;
    			}

    			if (n > 0) {
    			    pb->pos += n;
    			    if (pb->pos == pb->last) {
    			         pb->pos = pb->start;
    			         pb->last = pb->start;
    			    }
    			}
    		}
    	}

		if(c->read->ready) {
			size = b->end - b->last;
			if(size >= item->nexpected) {
			    size = item->nexpected;
			} else {
			    ngx_log_error(NGX_LOG_ERR, ev->log, 0, "client receive buffer "
			    			  "size is not enough.");
			    ngx_tcp_finalize_session(s);

			    return;
			}

			if(size <= 0) break;

			n = c->recv(c, b->last, size);

			err = ngx_socket_errno;
			ngx_log_debug1(NGX_LOG_DEBUG_TCP, ev->log, 0,
			                   "tcp proxy handler recv:%d", n);

			if (n == NGX_AGAIN || n == 0) {
			    break;
			}

			if (n == NGX_ERROR) {
			   	c->read->eof = 1;
			   	break;
			}

			if(n > 0) {
				b->last += n;

				if(n == size) {
				    ngx_tcp_update_reading_item(item, 0);
				} else {
				    ngx_tcp_update_reading_item(item, n);
				}

			    if(item->state == S_READING_END) {

                    ngx_tcp_parse_protobuf_msg(b, s);

			    	upstream = ngx_tcp_proxy_get_upstream(s);
			    	if(upstream == NULL) {
			    		ngx_log_error(NGX_LOG_ERR, ev->log, 0,
			    				"Can't find upstream [%s]", s->name.data);
			    		ngx_tcp_finalize_session(s);
			    		return;
			    	}

			    	pc = upstream->peer.connection;
			    	pb = pc->buffer;

			    	size = b->last - b->pos;
			    	if(size > pb->end - pb->last) {
			    	    ngx_log_error(NGX_LOG_ERR, ev->log, 0,
			    	    		"Backend's send buffer is not enough");
			    	    ngx_tcp_finalize_session(s);
			    	    return;
			    	}

			    	ngx_memcpy(pb->last, b->pos, size);
			    	pb->last += size;
			    	do_write = 1;

			    	b->pos = b->start;
			        b->last = b->start;

			        item->state = S_SIZE_READING;
			    }

				continue;
			}
		}

		break;
    }

    ngx_log_debug1(NGX_LOG_DEBUG_TCP, ev->log, 0,
                   "tcp proxy handler: %d", c->fd);

    if(c->read->eof) {
    	ngx_log_error(NGX_LOG_DEBUG, c->log, 0, "proxied session done");

    	// todo copy buffer
    	// ...


    	flag = 1;
        /* todo send data in all backends' send buffer

        upstream = s->upstreams.elts;
        for(i = 0; i < s->upstreams.nelts; ++i) {
        	if(upstream[i].unused) {
        		continue;
        	}

        	pc = upstream[i].peer.connection;
            pb = pc->buffer;
            size = pb->last - pb->pos;
            if(size && pc->write->ready) {
            	n = pc->send(pc, pb->pos, size);

            	err = ngx_socket_errno;
            	if (n == NGX_ERROR) {
            	    ngx_tcp_finalize_session(s);
            	    return;
            	}

            	if (n > 0) {
            	    pb->pos += n;
            	    if (pb->pos == pb->last) {
            	        pb->pos = pb->start;
            	        pb->last = pb->start;

            	        continue;
            	    }
                    flag = 0;
                }
            }
        }

        */

    	if(flag) {
    		ngx_tcp_finalize_session(s);
    	}

        return;
    }

    if(do_read) {
       	if(ev->timer_set) {
       		ngx_del_timer(ev);
       	}
       	ngx_add_timer(ev, cscf->timeout);
    }
}


static  void
ngx_tcp_proxy_init_upstream(ngx_tcp_session_t *s, void *conf)
{
    ngx_tcp_upstream_t           *u, *upstreams;
    ngx_tcp_proxy_loc_conf_t     *plcf = conf;

    s->connection->log->action = "ngx_tcp_proxy_init_upstream";

    if (plcf->upstream.upstream == NULL) {
        ngx_tcp_finalize_session(s);
        return;
    }

    if (ngx_tcp_upstream_create(s) != NGX_OK) {
        ngx_tcp_finalize_session(s);
        return;
    }

    upstreams = s->upstreams.elts;
    s->upstream = &upstreams[s->upstreams.nelts - 1];

    u = s->upstream;

    u->conf = &plcf->upstream;
    u->proxy_conf = plcf;

    u->name.data = plcf->name.data;
    u->name.len = plcf->name.len;

    u->write_event_handler = ngx_tcp_upstream_init_proxy_handler;
    u->read_event_handler = ngx_tcp_upstream_init_proxy_handler;

    ngx_tcp_upstream_init(s);

    return;
}


static void 
ngx_tcp_upstream_init_proxy_handler(ngx_tcp_session_t *s, ngx_tcp_upstream_t *u)
{
    ngx_connection_t             *c;
    ngx_tcp_proxy_loc_conf_t     *plcf;

    c = s->connection;
    c->log->action = "ngx_tcp_upstream_init_proxy_handler";

    ngx_log_debug0(NGX_LOG_DEBUG_TCP, s->connection->log, 0,
                   "tcp proxy upstream init proxy");

    plcf = u->proxy_conf;
    if (plcf == NULL) {
        ngx_tcp_finalize_session(s);
        return;
    }

    c = u->peer.connection;
    if (c->read->timedout || c->write->timedout) {
        ngx_tcp_upstream_next(s, u, NGX_TCP_UPSTREAM_FT_TIMEOUT);
        return;
    }

    if (ngx_tcp_upstream_check_broken_connection(s) != NGX_OK){
        ngx_tcp_upstream_next(s, u, NGX_TCP_UPSTREAM_FT_ERROR);
        return;
    }

    c->read->handler = ngx_tcp_proxy_peer_handler;
    c->write->handler = ngx_tcp_proxy_handler;

    // ngx_add_timer(c->read, plcf->upstream.read_timeout);
    // ngx_add_timer(c->write, plcf->upstream.send_timeout);

#if (NGX_TCP_SSL)

    /* 
     * The ssl connection with client may not trigger the read event again,
     * So I trigger it in this function.
     * */
    if (s->connection->ssl) {
        ngx_tcp_proxy_handler(s->connection->read); 
    }

#endif

    if(c->write->ready) {
    	c->write->handler(c->write);
    }

    return;
}

static char *
ngx_tcp_proxy_pass(ngx_conf_t *cf, ngx_command_t *cmd, void *conf) 
{
    ngx_tcp_proxy_loc_conf_t   *plcf = conf;

    u_short                     port = 80;
    ngx_str_t                  *value, *url = &plcf->url;
    ngx_url_t                   u;
    ngx_tcp_core_srv_conf_t    *cscf;
    ngx_tcp_core_loc_conf_t    *clcf;

    cscf = ngx_tcp_conf_get_module_srv_conf(cf, ngx_tcp_core_module);

    if (cscf->protocol && ngx_strncmp(cscf->protocol->name.data,
                                      (u_char *)"tcp_generic",
                                      sizeof("tcp_generic") - 1) != 0) {

        return "the protocol should be tcp_generic";
    }

    if (cscf->protocol == NULL) {
        cscf->protocol = &ngx_tcp_generic_protocol;
    }

    if (plcf->upstream.upstream) {
        return "is duplicate";
    }

    value = cf->args->elts;

    url = &value[1];

    ngx_memzero(&u, sizeof(u));

    u.url.len = url->len;
    u.url.data = url->data;
    u.default_port = port;
    u.uri_part = 1;
    u.no_resolve = 1;

    plcf->upstream.upstream = ngx_tcp_upstream_add(cf, &u, 0);
    if (plcf->upstream.upstream == NULL) {
        return NGX_CONF_ERROR;
    }

    clcf = ngx_tcp_conf_get_module_loc_conf(cf, ngx_tcp_core_module);
    plcf->name.data = clcf->name.data;
    plcf->name.len = clcf->name.len;

    return NGX_CONF_OK;
}

static void *
ngx_tcp_proxy_create_srv_conf(ngx_conf_t *cf)
{
	ngx_tcp_proxy_srv_conf_t *pscf;

	pscf = ngx_pcalloc(cf->pool, sizeof(ngx_tcp_proxy_srv_conf_t));
	if (pscf == NULL) {
		return NULL;
	}

	ngx_array_init(&pscf->locations, cf->pool, 4, sizeof(ngx_tcp_proxy_loc_conf_t));
	pscf->buffer_size = NGX_CONF_UNSET_SIZE;

    return pscf;
}

static char *
ngx_tcp_proxy_merge_srv_conf(ngx_conf_t *cf, void *parent, void *child)
{
    ngx_tcp_proxy_srv_conf_t *pscf = child, *prev = parent;
    ngx_tcp_proxy_loc_conf_t *conf = pscf->locations.elts;

    ngx_conf_merge_size_value(pscf->buffer_size, prev->buffer_size,
    		(size_t) ngx_pagesize);

    int i = 0;
    for(; i < pscf->locations.nelts; ++i) {
    	if(conf[i].buffer_size == NGX_CONF_UNSET_SIZE) {
    		conf[i].buffer_size = (size_t) ngx_pagesize;
    	}

    	if(conf[i].upstream.connect_timeout == NGX_CONF_UNSET_MSEC){
    		conf[i].upstream.connect_timeout = 60000;
    	}

    	if(conf[i].upstream.send_timeout == NGX_CONF_UNSET_MSEC) {
    		conf[i].upstream.send_timeout = 60000;
    	}

    	if(conf[i].upstream.read_timeout == NGX_CONF_UNSET_MSEC) {
    		conf[i].upstream.read_timeout = 60000;
    	}
    }

    return NGX_CONF_OK;
}

static void *
ngx_tcp_proxy_create_loc_conf(ngx_conf_t *cf){
    ngx_tcp_proxy_loc_conf_t          *plcf;
    ngx_tcp_proxy_srv_conf_t          *pscf;

    pscf = ngx_tcp_conf_get_module_srv_conf(cf, ngx_tcp_proxy_module);

    plcf = ngx_array_push(&pscf->locations);
    if(plcf == NULL) {
    	return NULL;
    }

    plcf->buffer_size = NGX_CONF_UNSET_SIZE;
    plcf->upstream.connect_timeout = NGX_CONF_UNSET_MSEC;
    plcf->upstream.send_timeout = NGX_CONF_UNSET_MSEC;
    plcf->upstream.read_timeout = NGX_CONF_UNSET_MSEC;

    return plcf;
}

static ngx_tcp_upstream_t *
ngx_tcp_proxy_get_upstream(ngx_tcp_session_t *s) {
	ngx_tcp_upstream_t *u = NULL;

	u = ngx_tcp_proxy_get_session_upstream(s);
	if(u == NULL) {
		u = ngx_tcp_procy_get_server_upstream(s);
	}

    return u;
}

static ngx_tcp_upstream_t *
ngx_tcp_proxy_get_session_upstream(ngx_tcp_session_t *s) {
	ngx_tcp_upstream_t          *upstreams;
    ngx_str_t                   *name;
    ngx_uint_t                   i;

    name = &s->name;

    upstreams = s->upstreams.elts;
	for(i = 0; i < s->upstreams.nelts; i ++) {
		if(upstreams[i].unused) {
			continue;
		}

		if(name->len != upstreams[i].name.len) {
			continue;
		}

		if(ngx_strncmp(name->data, upstreams[i].name.data, name->len) == 0) {
			return &upstreams[i];
		}
	}

	return NULL;
}

static ngx_tcp_upstream_t *
ngx_tcp_procy_get_server_upstream(ngx_tcp_session_t *s) {
	ngx_tcp_proxy_srv_conf_t    *pscf;
	ngx_tcp_proxy_loc_conf_t    *plcf, *plcfs;
	ngx_tcp_upstream_t          *u;
	ngx_str_t                   *name;
	ngx_uint_t                   i;

	name = &s->name;

	plcf = NULL;
	pscf = ngx_tcp_get_module_srv_conf(s, ngx_tcp_proxy_module);

    plcfs = pscf->locations.elts;
    for(i = 0; i < pscf->locations.nelts; ++i){
    	if(name->len != plcfs[i].name.len) {
    		continue;
    	}

    	if(ngx_strncmp(name->data, plcfs[i].name.data, name->len) == 0) {
            plcf = &plcfs[i];
            break;
    	}
    }

    if(plcf == NULL) {
    	return NULL;
    }

    ngx_tcp_proxy_init_upstream(s, plcf);

    return s->upstream;
}

static void
ngx_tcp_proxy_peer_handler(ngx_event_t *ev){
	ngx_tcp_core_srv_conf_t  *cscf;
	ngx_connection_t         *c, *pc;
	ngx_tcp_session_t        *s;
	ngx_tcp_upstream_t       *u, *us;
	ngx_buf_t                *b, *pb;
	ngx_tcp_reading_item_t   *item;
	size_t                    size;
	ssize_t                   n;
	ngx_uint_t                do_write, do_read;
	ngx_err_t                 err;
	ngx_str_t                *name;
	ngx_uint_t                i;

	c = ev->data;
	s = c->data;

	cscf = ngx_tcp_get_module_srv_conf(s, ngx_tcp_core_module);

	do_write = ev->write ? 1 : 0;
    do_read = !do_write;

	if(do_read) {
		pc = ev->data;
		item = pc->data2;
		pb = item->buffer;
	}

	if(do_read) {
		fprintf(stdout, "peer read\n");
	} else {
		fprintf(stdout, "client write\n");
	}

	c = s->connection;
	b = c->buffer;

	for(;;) {
	    if(do_write){
	        size = b->last - b->pos;

	    	if(size && c->write->ready) {
	    		n = c->send(c, b->pos, size);

	    		err = ngx_socket_errno;
	    		if (n == NGX_ERROR) {
	    			ngx_tcp_finalize_session(s);
	    			return;
	    	    }

	    		if (n > 0) {
	    			b->pos += n;
	    			if (b->pos == b->last) {
	    			    b->pos = b->start;
	    			    b->last = b->start;
	    			}
	    	    }
	    	}

	    	if(!do_read) {
	    		break;
	    	}
	    }

		if(pc->read->ready){
			size = pb->end - pb->last;
			if(size >= item->nexpected) {
				size = item->nexpected;
			} else {
				ngx_log_error(NGX_LOG_ERR, ev->log, 0, "backend's receive buffer "
						    "size is not enough.");
				ngx_tcp_finalize_session(s);

				return;
			}

			if(size <= 0) break;

			n = pc->recv(pc, pb->last, size);

			err = ngx_socket_errno;
			ngx_log_debug1(NGX_LOG_DEBUG_TCP, ev->log, 0,
						   "tcp proxy handler recv:%d", n);

			if (n == NGX_AGAIN || n == 0) {
				break;
			}

			if (n == NGX_ERROR) {
				c->read->eof = 1;
				break;
			}

			if(n > 0) {
				pb->last += n;

				if(n == size) {
					ngx_tcp_update_reading_item(item, 0);
				} else {
					ngx_tcp_update_reading_item(item, n);
				}

				if(item->state == S_READING_END) {
					ngx_memcpy(b->last, pb->pos, n);
					b->last += n;

					pb->pos = pb->start;
					pb->last = pb->start;

					do_write = 1;
					item->state = S_SIZE_READING;
				}

				continue;
			}
		}

		break;
    }

	ngx_log_debug1(NGX_LOG_DEBUG_TCP, ev->log, 0,
	               "tcp proxy dummy read handler: %d", c->fd);

	if(!do_read) {
		if(c->read->timer_set) {
			ngx_del_timer(c->read);
		}

		ngx_add_timer(c->read, cscf->timeout);
	}

    if(do_read) {
    	if(pc->read->eof) {

    		/* backend has colsed connection */

            us = s->upstreams.elts;
            for(i = 0; i < s->upstreams.nelts; ++i) {
            	if(us[i].peer.connection == pc) {
            		us[i].unused = 1;
            	}
            }

            // todo clear upstream info
            // ...


            // todo copy cache
            // ...

    		ngx_log_error(NGX_LOG_DEBUG, c->log, 0, "proxied session done");

    		if(!ngx_tcp_check_has_active_upstream(s)) {
    		    ngx_tcp_finalize_session(s);
    		}
    	}
    }
}

static ngx_uint_t
ngx_tcp_check_has_active_upstream(ngx_tcp_session_t *s) {
	ngx_tcp_upstream_t              *us;
	ngx_uint_t                       i;

	us = s->upstreams.elts;
	for(i = 0; i < s->upstreams.nelts; ++i) {
		if(!us[i].unused) {
			return 1;
		}
	}

	return 0;
}
