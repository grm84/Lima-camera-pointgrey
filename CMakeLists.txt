# CMakeLists.txt for Pointgrey camera

SET(CMAKE_SHARED_LINKER_FLAGS "-Wl,--add-needed")

SET(pointgrey_srcs src/PointGreyCamera.cpp  src/PointGreyInterface.cpp
	 src/PointGreyDetInfoCtrlObj.cpp src/PointGreySyncCtrlObj.cpp)

FILE(STRINGS "VERSION" pointgrey_vers)
ADD_LIBRARY(limapointgrey SHARED ${pointgrey_srcs})

TARGET_INCLUDE_DIRECTORIES(limapointgrey PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
TARGET_INCLUDE_DIRECTORIES(limapointgrey PUBLIC "${COMMON_PATH}/../include/flycapture")
TARGET_INCLUDE_DIRECTORIES(limapointgrey PUBLIC "${COMMON_PATH}/../include/flycapture/C")

FIND_LIBRARY(POINTGREY_LIB_FLYCAPTURE flycapture)

TARGET_LINK_LIBRARIES(limapointgrey limacore)
TARGET_LINK_LIBRARIES(limapointgrey ${POINTGREY_LIB_FLYCAPTURE})


SET_TARGET_PROPERTIES(limapointgrey PROPERTIES VERSION ${pointgrey_vers} SOVERSION "1")

INSTALL(TARGETS limapointgrey LIBRARY DESTINATION lib)

IF(COMPILE_SIP)
    SET(NAME "pointgrey")
    SET(INCLUDES)
    FILE(GLOB sipfiles RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/sip" "${CMAKE_CURRENT_SOURCE_DIR}/sip/*.sip")
    FOREACH(sipfile ${sipfiles})
        SET(INCLUDES 
        "${INCLUDES} 
        %Include ${sipfile}")
    ENDFOREACH()

    CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/sip/limamodules.sip.in sip/limapointgrey.sip)
    SET(SIP_CONCAT_PARTS 1)
    SET(SIP_INCLUDES ${SIP_INCLUDES}
        "${CMAKE_SOURCE_DIR}/third-party/Processlib/sip"
        "${CMAKE_BINARY_DIR}/sip/core"
        "${CMAKE_SOURCE_DIR}/third-party/Processlib/tasks/sip"
        "${CMAKE_SOURCE_DIR}/common/sip"
        "${CMAKE_SOURCE_DIR}/hardware/sip"
        "${CMAKE_SOURCE_DIR}/control/sip"
        "${CMAKE_SOURCE_DIR}/control/software_operation/sip"
        "${CMAKE_CURRENT_SOURCE_DIR}/sip")
    set(SIP_DISABLE_FEATURES WITH_CONFIG)
    set(SIP_CHECK_EXC ON)
    ADD_SIP_PYTHON_MODULE(limapointgrey ${CMAKE_CURRENT_BINARY_DIR}/sip/limapointgrey.sip)
    TARGET_INCLUDE_DIRECTORIES(python_module_limapointgrey PRIVATE
                               ${PYTHON_INCLUDE_DIRS}
                               "${CMAKE_SOURCE_DIR}/sip"
                               "${CMAKE_SOURCE_DIR}/sip/core")
    TARGET_LINK_LIBRARIES(python_module_limapointgrey limapointgrey)

    INSTALL(FILES python/__init__.py DESTINATION "${PYTHON_SITE_PACKAGES_DIR}/Lima/PointGrey")
ENDIF()
