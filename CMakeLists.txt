cmake_minimum_required(VERSION 3.5)

project( FloodFill-analysis )

find_package( OpenCV REQUIRED )

set(SOURCE_DIR ./sources)
set(FUNCTIONS_PATH ${SOURCE_DIR}/functions.cpp)
set(MAIN_PATH ${SOURCE_DIR}/main.cpp)

include_directories( ${OpenCV_INCLUDE_DIRS} )


add_executable( floodfill-analysis ${FUNCTIONS_PATH} ${MAIN_PATH} )
target_link_libraries( floodfill-analysis ${OpenCV_LIBS} )

