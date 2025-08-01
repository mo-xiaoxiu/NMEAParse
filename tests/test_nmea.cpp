#include "NMEAParser.h"
#include <gtest/gtest.h>

TEST(NMEAParserTest, BasicParsing) {
    NmeaParser::NMEAParser parser;
    
    // 测试有效的RMC消息
    std::string validRMC = "$$GNRMC,041704.000,A,2935.21718,N,10631.58906,E,0.00,172.39,071124,,,A*7E";
    auto result = parser.parseNMEAMessage(validRMC);
    
    EXPECT_TRUE(result->rmc.has_value());
    
    if (result && result->rmc) {
        EXPECT_EQ(result->rmc->latitude, 48.142333);
        EXPECT_EQ(result->rmc->longitude, 11.523667);
        EXPECT_EQ(result->rmc->speed, 22.4);
    }
}

TEST(NMEAParserTest, InvalidMessage) {
    NmeaParser::NMEAParser parser;
    
    // 测试无效消息
    std::string invalidMessage = "Invalid NMEA message";
    auto result = parser.parseNMEAMessage(invalidMessage);
    
    // 应该返回空值或解析失败
    EXPECT_FALSE(!result->rmc.has_value());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
} 