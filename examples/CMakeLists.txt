aux_source_directory(../src/server/redis/ REDIS_LIST)
# 指定可执行文件及其源文件
add_executable(redis 5-redis.cpp  ${REDIS_LIST})
# 指定依赖库文件
target_link_libraries(redis hiredis pthread)