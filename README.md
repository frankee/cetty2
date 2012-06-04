The Cetty Project  - Network Application Framework for c++
===============================================================================

Brief Introduce
---------------

The Cetty project is a network application framework for easy developmenting 
high performance and high scalability servers and clients.

It comes from [Netty](http://www.jboss.org/netty) project, frankly speaking, 
it is a translation of the Netty from java to c++. However, the difference 
between the Cetty and the Netty, is based on boost asio, using proactor 
network pattern.

Dependence
----------
1. [boost](www.boost.org)(required) 
   specially only need asio, bind, system, date_time, thread, any, smart_ptr,
   crc and string_algo. And need compile system, data_time, thread and regex
   libraries, others are headers only.
2. [google-perftools](code.google.com/p/google-perftools)(optional)
   using the tcmalloc to improve the memory operations.
3. [gtest](code.google.com/p/googletest)(optional)
   for unit test.
4. [protobuffer](http://code.google.com/p/protobuf)(optional)
   for protobuffer handle (currently have not integrated yet)

   
Questions?
--------------
If you have any questions, please feel free to send mail to me 
(frankee.zhou at gmail).


Acknowledgment
------------------
Thanks to [Trustin Lee](http://gleamynode.net/),
the Netty framework is like an art and so amazing.
Then thanks to my wife vici and my lovely daughter for their support.
