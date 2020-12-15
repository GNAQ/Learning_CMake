# CMake 简单学习

----------

CMake 用途：统一化生成各种平台相关的 makefile，方便各种 make 工具使用。

核心是编写 `CMakeLists.txt` 用于控制生成。

然后可以用 `cmake PATH` 命令对 `PATH` 下的 CMakeLists.txt 生成各种 makefile

然后就可以用 make / etc 进行编译

## Demo 1

源码：

```cpp
#include <cstdio>
#include <iostream>
#include <cmath>

int main(int argc, char *argv[])
{
	if (argc < 3)
	{
		printf("Use %s base power to calc!\n", argv[0]);
		return 1;
	}
	
	double base, pw;
	base = atof(argv[1]);
	pw = atof(argv[2]);
	
	printf("%lf ^ %lf = %lf\n", base, pw, pow(base, pw));
	return 0;
}
```

尝试编写 `CMakeLists.txt` ，放于同一目录下：

```plain
set(CMAKE_C_COMPILER "C:/MinGW64/bin/gcc.exe")
set(CMAKE_CXX_COMPILER "C:/MinGW64/bin/g++.exe")
project(demo_1)
add_executable(Demo demo.cpp) 
# 井号表示注释
# add_executable 的作用是把 demo.cpp 生成为 Demo 这个可执行文件。
```

之后可以运行 `cmake -G"MinGW Makefiles"` 来运行 cmake 了。
因为是在 Windows+MinGW+CMake 的配置环境下，所以需要加 Generate flag。（不然默认 NMake）

成功了，不过提示要求添加 CMake minimum version ，更改 `CMakeLists.txt` 如下：

```plain
cmake_minimum_required (VERSION 3.8)

set(CMAKE_C_COMPILER "C:/MinGW64/bin/gcc.exe")
set(CMAKE_CXX_COMPILER "C:/MinGW64/bin/g++.exe")

project(demo_1)

# 查找当前目录下的所有源文件
# 并将名称保存到 SRCS 变量
aux_source_directory(. SRCS)

# 指定生成目标
add_executable(Demo ${SRCS})
```

然后运行一下 `make` 就可以了，显示如下：

```plain
PS D:\Dev> cmake -G"MinGW Makefiles"
CMake Warning:
  No source or binary directory provided.  Both will be assumed to be the
  same as the current working directory, but note that this warning will 
  become a fatal error in future CMake releases.


-- Configuring done
-- Generating done
-- Build files have been written to: D:/Dev
PS D:\Dev> make
[100%] Built target Demo
PS D:\Dev> ./Demo 2 10
2.000000 ^ 10.000000 = 1024.000000
```

## Demo 2 

我们尝试来添加多个源文件，然后使用 CMake。

首先编写好源程序，然后放于同一目录下 `./src`

`pw.hpp`

```cpp
extern double Calc_Power(char *base, char *pw);
```

`pw.cpp`

```cpp
#include <cmath>
#include <cstring>
#include <cstdio>

double Calc_Power(char *base_ch, char *pw_ch)
{
	double base, pw;
	base = atof(base_ch);
	pw = atof(pw_ch);
	return pow(base, pw);
}
```

`main.cpp`

```cpp
#include <cstdio>
#include <iostream>
#include <cmath>
#include "pw.hpp"

int main(int argc, char *argv[])
{
	if (argc < 3)
	{
		printf("Use %s base power to calc!\n", argv[0]);
		return 1;
	}
	
	printf("%s ^ %s = %lf\n", argv[1], argv[2], Calc_Power(argv[1], argv[2]));
	return 0;
}
```

这样只需要稍微改动一下 `CMakeLists.txt` 就可以了。

```plain
cmake_minimum_required (VERSION 3.8)

set(CMAKE_C_COMPILER "C:/MinGW64/bin/gcc.exe")
set(CMAKE_CXX_COMPILER "C:/MinGW64/bin/g++.exe")

project(demo_1)

# 查找当前目录下的所有源文件
# 并将名称保存到 SRCS 变量
aux_source_directory(./src SRCS)

# 指定生成目标
add_executable(Demo ${SRCS})
```

然后执行命令，大成功。

```plain
PS D:\Dev> cmake .
-- Configuring done
-- Generating done
-- Build files have been written to: D:/Dev
PS D:\Dev> make
Scanning dependencies of target Demo
[ 33%] Building CXX object CMakeFiles/Demo.dir/src/main.cpp.obj
[ 66%] Building CXX object CMakeFiles/Demo.dir/src/pw.cpp.obj
[100%] Linking CXX executable Demo.exe
[100%] Built target Demo
```

## Demo 3

添加一些子目录 + 链接静态库

比如我可以把 `pw.cpp/hpp` 放置在单独的子文件夹中（ `CMake` = `cargo` 实锤）（不是）

建立一个 `pw` 文件夹，再于 `pw` 文件夹中编写一个单独的 `CMakeLists.txt`。

```plain
aux_source_directory(. PW_LIB_DIR)

# 创建一个静态库。
add_library(My_Power ${PW_LIB_DIR})
```

更新起初的 `CMakeLists.txt`

```plain
cmake_minimum_required (VERSION 3.8)

set(CMAKE_C_COMPILER "C:/MinGW64/bin/gcc.exe")
set(CMAKE_CXX_COMPILER "C:/MinGW64/bin/g++.exe")

project(demo_1)

# 查找当前目录下的所有源文件
# 并将名称保存到 SRCS 变量
aux_source_directory(./src SRCS)

# 添加一个子目录
add_subdirectory(pw)

# 指定生成目标
add_executable(Demo ${SRCS})

# 链接上生成的库
target_link_libraries(Demo My_Power)
```

## Demo 4

来用 CMake 控制编译选项试试？

将 My_Power 库设为一个可选的库，如果该选项为 ON ，就使用该库定义的数学函数来进行运算。否则就调用标准库中的数学函数库。

（虽然写的 pw.cpp 也是调了标准库罢了（摊手））

先改好代码：

`main.cpp`

```cpp
#include <cstdio>
#include <iostream>
#include "config.h"

#ifdef USE_MYMATH
	#include "pw/pw.hpp"
#else 
	#include <cmath>
#endif

int main(int argc, char *argv[])
{
	if (argc < 3)
	{
		printf("Use %s base power to calc!\n", argv[0]);
		return 1;
	}
	
	printf("%s ^ %s = %lf\n", argv[1], argv[2], Calc_Power(argv[1], argv[2]));
	return 0;
}
```

`pw.cpp`

```cpp
#include <cmath>
#include <cstring>
#include <cstdio>

double Calc_Power(char *base_ch, char *pw_ch)
{
	puts("Using my own power calc function now");
	
	double base, pw;
	base = atof(base_ch);
	pw = atof(pw_ch);
	return pow(base, pw);
}
```

然后改一下 `CMakeLists.txt` 就差不多了

```plain
cmake_minimum_required (VERSION 3.8)

set(CMAKE_C_COMPILER "C:/MinGW64/bin/gcc.exe")
set(CMAKE_CXX_COMPILER "C:/MinGW64/bin/g++.exe")

project(demo_1)

set(PROJECT_SOURCE_DIR "./src")
set(PROJECT_CONFIG_DIR "./config")

# 添加一个 USE_MYMATH 选项。可以指定默认为 ON ，见 CMake 文档
# 这里调换默认值记得删除 CMakeCache.txt ！！！！
option(USE_MYMATH
       "Use my own math implementation" OFF)

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

message(${USE_MYMATH})

# 是否加入 pw 库
if (USE_MYMATH)
  include_directories("${PROJECT_SOURCE_DIR}/pw")
  add_subdirectory(${PROJECT_SOURCE_DIR}/pw)  
  set(EXTRA_LIBS ${EXTRA_LIBS} My_Power) 
		# 实际上是拼接原有内容和 "My_Power" 字符串。
endif (USE_MYMATH)

# 指定生成目标
add_executable(Demo ${SRCS})

# 链接上生成的库
target_link_libraries(Demo ${EXTRA_LIBS})
```

记得写一个 `config.h.in` 文件（怎么写这么多文件……），单独开个 `config` 目录存算了。

内容这样写：

```cpp
#cmakedefine USE_MYMATH
```

这样就都完成了。现在的目录看起来是这样：

```plain
./
	/config
		config.h.in
	/src
		/pw
			CMakeLists.txt
			pw.cpp
			pw.hpp
		main.cpp
	CMakeLists.txt
```

然后开始跑。 `cmake ./ -G"MinGW Makefiles"` + `make` 就 OK 了。现在输出看起来像：

```plain
PS D:\Dev> cmake ./ -G"MinGW Makefiles"
OFF
-- Configuring done
-- Generating done
-- Build files have been written to: D:/Dev
PS D:\Dev> make
Scanning dependencies of target Demo
[100%] Built target Demo
PS D:\Dev> ./demo 2 10
2 ^ 10 = 1024.000000
```

和这样：

```plain
PS D:\Dev> cmake ./ -D USE_MYMATH=ON
ON
-- Configuring done
-- Generating done
-- Build files have been written to: D:/Dev
PS D:\Dev> make
Scanning dependencies of target My_Power
[ 25%] Building CXX object src/pw/CMakeFiles/My_Power.dir/pw.cpp.obj
[ 50%] Linking CXX static library libMy_Power.a
[ 50%] Built target My_Power
Scanning dependencies of target Demo
[ 75%] Building CXX object CMakeFiles/Demo.dir/src/main.cpp.obj
[100%] Linking CXX executable Demo.exe
[100%] Built target Demo
PS D:\Dev> ./demo 300 2
Using my own power calc function now
300 ^ 2 = 90000.000000
```

其实那个 cmake-gui (On Windows) / ccmake 挺好用的，可以试一下。

## Demo 5

尝试对项目添加测试，来判断程序是否正确执行。
这里只需要更改 `CMakeLists.txt` 就可以了。
（其实是 CTest 在干活。）

结尾加入几行就可以了。其中 `PASS_REGULAR_EXPRESSION` 启用正则表达式。

```plain
enable_testing()

add_test(test_whatevername demo_1 201 2)

set_tests_properties(test_whatevername PROPERTIES PASS_REGULAR_EXPRESSION "40401")
```

输出如下：

```plain
PS D:\Dev> cmake . -G"MinGW Makefiles"
OFF
-- Configuring done
-- Generating done
-- Build files have been written to: D:/Dev
PS D:\Dev> make test
Running tests...
Test project D:/Dev
    Start 1: test_whatevername
1/1 Test #1: test_whatevername ................   Passed    0.01 sec

100% tests passed, 0 tests failed out of 1

Total Test time (real) =   0.02 sec
```

## Demo 6

最后就是一些环境检查的东西。比如构建的时候如果编译器的标准库自带 `pow()` 函数，我们就使用标准库函数，否则使用自己的函数。

源码啥的改一改，然后直接把 `CMakeLists.txt` 里的 `options` 魔改过去：

```cpp
#include <cstdio>
#include <iostream>
#include "config.h"

#ifdef USE_STDMATH
	#include <cmath>
#else 
	#include "pw/pw.hpp"
#endif

int main(int argc, char *argv[])
{
	if (argc < 3)
	{
		printf("Use %s base power to calc!\n", argv[0]);
		return 1;
	}
	
	#ifdef USE_STDMATH
		printf("%s ^ %s = %lf\n", argv[1], argv[2], pow(atof(argv[1]), atof(argv[2])));
	#else 
		printf("%s ^ %s = %lf\n", argv[1], argv[2], Calc_Power(argv[1], argv[2]));
	#endif
	
	return 0;
}
```

```plain
cmake_minimum_required (VERSION 3.8)

set(CMAKE_C_COMPILER "C:/MinGW64/bin/gcc.exe")
set(CMAKE_CXX_COMPILER "C:/MinGW64/bin/g++.exe")

project(demo_1)

set(PROJECT_SOURCE_DIR "./src")
set(PROJECT_CONFIG_DIR "./config")

# 用这个 CMake 自带的检查函数宏。
include(${CMAKE_ROOT}/Modules/CheckFunctionExists.cmake)

# 加入一个配置头文件 config.h ，这个文件由 CMake 从 config.h.in 生成。
# 通过这样的机制，将可以通过预定义一些参数和变量来控制代码的生成。
configure_file (
  "${PROJECT_CONFIG_DIR}/config.h.in"
  "${PROJECT_SOURCE_DIR}/config.h"
)

check_function_exists(pow USE_STDMATH)

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
```

记得改 `config.h.in`

```plain
#cmakedefine USE_STDMATH
```

然后就做完了。来试试：

```plain
PS D:\Dev> cmake . -G"MinGW Makefiles"
-- The C compiler identification is GNU 8.1.0
-- The CXX compiler identification is GNU 8.1.0
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Check for working C compiler: C:/MinGW64/bin/gcc.exe - skipped
-- Detecting C compile features
-- Detecting C compile features - done
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Check for working CXX compiler: C:/MinGW64/bin/g++.exe - skipped
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- Looking for pow
-- Looking for pow - found
Result:1
-- Configuring done
-- Generating done 
-- Build files have been written to: D:/Dev
PS D:\Dev> make
Scanning dependencies of target Demo
[100%] Built target Demo
PS D:\Dev> ./demo 2 2
2 ^ 2 = 4.000000
```

OK.

## Ending

那么这就是 CMake 的基本接触啦。其余的部分也可以通过文档解决，就这样吧。

dadada~