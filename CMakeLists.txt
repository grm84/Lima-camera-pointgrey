############################################################################
# This file is part of LImA, a Library for Image Acquisition
#
# Copyright (C) : 2009-2017
# European Synchrotron Radiation Facility
# BP 220, Grenoble 38043
# FRANCE
#
# This is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This software is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, see <http://www.gnu.org/licenses/>.
############################################################################
# CMakeLists.txt for Pointgrey camera

SET(CMAKE_SHARED_LINKER_FLAGS "-Wl,--add-needed")

#SOURCEs
SET(pointgrey_srcs src/PointGreyCamera.cpp  src/PointGreyInterface.cpp
	 src/PointGreyDetInfoCtrlObj.cpp src/PointGreySyncCtrlObj.cpp)

FILE(STRINGS "VERSION" pointgrey_vers)
ADD_LIBRARY(limapointgrey SHARED ${pointgrey_srcs})

#INCLUDEs
TARGET_INCLUDE_DIRECTORIES(limapointgrey PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
TARGET_INCLUDE_DIRECTORIES(limapointgrey PUBLIC "${COMMON_PATH}/../include/flycapture")
TARGET_INCLUDE_DIRECTORIES(limapointgrey PUBLIC "${COMMON_PATH}/../include/flycapture/C")

#LIBRARIES
FIND_LIBRARY(POINTGREY_LIB_FLYCAPTURE flycapture)
IF("${POINTGREY_LIB_FLYCAPTURE}" STREQUAL "POINTGREY_LIB_FLYCAPTURE-NOTFOUND")
  MESSAGE(FATAL_ERROR "pointgrey : flycapture not found, you can search it in cmake-gui or try to install it if it isn't here.")
ENDIF()

TARGET_LINK_LIBRARIES(limapointgrey limacore)
TARGET_LINK_LIBRARIES(limapointgrey ${POINTGREY_LIB_FLYCAPTURE})


SET_TARGET_PROPERTIES(limapointgrey PROPERTIES VERSION ${pointgrey_vers} SOVERSION "1")

INSTALL(TARGETS limapointgrey LIBRARY DESTINATION lib)

IF(COMPILE_SIP)
    SET(NAME "pointgrey")
    include(CameraRunSIP)
    INSTALL(FILES python/__init__.py DESTINATION "${PYTHON_SITE_PACKAGES_DIR}/Lima/PointGrey")
ENDIF()
