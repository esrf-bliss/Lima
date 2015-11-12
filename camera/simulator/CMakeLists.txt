
set(simu_srcs src/SimulatorFrameBuilder.cpp src/SimulatorCamera.cpp src/SimulatorInterface.cpp
              src/SimulatorSyncCtrlObj.cpp src/SimulatorDetInfoCtrlObj.cpp src/SimulatorShutterCtrlObj.cpp)

file(STRINGS "VERSION" simu_ver)
add_library(limasimulator SHARED
    ${simu_srcs}
)
TARGET_INCLUDE_DIRECTORIES(limasimulator PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
TARGET_INCLUDE_DIRECTORIES(limasimulator PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../common/include")
TARGET_INCLUDE_DIRECTORIES(limasimulator PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../hardware/include")
TARGET_LINK_LIBRARIES(limasimulator limacore)
SET_TARGET_PROPERTIES(limasimulator PROPERTIES VERSION ${simu_ver} SOVERSION "1")

