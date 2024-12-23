#ifndef NMEAPARSER_H
#define NMEAPARSER_H

#include <memory>
#include <string>
#include <optional>
#include <vector>
#include <ctime>

namespace NmeaParser{

enum class LocationMode {
    GPS = 1,
    BD,
    GN,
};

struct RMCData {
    LocationMode locationMode;
    time_t utcTime;     // UTC时间
    char status;            // 定位状态 A=有效，V=无效
    double latitude;        // 纬度
    char latHemisphere;     // 北纬N或南纬S
    double longitude;       // 经度
    char lonHemisphere;     // 东经E或西经W
    double speed;           // 速度（节）
    double course;          // 航向（度）
    std::string date;       // 日期
    double variation;       // 磁偏角
    char variationDirection; // 磁偏角方向 E或W
    char mode;              // 模式 A=自动，D=差分，E=估测，N=数据无效
};

struct GGAData {
    LocationMode locationMode;
    time_t utcTime;         // UTC时间，格式为HHMMSS.SSS
    double latitude;             // 纬度
    char latHemisphere;          // 纬度半球，'N'=北纬，'S'=南纬
    double longitude;            // 经度
    char lonHemisphere;          // 经度半球，'E'=东经，'W'=西经
    char status;                 // 定位状态，'0'=无定位，'1'=定位有效
    int satellites;              // 正在使用的卫星数量
    double hdop;                 // 水平精度因子
    double altitude;             // 海拔高度
    double WGaltitude;           // 相对大地水准面，'M'=米
    int ageDifferential;         // 差分数据的年龄
    std::string stationID;        // 差分站ID
};

struct GSAData {
    std::string rawMessage;
    // 其他GSA数据字段
};

// 定义卫星信息结构体
struct SatelliteInfo {
    int satelliteID;         // 卫星编号
    double elevation;        // 仰角（度）
    double azimuth;          // 方位角（度）
    double signalToNoiseRatio; // 信噪比（dB）
};

// 定义GSV数据结构体
struct GSVData {
    LocationMode locationMode;
    int totalMessages;       // 总消息数
    int messageNumber;       // 当前消息编号
    int satelliteCount;      // 可见卫星总数
    std::vector<SatelliteInfo> satellites; // 卫星信息列表
};

struct VTGData {
    std::string rawMessage;
    // 其他VTG数据字段
};

class NMEAParser {
public:
    struct NMEAData {
        std::string rawMessage;
        std::optional<RMCData> rmc;
        std::optional<GGAData> gga;
        std::optional<GSAData> gsa;
        std::optional<GSVData> gsv;
        std::optional<VTGData> vtg;
    };

    NMEAParser();
    ~NMEAParser();

    std::optional<NMEAData> parseNMEAMessage(const std::string& nmeaMessage);

    virtual void dumpLocationInfo(std::optional<NMEAData> &op);

    virtual void saveLocationInfo(std::optional<NMEAData> &op, const std::string& fp = "./output.txt");

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

}

#endif // NMEAPARSER_H
