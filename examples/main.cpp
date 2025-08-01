#include "NMEAParser.h"
#include <iostream>

int main() {
    NmeaParser::NMEAParser parser;
    
    // 使用一个带有正确校验和的NMEA消息
    // $GPRMC,123519,A,4808.54,N,01131.42,E,022.4,084.4,230394,003.1,W*6A
    // 校验和计算：从$后到*前的所有字符进行XOR运算
    std::string nmeaMessage = "$GNRMC,041704.000,A,2935.21718,N,10631.58906,E,0.00,172.39,071124,,,A*7E";
    
    std::cout << "解析NMEA消息: " << nmeaMessage << std::endl;
    
    auto result = parser.parseNMEAMessage(nmeaMessage);
    
    if (result && result->rmc) {
        std::cout << "解析成功！" << std::endl;
        std::cout << "纬度: " << result->rmc->latitude << std::endl;
        std::cout << "经度: " << result->rmc->longitude << std::endl;
        std::cout << "速度: " << result->rmc->speed << " 节" << std::endl;
        std::cout << "状态: " << result->rmc->status << std::endl;
    } else {
        std::cout << "解析失败！" << std::endl;
    }
    
    return 0;
} 