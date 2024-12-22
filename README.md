# NMEAParse
NMEAParse旨在C++应用中解析NMEA原始报文相关业务，方便地使用对象得出解析的定位信息。本项目需要根据指引编译成动态库，在用户的程序中只需要包含头文件并在编译时连接该动态库即可。

## 功能

该项目的核心功能是解析`NMEA`格式的消息。`NMEAParser` 类可以解析输入的 `NMEA` 消息并验证其有效性。它支持常见的 `NMEA` 格式，能够处理消息校验和并提取相关数据。

## 主要组件

* `src/NMEAParser.cpp`和`include/NMEAParser.h`：这是 `NMEA` 消息解析器的实现部分，包含了消息解析的核心逻辑。
* `build.sh`：这是一个自动化编译脚本，自动配置和编译项目，便于开发者快速构建和运行测试。

## 构建和安装

1. 安装依赖项
本项目依赖 `CMake`。如果你没有安装这些工具，按照以下步骤进行安装：

* 安装 `CMake`
Ubuntu：
```
sudo apt-get install cmake
```

2. 构建项目
项目使用 `CMake` 进行构建。按照以下步骤来构建项目：

克隆该项目：
```
git clone https://github.com/yourusername/nmea-parser.git
```
运行自动化构建脚本：
```
cd nmea-parser
./build.sh
```
该脚本会：
* 创建一个 build 目录，生成编译产物。
* `CMake` 构建系统，该NMEA解析器会生成动态库和头文件。
* 如果不指定`MY_INSTALL_PREFIX`作为安装动态库和头文件的路径，则默认把动态库和头文件安装到系统路径`/usr/lib`和`usr/include`下。可用过`export MY_INSTALL_PREFIX=`加上本地文件路径来指定动态库和头文件的安装路径。

## 使用示例

编写示例程序
```cpp
#include <iostream>
#include <NMEAParser.h>

int main() {
    std::string rmcNmea = "$GNRMC,041704.000,A,2935.21718,N,10631.58906,E,0.00,172.39,071124,,,A*7E";
    std::string gsvNmea= "$GPGSV,4,1,13,02,09,305,24,10,72,161,34,12,19,048,32,21,15,288,18*7A";

    NmeaParser::NMEAParser parse;
    
    auto data = parse.parseNMEAMessage(rmcNmea);
    parse.dumpLocationInfo(data);

    data = parse.parseNMEAMessage(gsvNmea);
    parse.dumpLocationInfo(data);

    return 0;
}
```

编译并运行：
```
g++ main.cpp -std=c++17 -lNMEAParser -g
./a.out
```

运行结果：
```
locationMode: 3
utcTime: 946671424
latitude: 32.9697
latHemisphere: N
longitude: 116.128
lonHemisphere: E
speed: 0
course: 172.39
date: 071124
variation: 0
variationDirection: E
mode: A
totalMessages: 4
messageNumber: 1
satelliteCount: 13
satelliteID: 13
        elevation: 2
        azimuth: 9
        signalToNoiseRatio: 
satelliteID: 24
        elevation: 10
        azimuth: 72
        signalToNoiseRatio: 
satelliteID: 34
        elevation: 12
        azimuth: 19
        signalToNoiseRatio: 
satelliteID: 32
        elevation: 21
        azimuth: 15
        signalToNoiseRatio: 
satelliteID: 18
        elevation: 21
        azimuth: 15
        signalToNoiseRatio: 
```

1. 需要包含头文件`NMEAParser.h`
2. 编译时需要链接`libNMEAParser.so`，如果有指定动态库和头文件路径需要在你的`CMake`或编译指令中指定路径
3. 如果你想使用`dumpLocationInfo`进行定制化打印，只需要继承`NMEAParser`并重写`dumpLocationInfo`即可
```cpp title="override dumpLocationInfo"
#include <iostream>
#include <optional>
#include <memory>
#include <NMEAParser.h>

class MyNMEALocationInfoPrint : public NmeaParser::NMEAParser {
public:
    void dumpLocationInfo(std::optional<NMEAData>& data) {
        std::cout << "this is my own print." << std::endl;
        if (data->rmc) {
            std::cout << data->rmc->latitude << std::endl;
        }
    }
};

int main() {
    std::string rmcNmea = "$GNRMC,041704.000,A,2935.21718,N,10631.58906,E,0.00,172.39,071124,,,A*7E";
    std::string gsvNmea= "$GPGSV,4,1,13,02,09,305,24,10,72,161,34,12,19,048,32,21,15,288,18*7A";

    NmeaParser::NMEAParser parse;
    
    auto data = parse.parseNMEAMessage(rmcNmea);
    parse.dumpLocationInfo(data);

    // data = parse.parseNMEAMessage(gsvNmea);
    // parse.dumpLocationInfo(data);

    std::cout << "---------------------" << std::endl;

    std::unique_ptr<NmeaParser::NMEAParser> np = std::make_unique<MyNMEALocationInfoPrint>();
    np->dumpLocationInfo(data);

    return 0;
}
```
编译运行如下：
```shell
locationMode: 3
utcTime: 946671424
latitude: 32.9697
latHemisphere: N
longitude: 116.128
lonHemisphere: E
speed: 0
course: 172.39
date: 071124
variation: 0
variationDirection: E
mode: A
---------------------
this is my own print.
32.9697
```
**注意：在使用诸如`data->rmc`时应先判断是否为空！！！**
