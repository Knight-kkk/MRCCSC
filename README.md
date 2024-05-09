                                 ___  ___   _____    _____   _____   _____   _____  
                                /   |/   | |  _  \  /  ___| /  ___| /  ___/ /  ___| 
                               / /|   /| | | |_| |  | |     | |     | |___  | |     
                              / / |__/ | | |  _  /  | |     | |     \___  \ | |     
                             / /       | | | | \ \  | |___  | |___   ___| | | |___  
                            /_/        |_| |_|  \_\ \_____| \_____| /_____/ \_____| 
MRCCSC: A **M**uduo and **R**edis-based **C**hat **C**luster **S**erver and **C**lient.

## :memo:Overview 
MRCCSC is a cluster chat system, it employs Muduo as network communication component, employs JSON to organize message,  employs MySQL as DBMS, employs Nginx for load balancing and employs Redis as MQ component.

## :hammer: Environment Configuration & Build
  As mentioned above, MRCCSC is based on Muduo, JSON, MySQL, Nginx and Redis. If you are interested in MRCCSC and try to build and run MRCCSC, you can follow these tutorials to config your basic environment:
    - [how to use muduo in your project?](https://github.com/chenshuo/muduo-tutorial)
    - [how to use redis in your project?](https://github.com/redis/hiredis.git)
    - [how to use nginx in your project?](https://nginx.org/en/docs/)

  📌*REMARK*:
    1. Since nginx does not support the TCP protocol by default, you should use `./configure --with-stream` to build a TCP-supported version.
    2. The usage of MySQL and JSON has been encapsulated within this project, and will be introduced in subsequent section.

  Once you finished these basic environment configuration, you can build MRCCSC next.
  ```shell
  git clone https://github.com/Knight-kkk/MRCCSC.git
  cd build
  ./autobuild.sh
  ```
  If you build this project successfully, you will see:
  ```shell
  [ 66%] Built target ChatServer
  [ 80%] Built target ChatClient
  [100%] Built target redis
  ```

## 💻 Usage
- To enable nginx to effectively perform its load balancing function, you need to edit nginx configuration file and add the servers that require proxying.
  ```shell
  vim /path/to/nginx/conf/nginx.conf
  ```
  Add your server info in the `stream` section:
  ```
  stream {
      upstream MyServer {
          server ServerIP:Port weight=1 max_fails=3 fail_timeout=30s;
          # ...
          server ServerIP:Port weight=1 max_fails=3 fail_timeout=30s;
      }
      
      server {
          proxy_connect_timeout 1s;
          proxy_timeout 3000s;
          listen 8000;
          proxy_pass MyServer;
          tcp_nodelay on;
      }
  }
  ```
  Then, run nginx with `nginx -c /path/to/nginx/conf/nginx.conf`.

- Run RMCCSC server(s) with `./bin/ChatServer [IP] [Port]`.
- Run Redis with `redis-server`.
- Run RMCCSC client(s) with `./bin/ChatClient [NginxServerIP] 8000`, 8000 is Nginx's default port of load balancing.
- The usage of client will be shown once you run it.

## :art:Features
MRCCSC supports these fucntions below:
