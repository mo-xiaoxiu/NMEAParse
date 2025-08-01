cmake_minimum_required(VERSION 3.10)
project(NMEAParser VERSION 1.0.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 项目选项
option(BUILD_SHARED_LIBS "Build shared libraries" ON)
option(BUILD_STATIC_LIBS "Build static libraries" ON)
option(BUILD_EXAMPLES "Build example programs" OFF)
option(BUILD_TESTS "Build test programs" OFF)

# 源文件
set(SOURCE_FILES
    src/NMEAParser.cpp
)

# 头文件
set(HEADER_FILES
    include/NMEAParser.h
)

# 查找依赖
find_package(Threads REQUIRED)

# 创建库目标
if(BUILD_SHARED_LIBS)
    add_library(NMEAParser_shared SHARED ${SOURCE_FILES})
    set_target_properties(NMEAParser_shared PROPERTIES
        OUTPUT_NAME NMEAParser
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
        PUBLIC_HEADER "${HEADER_FILES}"
    )
    
    # 设置包含目录
    target_include_directories(NMEAParser_shared
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:include>
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/src
    )
    
    # 链接依赖库
    target_link_libraries(NMEAParser_shared PRIVATE Threads::Threads)
    
    # 设置编译定义
    target_compile_definitions(NMEAParser_shared PRIVATE NMEAPARSER_EXPORTS)
endif()

if(BUILD_STATIC_LIBS)
    add_library(NMEAParser_static STATIC ${SOURCE_FILES})
    set_target_properties(NMEAParser_static PROPERTIES
        OUTPUT_NAME NMEAParser
        PUBLIC_HEADER "${HEADER_FILES}"
    )
    
    # 设置包含目录
    target_include_directories(NMEAParser_static
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:include>
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/src
    )
    
    # 链接依赖库
    target_link_libraries(NMEAParser_static PRIVATE Threads::Threads)
endif()

# 创建别名目标
if(BUILD_SHARED_LIBS AND BUILD_STATIC_LIBS)
    add_library(NMEAParser ALIAS NMEAParser_shared)
elseif(BUILD_SHARED_LIBS)
    add_library(NMEAParser ALIAS NMEAParser_shared)
elseif(BUILD_STATIC_LIBS)
    add_library(NMEAParser ALIAS NMEAParser_static)
endif()

# 安装配置
include(GNUInstallDirs)

# 安装库文件
if(BUILD_SHARED_LIBS)
    install(TARGETS NMEAParser_shared
        EXPORT NMEAParserTargets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/NMEAParser
    )
endif()

if(BUILD_STATIC_LIBS)
    install(TARGETS NMEAParser_static
        EXPORT NMEAParserTargets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/NMEAParser
    )
endif()

# 安装头文件
install(FILES ${HEADER_FILES}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/NMEAParser
)

# 导出目标
install(EXPORT NMEAParserTargets
    FILE NMEAParserTargets.cmake
    NAMESPACE NMEAParser::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/NMEAParser
)

# 创建配置文件
include(CMakePackageConfigHelpers)

# 创建包配置文件
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/NMEAParserConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/NMEAParserConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/NMEAParser
)

# 创建版本文件
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/NMEAParserConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

# 安装配置文件
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/NMEAParserConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/NMEAParserConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/NMEAParser
)

# 生成pkg-config文件
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/NMEAParser.pc.in"
    "${CMAKE_CURRENT_BINARY_DIR}/NMEAParser.pc"
    @ONLY
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/NMEAParser.pc"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
)

# 示例程序
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# 测试程序
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
