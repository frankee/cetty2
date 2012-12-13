#
#===============================================================================
#
#         FILE:  sample.t
#
#  DESCRIPTION: test 
#
#        FILES:  ---
#         BUGS:  ---
#        NOTES:  ---
#       AUTHOR:  Weibin Yao (http://yaoweibin.cn/), yaoweibin@gmail.com
#      COMPANY:  
#      VERSION:  1.0
#      CREATED:  03/02/2010 03:18:28 PM
#     REVISION:  ---
#===============================================================================


# vi:filetype=perl

use lib 'lib';
use Test::Nginx::LWP;

plan tests => repeat_each() * 2 * blocks();

#no_diff;

run_tests();

__DATA__

=== TEST 1: the upstream_busyness command
--- config
    upstream test{
        server blog.163.com;
        busyness;
    }

    server {
        listen 1984;

        proxy_pass test;
    }
--- request
GET /
--- response_body_like: ^<(.*)>$

=== TEST 2: the upstream_busyness command with check
--- config
    upstream test{
        server blog.163.com;

        check interval=3000 rise=1 fall=5 timeout=1000;
        busyness;
    }

    server {
        listen 1984;

        proxy_pass test;
    }
--- request
GET /
--- response_body_like: ^<(.*)>$
