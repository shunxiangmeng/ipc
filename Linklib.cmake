#添加公共依赖的库
set(PLATFORM_DEPEND_LIBS)
set(APP_DEPEND_LIBS)

if (UNIX)
    set(APP_DEPEND_LIBS ${APP_DEPEND_LIBS} pthread rt m)
elseif(WIN32)
    set(APP_DEPEND_LIBS ${APP_DEPEND_LIBS} Ws2_32.lib Wldap32.lib Crypt32.lib Normaliz.lib libcurl.lib)
endif()

include(${PROJECT_SOURCE_DIR}/hardware/soc/${Platform}/Linklib.cmake)

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/hardware/soc/${PLATFORM}/lib)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/common/thirdparty/prebuild/lib/win32)

set(APP_DEPEND_LIBS ${APP_DEPEND_LIBS} ${PLATFORM_DEPEND_LIBS})
