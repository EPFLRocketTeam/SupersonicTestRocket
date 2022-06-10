cmake_minimum_required(VERSION 3.6.2)
project(hermesGS)

###############################################################
# Set the build varialble to ON or OFF on your need
set(USE_GUI ON)
set(USE_XBEE ON)
set(RUNNING_ON_RPI OFF) # <-- if not ON, the Ignition hardware won't wrork
set(USE_SOUND OFF)
####
set(USE_LORA OFF)
###############################################################
if (NOT RUNNING_ON_RPI)
    message("/!\\ Ignition Circuit not activated !")
endif ()

set(CMAKE_CXX_STANDARD 14)

add_subdirectory(Logger)
add_subdirectory(Telecom)
add_subdirectory(RF-UI-Interface)

if (USE_XBEE)
    include_directories(Telecom/RFmodem/lib/serial/include/)

    add_executable(hermesGS src/HERMES_GS.cpp)
    add_executable(hermesAVsim src/HERMES_AVsim.cpp)

    if (USE_GUI)
        find_package(Qt5Widgets)
        find_package(Qt5Charts)
        qt5_add_resources(RCC_SOURCES UI/assets.qrc)      # add resources to Gui
        add_subdirectory(UI)
        #add_executable(${PROJECT_NAME} ${RCC_SOURCES} src/ERT2020GS.cpp)
    endif (USE_GUI)

endif (USE_XBEE)

############# link librarie to targets #######################
if (USE_XBEE)
    target_link_libraries(hermesGS Telecom)
    target_link_libraries(hermesAVsim Telecom)

    include_directories(RFmodem/lib/serial/include/)

    if (USE_GUI)
        target_link_libraries(${PROJECT_NAME} GUI)
        target_include_directories(${PROJECT_NAME} PUBLIC UI)
    endif (USE_GUI)

endif (USE_XBEE)
