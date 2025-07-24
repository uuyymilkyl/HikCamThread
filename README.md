# HikCamThread

HikCamThread - 基于海康工业相机的多线程图像采集demo 

---


HikCamThread/
├── .git/                    # Git 配置目录（若使用版本控制）
├── src/                     # 源代码目录
│   ├── main.cpp             # 主函数，程序入口
│   ├── MonitorCam.cpp       # 摄像头监控与处理逻辑实现
│   └── MonitorCam.h         # 摄像头处理的类声明
├── CMakeLists.txt           # CMake 构建脚本
├── Makefile                 # 备用 Make 构建脚本
└── README.md                # 项目说明文件（本文件）
```

---

- **main.cpp**：程序入口，初始化设备，调用 MonitorCam 类。
- **MonitorCam.h / MonitorCam.cpp**：封装了摄像头初始化、数据采集、图像处理与线程管理等逻辑。
- **CMakeLists.txt**：用于 CMake 构建系统的配置文件。

---


## 1. Install MVS SDK and OpenCV Library

Download the **MVS Runtime SDK** from the official HikRobot website:

🔗 [HikRobot MVS SDK Download Page](https://www.hikrobotics.com/en/machinevision/service/download/?module=0)

Make sure to select the **Runtime Package** suitable for your system.

After installation, verify the path:

```bash
ls /opt/MVS
```
## 2. Install C++11+ and CMake

```
sudo apt update
sudo apt install cmake

sudo apt install build-essential
```

## Build
```bash 

cd your_project
mkdir build
cd build
cmake ..
make
```

## Run the Executable
``` bash
./your_executable_name
```

## Custom Development

1. Move the src files MonitorCam.h and MonitorCam.cpp into your project,
2. Include them in your project's CMakeLists.txt, along with the path to the Hikvision camera API (refer to the CMakeLists.txt of this project as a reference).
3. In the code where you want to use the camera, include the head file MonitorCam.h, then use the function.
