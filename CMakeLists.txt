cmake_minimum_required (VERSION 3.8)

set(CMAKE_C_COMPILER "C:/MinGW64/bin/gcc.exe")
set(CMAKE_CXX_COMPILER "C:/MinGW64/bin/g++.exe")

project(demo_1)

set(PROJECT_SOURCE_DIR "./src")
set(PROJECT_CONFIG_DIR "./config")

# 用这个 CMake 自带的检查函数宏。
include(${CMAKE_ROOT}/Modules/CheckFunctionExists.cmake)
check_function_exists(pow USE_STDMATH)

# 加入一个配置头文件 config.h ，这个文件由 CMake 从 config.h.in 生成。
# 通过这样的机制，将可以通过预定义一些参数和变量来控制代码的生成。
# 顺序执行，在生成配置之前请确保变量已被赋值正确。
configure_file (
  "${PROJECT_CONFIG_DIR}/config.h.in"
  "${PROJECT_SOURCE_DIR}/config.h"
  )
  
# 查找当前目录下的所有源文件
# 并将名称保存到 SRCDIR 变量
aux_source_directory(./src SRCS)

message(Result: ${USE_STDMATH})

# 是否加入 pw 库
if (NOT USE_STDMATH)
  include_directories("${PROJECT_SOURCE_DIR}/pw")
  add_subdirectory(${PROJECT_SOURCE_DIR}/pw)  
  set(EXTRA_LIBS ${EXTRA_LIBS} My_Power) 
		# 实际上是拼接原有内容和 "My_Power" 字符串。
endif (NOT USE_STDMATH)

# 指定生成目标
add_executable(Demo ${SRCS})

# 链接上生成的库
target_link_libraries(Demo ${EXTRA_LIBS})