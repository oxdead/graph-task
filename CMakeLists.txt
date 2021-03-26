cmake_minimum_required(VERSION 3.16)

project("graphtask")

add_executable("${PROJECT_NAME}" "graph-task.cpp")

install(TARGETS "${PROJECT_NAME}" DESTINATION bin)

install(FILES "graph-task.cpp" DESTINATION src)