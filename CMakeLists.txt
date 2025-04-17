cmake_minimum_required(VERSION 3.10)

# 项目名称

set(PROJECT_NAME HikCamTest)
project(${PROJECT_NAME})

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 查找 OpenCV 包
find_package(OpenCV REQUIRED)

# 头文件路径
include_directories(
    src
    /opt/MVS/include
    ${OpenCV_INCLUDE_DIRS}
)


# 设置库文件路径
set (LIB_PATH
    /usr/local/lib
    /opt/MVS/lib/64
    /opt/MVS/bin
)

# 设置包含路径
set (INCLUDE_PATH
    /usr/local/include/opencv4
    /usr/local/include/eigen3
    /opt/MVS/include
    
)

# 源文件
set(SOURCES
    src/main.cpp
    src/MonitorCam.cpp
    # 其他源文件
)

link_directories(/opt/MVS/lib/64)

# 添加可执行文件
add_executable(${PROJECT_NAME} ${SOURCES} src/MonitorCam.h)

# 添加可执行文件的包含目录
target_include_directories(${PROJECT_NAME} 
    PRIVATE
    ${INCLUDE_PATH}
)

target_include_directories(${PROJECT_NAME} PRIVATE /opt/MVS/include)

target_link_directories(${PROJECT_NAME} 
    PRIVATE
    ${LIB_PATH}
)

# 链接 OpenCV 库
target_link_libraries(${PROJECT_NAME} 
MvCameraControl
${OpenCV_LIBS})

