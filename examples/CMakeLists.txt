# 示例程序
add_executable(nmea_example main.cpp)
target_link_libraries(nmea_example NMEAParser)
target_include_directories(nmea_example PRIVATE ${CMAKE_SOURCE_DIR}/include) 