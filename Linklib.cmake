#添加公共依赖的库
set(PLATFORM_DEPEND_LIBS)
set(APP_DEPEND_LIBS)

if (UNIX)
    set(APP_DEPEND_LIBS ${APP_DEPEND_LIBS} pthread rt m)
elseif(WIN32)
endif()

#only for test
link_directories(${PROJECT_SOURCE_DIR}/application/ulucu/lib)
set(APP_DEPEND_LIBS ${APP_DEPEND_LIBS} ulu_crypt uluai_core uluai_imgio uluai_ppose uluai_carplate uluai_dump_desc uluai_fea_ext uluai_objdet uluai_passflow uluai_speech_rec rknn_api)


include(${PROJECT_SOURCE_DIR}/hardware/soc/${Platform}/Linklib.cmake)

link_directories(${PROJECT_SOURCE_DIR}/hardware/soc/${PLATFORM}/lib)

set(APP_DEPEND_LIBS ${APP_DEPEND_LIBS} ${PLATFORM_DEPEND_LIBS})
