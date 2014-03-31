include_directories(..)

add_executable(ls ls.cpp)

target_link_libraries(ls mlib)
