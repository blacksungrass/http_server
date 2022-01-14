# http_server
simple toy http server 


##todo list
- todo mmap 返回文件资源
- todo 对mmap进一步包装缓存，减少磁盘IO
- todo 测试加不加超时timer的影响
- todo 调试目前性能表现较差的原因


  Benchmarking: GET http://localhost:23333/
  20000 clients, running 5 sec.

Speed=206460 pages/min, 385821 bytes/sec.
Requests: 16458 susceed, 747 failed.

