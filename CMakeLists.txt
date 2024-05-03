cmake_minimum_required(VERSION 3.5)

project(aw_emu LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory("externals")
include_directories("${unicorn_SOURCE_DIR}/include")

include_directories("include")
add_executable(aw_emu
  "src/main.cpp"
 "include/util.h")

target_link_libraries(aw_emu PRIVATE unicorn)

include(GNUInstallDirs)
install(TARGETS aw_emu
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
