include_directories(..)

add_executable(ls ls.cpp)
add_executable(msh msh.cpp)
add_executable(reflection reflection-example.cpp)

target_link_libraries(ls mlib)
target_link_libraries(msh mlib)
target_link_libraries(reflection mlib)
