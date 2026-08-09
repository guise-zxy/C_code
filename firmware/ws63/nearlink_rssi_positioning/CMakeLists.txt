#===============================================================================
# @brief    CMake file for the NearLink RSSI positioning application.
#===============================================================================

set(COMPONENT_NAME "nearlink_rssi_positioning")

set(SOURCES
)

if(DEFINED CONFIG_NEARLINK_RSSI_POSITIONING_MIN_SERVER)
    list(APPEND SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/nearlink_rssi_server.c
    )
endif()

if(DEFINED CONFIG_NEARLINK_RSSI_POSITIONING_RSSI_CLIENT)
    list(APPEND SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/nearlink_rssi_client.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/nearlink_rssi_calibration.c
    )
endif()

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
)

set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()
