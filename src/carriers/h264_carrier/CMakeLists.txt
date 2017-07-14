# Copyright (C) 2010 RobotCub Consortium
# Authors: Valentina Gaggero
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

yarp_prepare_plugin(h264
                    CATEGORY carrier
                    TYPE yarp::os::H264Carrier
                    INCLUDE H264Carrier.h
                    EXTRA_CONFIG CODE=""
                    DEPENDS "CREATE_OPTIONAL_CARRIERS")

#if(NOT SKIP_H264)


  set(CMAKE_INCLUDE_CURRENT_DIR ON)

  find_package(PkgConfig)

  pkg_check_modules(GST REQUIRED gstreamer-1.0>=1.4
                               gstreamer-sdp-1.0>=1.4
                               gstreamer-video-1.0>=1.4
                               gstreamer-app-1.0>=1.4)

  pkg_check_modules(GDKPIXBUF REQUIRED gdk-pixbuf-2.0)


  include_directories(SYSTEM ${H264_INCLUDE_DIR})
  include_directories(${CMAKE_CURRENT_DIR}  ${GST_INCLUDE_DIRS} )
  include_directories(${GDKPIXBUF_INCLUDE_DIRS})

  yarp_add_plugin(yarp_h264
                  H264Carrier.h
                  H264Carrier.cpp
                  H264Stream.h
                  H264Stream.cpp 
                  H264Decoder.cpp
                  H264Decoder.h)

  target_link_libraries(yarp_h264 YARP::YARP_OS
                                   YARP::YARP_sig
                                   YARP::YARP_wire_rep_utils
                                   ${GST_LIBRARIES} ${GDKPIXBUF_LIBRARIES})

  yarp_install(TARGETS yarp_h264
               EXPORT YARP
               COMPONENT runtime
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})
  yarp_install(FILES h264.ini
               COMPONENT runtime
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set_property(TARGET yarp_h264 PROPERTY FOLDER "Plugins/Carrier")
#endif()
