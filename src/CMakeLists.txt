project(server)
file(GLOB source_file_path "./*/*.cpp")
include_directories(./log ./parser ./server ./thread_pool ./util ./app)
add_executable(server "main.cpp;${source_file_path}")
target_link_libraries(server pthread)