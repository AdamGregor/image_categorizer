cmake_minimum_required(VERSION 3.15)
project(CMake_project VERSION 1.0)

add_executable(photo_organizer src/main.cpp src/misc.cpp)

#translation flags (mostly c++20 for the auto templates)
add_library(tutorial_compiler_flags INTERFACE)
set(gcc_like_cxx "$<COMPILE_LANG_AND_ID:CXX,ARMClang,AppleClang,Clang,GNU,LCC>")
target_compile_options(tutorial_compiler_flags INTERFACE
"$<${gcc_like_cxx}:$<BUILD_INTERFACE:-Wall;-Wextra;-Wshadow;-Wformat=2;-Wunused;-std=c++20;>>" 
)

#add nlohmann
set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(3rd_party/json)

#add OpenCV
#set("OpenCV_DIR" "/tmp/opencv/build/")
find_package(OpenCV REQUIRED)
include_directories( ${OpenCV_INCLUDE_DIRS} )

#add Tesseract
#set("Tesseract_DIR" "/tmp/tesseract/build/cmake/tesseract/")
find_package(Tesseract REQUIRED)
set("TESSDATA_PREFIX" "/usr/local/share/tessdata/")


target_link_libraries(photo_organizer 
                        PRIVATE nlohmann_json::nlohmann_json ${OpenCV_LIBS} Tesseract::libtesseract 
                        PUBLIC tutorial_compiler_flags
                    )