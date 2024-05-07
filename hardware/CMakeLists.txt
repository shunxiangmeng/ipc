
file(GLOB_RECURSE APP_PLATFORM_SOURCE_FILES media/*.cpp soc/${Platform}/*.cpp)
file(GLOB_RECURSE APP_PLATFORM_HEADER_FILES media/*.h soc/${Platform}/*.h)

set(APP_SOURCE_FILES ${APP_SOURCE_FILES} ${APP_PLATFORM_SOURCE_FILES} PARENT_SCOPE)
set(APP_SOURCE_HEADER_FILES ${APP_SOURCE_HEADER_FILES} ${APP_PLATFORM_HEADER_FILES} PARENT_SCOPE)

if(${Platform} STREQUAL "RK")
    set(APP_INCLUDE_DIRS ${APP_INCLUDE_DIRS} 
        ${CMAKE_CURRENT_SOURCE_DIR}/soc/RK/include/rkaiq/common 
        ${CMAKE_CURRENT_SOURCE_DIR}/soc/RK/include/rkaiq/algos 
        ${CMAKE_CURRENT_SOURCE_DIR}/soc/RK/include/rkaiq/xcore
        ${CMAKE_CURRENT_SOURCE_DIR}/soc/RK/include/rkaiq/iq_parser
        PARENT_SCOPE)
endif()
