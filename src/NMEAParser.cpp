#include "NMEAParser.h"
#include <iostream>
#include <sstream>
#include <iomanip>

namespace NmeaParser{

#define CATCH_AND_HANDLE_EXCEPTIONS(code) \
    do { \
        try { \
            code \
        } catch (const std::invalid_argument& e) { \
            std::cerr << "Invalid argument: " << e.what() << std::endl; \
            /*TODO*/ \
        } catch (const std::out_of_range& e) { \
            std::cerr << "Out of range: " << e.what() << std::endl; \
            /*TODO*/ \
        } catch (const std::exception& e) { \
            std::cerr << "Exception: " << e.what() << std::endl; \
            /*TODO*/ \
        } \
    } while (0)

template<typename T>
T stringToNumber(const std::string& str, T defaultValue) {
    if (str.empty()) return defaultValue;
    try {
        return std::stoi(str);
    } catch (const std::invalid_argument&) {
        try {
            return std::stod(str);
        } catch (...) {
            return defaultValue;
        }
    }
}

class NMEAParser::Impl {
private:
    bool verifyChecksum(const std::string& nmeaMessage) {
        size_t starPos = nmeaMessage.find('*');
        if (starPos == std::string::npos || starPos + 1 >= nmeaMessage.size()) {
            // 没有校验和
            return false;
        }

        std::string checksumHex = nmeaMessage.substr(starPos + 1);
        if (checksumHex.length() != 2) {
            // 校验和格式不正确
            return false;
        }

        unsigned char calculatedChecksum = 0;
        // 从第一个字符开始，直到 '*' 前一个字符
        for (size_t i = 1; i < starPos; ++i) {
            calculatedChecksum ^= nmeaMessage[i];
        }

        unsigned char givenChecksum = static_cast<unsigned char>(std::stoi(checksumHex, nullptr, 16));

        return calculatedChecksum == givenChecksum;
    }

    auto assignLocationMode(const std::string& token) {
        std::string tmpMode = token.substr(1, 2);
        if (tmpMode == "GP") {
            return LocationMode::GPS;
        } else if (tmpMode == "BD") {
            return LocationMode::BD;
        } else {
            return LocationMode::GN;
        }
    }

    double convertDMStoDD(const std::string& dms, char hemisphere) {
        std::istringstream iss(dms);
        int degrees;
        double minutes;
        // 用于读取度与分之间的分隔符
        char temp;

        // 读取度和分
        iss >> degrees >> temp >> minutes;
        // 将分转换为度的小数部分
        minutes /= 60.0;

        // 根据半球符号调整符号
        if (hemisphere == 'S' || hemisphere == 'W') {
            return -(degrees + minutes) / 100;
        } else {
            return (degrees + minutes) / 100;
        }
    }

    
    // 将HHMMSS.SSS格式的时间字符串转换为time_t
    time_t convertToTimestamp(const std::string& utcTime) {
        if (utcTime.empty()) {
            return 0;
        }
        int hour, min, sec, msec;
        std::sscanf(utcTime.c_str(), "%2d%2d%2d.%d", &hour, &min, &sec, &msec);

        std::tm tm = {};
        tm.tm_hour = hour;
        tm.tm_min = min;
        tm.tm_sec = sec;
        tm.tm_year = 100; // 2000年
        tm.tm_mon = 0;   // 一月
        tm.tm_mday = 1;  // 一天

        time_t timestamp = std::mktime(&tm);

        timestamp += msec / 1000.0;

        return timestamp;
    }

public:
    std::optional<NMEAParser::NMEAData> parseNMEA(const std::string& nmeaMessage) {
        if (!verifyChecksum(nmeaMessage)) {
            // 校验和错误，返回空值
            return std::nullopt;
        }

        NMEAParser::NMEAData data;
        if (nmeaMessage.substr(3, 3) == "RMC") {
            auto rmc = parseRMC(nmeaMessage);
            if (rmc) data.rmc = *rmc;
        } else if (nmeaMessage.substr(3, 3) == "GGA") {
            auto gga = parseGGA(nmeaMessage);
            if (gga) data.gga = *gga;
        } else if (nmeaMessage.substr(3, 3) == "GSA") {
            auto gsa = parseGSA(nmeaMessage);
            if (gsa) data.gsa = *gsa;
        } else if (nmeaMessage.substr(3, 3) == "GSV") {
            auto gsv = parseGSV(nmeaMessage);
            if (gsv) data.gsv = *gsv;
        } else if (nmeaMessage.substr(3, 3) == "VTG") {
            auto vtg = parseVTG(nmeaMessage);
            if (vtg) data.vtg = *vtg;
        }

        data.rawMessage = nmeaMessage;
        return data;
    }

private:
    std::optional<RMCData> parseRMC(const std::string& message) {
        RMCData rmc;
        std::istringstream iss(message.substr(1));
        std::string token;
        std::vector<std::string> tokens;
        while (std::getline(iss, token, ',')) {
            tokens.push_back(token);
        }

        if (tokens.size() < 12) {
            return std::nullopt;
        }

        RMCData rmcData;
        CATCH_AND_HANDLE_EXCEPTIONS(
            rmcData.locationMode = assignLocationMode(tokens[0]);
            rmcData.utcTime = convertToTimestamp(tokens[1]);
            rmcData.status = !tokens[2].empty() ? tokens[2][0] : 'V';
            rmcData.latHemisphere = !tokens[4].empty() ? tokens[4][0] : 'N';
            rmcData.latitude = !tokens[3].empty() ? convertDMStoDD(tokens[3], rmcData.latHemisphere) : 0;
            rmcData.lonHemisphere = !tokens[6].empty() ? tokens[6][0] : 'E';
            rmcData.longitude = !tokens[5].empty() ? convertDMStoDD(tokens[5], rmcData.lonHemisphere) : 0;
            rmcData.speed = !tokens[7].empty() ? std::stof(tokens[7]) : 0;
            rmcData.course = !tokens[8].empty() ? std::stof(tokens[8]) : 0;
            rmcData.date = tokens[9];
            rmcData.variation = !tokens[10].empty() ? std::stof(tokens[10]) : 0;
            rmcData.variationDirection = !tokens[11].empty() ? tokens[11][0] : 'E';
            rmcData.mode = !tokens[12].empty() ? tokens[12][0] : 'A';
        );


        return rmcData;
    }

    std::optional<GSAData> parseGSA(const std::string& message) {
        GSAData gsa;
        gsa.rawMessage = message;
        // TODO
        return gsa;
    }

    std::optional<GGAData> parseGGA(const std::string& message) {
        std::istringstream iss(message.substr(1));
        std::string token;
        std::vector<std::string> tokens;
        while (std::getline(iss, token, ',')) {
            tokens.push_back(token);
        }

        if (tokens.size() < 15) {
            return std::nullopt;
        }

        GGAData ggaData;
        CATCH_AND_HANDLE_EXCEPTIONS(
            ggaData.locationMode = assignLocationMode(tokens[0]);
            ggaData.utcTime = convertToTimestamp(std::string(tokens[1]));
            ggaData.status = !tokens[2].empty() ? tokens[2][0] : 'V';
            // ggaData.latitude = !tokens[3].empty() ? std::stof(tokens[3]) : 0;
            ggaData.latHemisphere = !tokens[4].empty() ? tokens[4][0] : 'N';
            ggaData.latitude = convertDMStoDD(tokens[3], ggaData.latHemisphere);
            ggaData.lonHemisphere = !tokens[6].empty() ? tokens[6][0] : 'E';
            // ggaData.longitude = !tokens[5].empty() ? std::stof(tokens[5]) : 0;
            ggaData.longitude = convertDMStoDD(tokens[5], ggaData.lonHemisphere);
            ggaData.satellites = stringToNumber<int>(tokens[7], 0);
            ggaData.hdop = stringToNumber<double>(tokens[8], 0);
            ggaData.altitude = stringToNumber<double>(tokens[9], 0);
            ggaData.altitudeUnit = !tokens[10].empty() ? tokens[10][0] : 'M';
            ggaData.geoidSeparation = stringToNumber<double>(tokens[11], 0);
            ggaData.geoidSeparationUnit = !tokens[12].empty() ? tokens[12][0] : 'M';
            ggaData.ageDifferential = stringToNumber<int>(tokens[13], 0);
            ggaData.stationID = tokens[14];
        );

        return ggaData;
    }

    std::optional<GSVData> parseGSV(const std::string& message) {
        std::istringstream iss(message.substr(1));
        std::string token;
        std::vector<std::string> tokens;
        while (std::getline(iss, token, ',')) {
            tokens.push_back(token);
        }

        if (tokens.size() < 4) {
            // 确保有足够的字段
            return std::nullopt;
        }

        GSVData gsvData;
        gsvData.totalMessages = stringToNumber<int>(tokens[1], 0);
        gsvData.messageNumber = stringToNumber<int>(tokens[2], 0);
        gsvData.satelliteCount = stringToNumber<int>(tokens[3], 0);

        // 根据NMEA 0183标准，卫星信息从第4个字段开始，每4个字段为一组
        for (size_t i = 0; i + 3 < tokens.size(); i += 4) {
            SatelliteInfo satellite;
            satellite.satelliteID = stringToNumber<int>(tokens[i + 3], 0);
            if (!tokens[i + 4].empty()) {
                satellite.elevation = stringToNumber<double>(tokens[i + 4], 0);
            }
            if (!tokens[i + 5].empty()) {
                satellite.azimuth = stringToNumber<double>(tokens[i + 5], 0);
            }
            if (!tokens[i + 6].empty()) {
                satellite.signalToNoiseRatio = stringToNumber<double>(tokens[i + 6], 0);
            }
            gsvData.satellites.push_back(satellite);
        }

        return gsvData;
    }

    std::optional<VTGData> parseVTG(const std::string& message) {
        VTGData vtg;
        vtg.rawMessage = message;
        // TODO
        return vtg;
    }
};

NMEAParser::NMEAParser() : pImpl(std::make_unique<Impl>()) {}

NMEAParser::~NMEAParser() = default;

std::optional<NMEAParser::NMEAData> NMEAParser::parseNMEAMessage(const std::string& nmeaMessage) {
    return pImpl->parseNMEA(nmeaMessage);
}

void NMEAParser::dumpLocationInfo(std::optional<NMEAData> &op) {
    if (!op) {
        std::cerr << "NMEAData optional handler not exist!!!" << std::endl;
    }
    if (op->rmc) {
        std::cout << "locationMode: " << (int)op->rmc->locationMode << std::endl
                  << "utcTime: " << (long long)op->rmc->utcTime << std::endl
                  << "latitude: " << op->rmc->latitude << std::endl
                  << "latHemisphere: " << op->rmc->latHemisphere << std::endl
                  << "longitude: " << op->rmc->longitude <<std::endl
                  << "lonHemisphere: " << op->rmc->lonHemisphere << std::endl
                  << "speed: " << op->rmc->speed << std::endl
                  << "course: " << op->rmc->course << std::endl
                  << "date: " << op->rmc->date << std::endl
                  << "variation: " << op->rmc->variation <<std::endl
                  << "variationDirection: " << op->rmc->variationDirection << std::endl
                  << "mode: " << op->rmc->mode << std::endl;
    } else if(op->gsv) {
        std::cout << "totalMessages: " << (int)op->gsv->totalMessages << std::endl
                  << "messageNumber: " << op->gsv->messageNumber << std::endl
                  << "satelliteCount: " << op->gsv->satelliteCount << std::endl;
        for(auto &s: op->gsv->satellites) {
            std::cout << "satelliteID: " << s.satelliteID << std::endl << "\t"
                      << "elevation: " << s.elevation << std::endl << "\t"
                      << "azimuth: " << s.azimuth << std::endl << "\t"
                      << "signalToNoiseRatio: " << std::endl;
        }
    }
}

}
