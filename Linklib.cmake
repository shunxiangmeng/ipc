#添加公共依赖的库
set(PLATFORM_DEPEND_LIBS)
set(APP_DEPEND_LIBS)

if (UNIX)
    set(APP_DEPEND_LIBS ${APP_DEPEND_LIBS} pthread)
elseif(WIN32)
endif()

include(${PROJECT_SOURCE_DIR}/hardware/soc/${Platform}/Linklib.cmake)

link_directories(${PROJECT_SOURCE_DIR}/hardware/soc/${PLATFORM}/lib)

set(APP_DEPEND_LIBS ${APP_DEPEND_LIBS} ${PLATFORM_DEPEND_LIBS})
