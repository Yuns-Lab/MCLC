# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "src\\CMakeFiles\\Core_autogen.dir\\AutogenUsed.txt"
  "src\\CMakeFiles\\Core_autogen.dir\\ParseCache.txt"
  "src\\CMakeFiles\\Launcher_autogen.dir\\AutogenUsed.txt"
  "src\\CMakeFiles\\Launcher_autogen.dir\\ParseCache.txt"
  "src\\CMakeFiles\\MCLC_autogen.dir\\AutogenUsed.txt"
  "src\\CMakeFiles\\MCLC_autogen.dir\\ParseCache.txt"
  "src\\CMakeFiles\\Network_autogen.dir\\AutogenUsed.txt"
  "src\\CMakeFiles\\Network_autogen.dir\\ParseCache.txt"
  "src\\Core_autogen"
  "src\\Launcher_autogen"
  "src\\MCLC_autogen"
  "src\\Network_autogen"
  )
endif()
