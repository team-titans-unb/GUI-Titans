cmake_minimum_required(VERSION 3.16)

project(GUI-Titans VERSION 0.1 LANGUAGES CXX)

# Força uso do GCC
set(CMAKE_CXX_COMPILER /usr/bin/g++)
set(CMAKE_C_COMPILER /usr/bin/gcc)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Network LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network LinguistTools)
find_package(Protobuf REQUIRED)

# Ajuste de flags: troca -fcolor-diagnostics (clang) por -fdiagnostics-color=always (g++)
foreach(flag_var CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
                 CMAKE_CXX_FLAGS_RELWITHDEBINFO CMAKE_CXX_FLAGS_MINSIZEREL)
    string(REPLACE "-fcolor-diagnostics" "-fdiagnostics-color=always" ${flag_var} "${${flag_var}}")
endforeach()

# --- Criação da biblioteca Protobuf ---
file(GLOB PROTO_FILES "${CMAKE_CURRENT_SOURCE_DIR}/proto/*.proto")

protobuf_generate(
    PROTOS ${PROTO_FILES}
    IMPORT_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/proto
    OUT_VAR PROTOBUF_SOURCES
)

add_library(vision_protocol ${PROTOBUF_SOURCES})
target_link_libraries(vision_protocol PRIVATE ${Protobuf_LIBRARIES})
target_include_directories(vision_protocol PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_compile_definitions(vision_protocol PRIVATE PROTOBUF_USE_DLLS)
# --- Fim da criação da biblioteca ---

set(TS_FILES GUI-Titans_pt_BR.ts)

set(PROJECT_SOURCES
      main.cpp
      mainwindow.cpp
      mainwindow.h
      mainwindow.ui
      ${TS_FILES}
)

# Criação do executável dependendo do Qt
if(QT_VERSION_MAJOR GREATER_EQUAL 6)
    qt_add_executable(GUI-Titans
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        resources.qrc
        robotwidget.h robotwidget.cpp
        robotselectiondialog.h robotselectiondialog.cpp
        robotdata.h
        configmanager.h configmanager.cpp
    )
    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    add_executable(GUI-Titans
        ${PROJECT_SOURCES}
        resources.qrc
        robotwidget.h robotwidget.cpp
        robotselectiondialog.h robotselectiondialog.cpp
        robotdata.h
        configmanager.h configmanager.cpp
    )
endif()

# --- Linkagem final ---
target_link_libraries(GUI-Titans PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Network
    vision_protocol
)

# --- Propriedades ---
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.GUI-Titans)
endif()
set_target_properties(GUI-Titans PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS GUI-Titans
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(GUI-Titans)
endif()
