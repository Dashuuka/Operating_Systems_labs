add_library(Common INTERFACE)
target_include_directories(Common INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_sources(Common INTERFACE
    MessageQueue.h
    SyncUtils.h
)