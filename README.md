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

  3. Before building, you should make sure that `g++,gcc,cmake` are configured.

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
- create tables in MySQL server, exact mysql script is `example/create.sql`, and the configuration of MySQL server is in `include/server/db/db.hpp`.
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

- Run MRCCSC server(s) with `./bin/ChatServer [IP] [Port]`.
- Run Redis with `redis-server`.
- Run MRCCSC client(s) with `./bin/ChatClient [NginxServerIP] 8000`, 8000 is Nginx's default port of load balancing.
- The usage of client will be shown once you run it.

## :art:Features
  - MRCCSC employs the Muduo network library as the foundational network I/O component, decoupling the business layer from the network layer.
  - MRCCSC employs Nginx as a proxy server to implement load balancing, ensuring high availability and disaster recovery for the server cluster.
  - MRCCSC employs the publish-subscribe mechanism of Redis to enable message sharing and dissemination across server clusters, avoiding direct tight coupling among servers and thereby enhancing the scalability of the cluster.

## 📁Project Organization
.
├── bin
│   ├── ChatClient
│   ├── ChatServer
│   └── redis
├── build
│   ├── autobuild.sh
│   ├── CMakeCache.txt
│   ├── CMakeFiles
│   │   ├── 3.14.5
│   │   │   ├── CMakeCCompiler.cmake
│   │   │   ├── CMakeCXXCompiler.cmake
│   │   │   ├── CMakeDetermineCompilerABI_C.bin
│   │   │   ├── CMakeDetermineCompilerABI_CXX.bin
│   │   │   ├── CMakeSystem.cmake
│   │   │   ├── CompilerIdC
│   │   │   │   ├── a.out
│   │   │   │   ├── CMakeCCompilerId.c
│   │   │   │   └── tmp
│   │   │   └── CompilerIdCXX
│   │   │       ├── a.out
│   │   │       ├── CMakeCXXCompilerId.cpp
│   │   │       └── tmp
│   │   ├── cmake.check_cache
│   │   ├── CMakeDirectoryInformation.cmake
│   │   ├── CMakeOutput.log
│   │   ├── CMakeTmp
│   │   ├── feature_tests.bin
│   │   ├── feature_tests.c
│   │   ├── feature_tests.cxx
│   │   ├── Makefile2
│   │   ├── Makefile.cmake
│   │   ├── progress.marks
│   │   └── TargetDirectories.txt
│   ├── cmake_install.cmake
│   ├── compile_commands.json
│   ├── examples
│   │   ├── CMakeFiles
│   │   │   ├── CMakeDirectoryInformation.cmake
│   │   │   ├── progress.marks
│   │   │   └── redis.dir
│   │   │       ├── __
│   │   │       │   └── src
│   │   │       │       └── server
│   │   │       │           └── redis
│   │   │       ├── build.make
│   │   │       ├── cmake_clean.cmake
│   │   │       ├── DependInfo.cmake
│   │   │       ├── depend.make
│   │   │       ├── flags.make
│   │   │       ├── link.txt
│   │   │       └── progress.make
│   │   ├── cmake_install.cmake
│   │   └── Makefile
│   ├── Makefile
│   └── src
│       ├── client
│       │   ├── CMakeFiles
│       │   │   ├── ChatClient.dir
│       │   │   │   ├── build.make
│       │   │   │   ├── cmake_clean.cmake
│       │   │   │   ├── DependInfo.cmake
│       │   │   │   ├── depend.make
│       │   │   │   ├── flags.make
│       │   │   │   ├── link.txt
│       │   │   │   └── progress.make
│       │   │   ├── CMakeDirectoryInformation.cmake
│       │   │   └── progress.marks
│       │   ├── cmake_install.cmake
│       │   └── Makefile
│       ├── CMakeFiles
│       │   ├── CMakeDirectoryInformation.cmake
│       │   └── progress.marks
│       ├── cmake_install.cmake
│       ├── Makefile
│       └── server
│           ├── CMakeFiles
│           │   ├── ChatServer.dir
│           │   │   ├── build.make
│           │   │   ├── cmake_clean.cmake
│           │   │   ├── db
│           │   │   ├── DependInfo.cmake
│           │   │   ├── depend.make
│           │   │   ├── flags.make
│           │   │   ├── link.txt
│           │   │   ├── model
│           │   │   ├── progress.make
│           │   │   └── redis
│           │   ├── CMakeDirectoryInformation.cmake
│           │   └── progress.marks
│           ├── cmake_install.cmake
│           └── Makefile
├── CMakeCache.txt
├── CMakeFiles
│   ├── 3.14.5
│   │   ├── CMakeCCompiler.cmake
│   │   ├── CMakeCXXCompiler.cmake
│   │   ├── CMakeDetermineCompilerABI_C.bin
│   │   ├── CMakeDetermineCompilerABI_CXX.bin
│   │   ├── CMakeSystem.cmake
│   │   ├── CompilerIdC
│   │   │   ├── a.out
│   │   │   ├── CMakeCCompilerId.c
│   │   │   └── tmp
│   │   └── CompilerIdCXX
│   │       ├── a.out
│   │       ├── CMakeCXXCompilerId.cpp
│   │       └── tmp
│   ├── cmake.check_cache
│   ├── CMakeDirectoryInformation.cmake
│   ├── CMakeOutput.log
│   ├── CMakeTmp
│   ├── feature_tests.bin
│   ├── feature_tests.c
│   ├── feature_tests.cxx
│   ├── Makefile2
│   ├── Makefile.cmake
│   ├── progress.marks
│   └── TargetDirectories.txt
├── cmake_install.cmake
├── CMakeLists.txt
├── dump.rdb
├── examples
│   ├── 1-json
│   ├── 1-json.cpp
│   ├── 2-serialization
│   ├── 2-serialization.cpp
│   ├── 3-deserialization
│   ├── 3-deserialization.cpp
│   ├── 4-muduoServer
│   ├── 4-muduoServer.cpp
│   ├── 5-redis.cpp
│   ├── CMakeFiles
│   │   ├── CMakeDirectoryInformation.cmake
│   │   ├── progress.marks
│   │   └── redis.dir
│   │       ├── __
│   │       │   └── src
│   │       │       └── server
│   │       │           └── redis
│   │       │               └── redis.cpp.o
│   │       ├── 5-redis.cpp.o
│   │       ├── build.make
│   │       ├── cmake_clean.cmake
│   │       ├── CXX.includecache
│   │       ├── DependInfo.cmake
│   │       ├── depend.internal
│   │       ├── depend.make
│   │       ├── flags.make
│   │       ├── link.txt
│   │       └── progress.make
│   ├── cmake_install.cmake
│   ├── CMakeLists.txt
│   ├── create.sql
│   └── Makefile
├── include
│   ├── client
│   ├── common
│   │   ├── json.hpp
│   │   └── public.hpp
│   └── server
│       ├── db
│       │   └── db.hpp
│       ├── model
│       │   ├── friendmodel.hpp
│       │   ├── group.hpp
│       │   ├── groupmodel.hpp
│       │   ├── groupuser.hpp
│       │   ├── offlinemessagemodel.hpp
│       │   ├── user.hpp
│       │   └── usermodel.hpp
│       ├── redis
│       │   └── redis.hpp
│       ├── server.hpp
│       └── service.hpp
├── Makefile
├── README.md
└── src
    ├── client
    │   ├── CMakeFiles
    │   │   ├── ChatClient.dir
    │   │   │   ├── build.make
    │   │   │   ├── cmake_clean.cmake
    │   │   │   ├── CXX.includecache
    │   │   │   ├── DependInfo.cmake
    │   │   │   ├── depend.internal
    │   │   │   ├── depend.make
    │   │   │   ├── flags.make
    │   │   │   ├── link.txt
    │   │   │   ├── main.cpp.o
    │   │   │   └── progress.make
    │   │   ├── CMakeDirectoryInformation.cmake
    │   │   └── progress.marks
    │   ├── cmake_install.cmake
    │   ├── CMakeLists.txt
    │   ├── main.cpp
    │   └── Makefile
    ├── CMakeFiles
    │   ├── CMakeDirectoryInformation.cmake
    │   └── progress.marks
    ├── cmake_install.cmake
    ├── CMakeLists.txt
    ├── Makefile
    └── server
        ├── CMakeFiles
        │   ├── ChatServer.dir
        │   │   ├── build.make
        │   │   ├── cmake_clean.cmake
        │   │   ├── CXX.includecache
        │   │   ├── db
        │   │   │   └── db.cpp.o
        │   │   ├── DependInfo.cmake
        │   │   ├── depend.internal
        │   │   ├── depend.make
        │   │   ├── flags.make
        │   │   ├── link.txt
        │   │   ├── main.cpp.o
        │   │   ├── model
        │   │   │   ├── friendmodel.cpp.o
        │   │   │   ├── groupmodel.cpp.o
        │   │   │   └── usermodel.cpp.o
        │   │   ├── offlinemessagemodel.cpp.o
        │   │   ├── progress.make
        │   │   ├── redis
        │   │   │   └── redis.cpp.o
        │   │   ├── server.cpp.o
        │   │   └── service.cpp.o
        │   ├── CMakeDirectoryInformation.cmake
        │   └── progress.marks
        ├── cmake_install.cmake
        ├── CMakeLists.txt
        ├── db
        │   └── db.cpp
        ├── main.cpp
        ├── Makefile
        ├── model
        │   ├── friendmodel.cpp
        │   ├── groupmodel.cpp
        │   └── usermodel.cpp
        ├── offlinemessagemodel.cpp
        ├── README
        ├── redis
        │   └── redis.cpp
        ├── server.cpp
        └── service.cpp
