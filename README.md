

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