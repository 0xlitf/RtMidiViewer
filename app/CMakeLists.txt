cmake_minimum_required(VERSION 3.27.5)

get_filename_component(current_dir "${CMAKE_CURRENT_SOURCE_DIR}" NAME)
string(REPLACE " " "" current_dir "${current_dir}")
string(REPLACE "-" "_" current_dir "${current_dir}")

set(CMAKE_VERBOSE_MAKEFILE ON)
message("CMake start \n\t directory name: ${current_dir}")

project("${current_dir}" VERSION 0.1 LANGUAGES CXX)
string(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UPPER)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets LinguistTools Core Quick Widgets Xml Core5Compat)
find_package(Qt${QT_VERSION_MAJOR} 6.6 REQUIRED COMPONENTS Widgets LinguistTools Core Quick Widgets Xml Core5Compat)

set(QMidi_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../QMidi")
add_subdirectory(${QMidi_DIR} QMidi.out)
#find_package(QMidi REQUIRED CONFIG)

#qt_standard_project_setup(REQUIRES 6.6)
file(GLOB ${PROJECT_NAME_UPPER}_1st_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/1st/*")
add_custom_target(1st SOURCES ${${PROJECT_NAME_UPPER}_1st_SRC})
file(GLOB ${PROJECT_NAME_UPPER}_2nd_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/2nd/*")
add_custom_target(2nd SOURCES ${${PROJECT_NAME_UPPER}_2nd_SRC})
file(GLOB ${PROJECT_NAME_UPPER}_3rd_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/3rd/*")
add_custom_target(3rd SOURCES ${${PROJECT_NAME_UPPER}_3rd_SRC})
file(GLOB ${PROJECT_NAME_UPPER}_4th_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/4th/*")
add_custom_target(4th SOURCES ${${PROJECT_NAME_UPPER}_4th_SRC})
file(GLOB ${PROJECT_NAME_UPPER}_5th_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/5th/*")
add_custom_target(5th SOURCES ${${PROJECT_NAME_UPPER}_5th_SRC})
file(GLOB ${PROJECT_NAME_UPPER}_6th_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/6th/*")
add_custom_target(6th SOURCES ${${PROJECT_NAME_UPPER}_6th_SRC})

file(GLOB_RECURSE ${PROJECT_NAME_UPPER}_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/*")
file(GLOB ${PROJECT_NAME_UPPER}_QRC "${CMAKE_CURRENT_SOURCE_DIR}/qrc/*.qrc")
add_custom_target(qrc SOURCES ${${PROJECT_NAME_UPPER}_QRC})
file(GLOB ${PROJECT_NAME_UPPER}_TS "${CMAKE_CURRENT_SOURCE_DIR}/ts/*.ts")
add_custom_target(ts SOURCES ${${PROJECT_NAME_UPPER}_TS})

source_group("SRC" FILES ${${PROJECT_NAME_UPPER}_SRC})
source_group("QRC" FILES ${${PROJECT_NAME_UPPER}_QRC})
source_group("TS" FILES ${${PROJECT_NAME_UPPER}_TS})

set(PROJECT_SOURCES
    ${${PROJECT_NAME_UPPER}_SRC}
    ${${PROJECT_NAME_UPPER}_QRC}
    ${${PROJECT_NAME_UPPER}_TS}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(${PROJECT_NAME}
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET ${PROJECT_NAME} APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation

    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${PROJECT_NAME_UPPER}_TS)
else()
    if(ANDROID)
        add_library(${PROJECT_NAME} SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(${PROJECT_NAME}
            ${PROJECT_SOURCES}
        )
    endif()

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt6::Core5Compat
    QMidi::QMidi
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.${PROJECT_NAME})
endif()
set_target_properties(${PROJECT_NAME} PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(${PROJECT_NAME})
endif()

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/1st
    ${CMAKE_CURRENT_SOURCE_DIR}/src/2nd
    ${CMAKE_CURRENT_SOURCE_DIR}/src/3rd
    ${CMAKE_CURRENT_SOURCE_DIR}/src/4th
    ${CMAKE_CURRENT_SOURCE_DIR}/src/5th
    ${CMAKE_CURRENT_SOURCE_DIR}/src/6th
    ${3rdParty_DIR}/rapidjson-1.1.0
)
