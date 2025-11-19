cmake_minimum_required(VERSION 3.16)

project(SpaceBuddyApp VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick WebSockets)
find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Threads REQUIRED)


qt_standard_project_setup(REQUIRES 6.8)

qt_add_executable(appSpaceBuddyApp
    main.cpp
)

# Set Qt-specific compiler definitions for code highlighting
if(WIN32)
    target_compile_definitions(appSpaceBuddyApp PRIVATE Q_OS_WIN)
elseif(UNIX AND NOT APPLE)
    target_compile_definitions(appSpaceBuddyApp PRIVATE Q_OS_LINUX)
elseif(APPLE)
    target_compile_definitions(appSpaceBuddyApp PRIVATE Q_OS_MAC)
endif()

qt_add_qml_module(appSpaceBuddyApp
    URI SpaceBuddyApp
    QML_FILES
        Main.qml
        SOURCES socketclient.h socketclient.cpp
        SOURCES json.hpp
        SOURCES config.h
        SOURCES backend.h backend.cpp
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appSpaceBuddyApp PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appSpaceBuddyApp
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appSpaceBuddyApp
    PRIVATE Qt6::Quick Qt6::WebSockets Threads::Threads
)
target_link_libraries(appSpaceBuddyApp PRIVATE Qt6::Core)

# Platform-specific global hotkey dependencies
if(WIN32)
    target_link_libraries(appSpaceBuddyApp PRIVATE user32)
elseif(UNIX)
    find_package(X11 REQUIRED)
    target_link_libraries(appSpaceBuddyApp PRIVATE ${X11_LIBRARIES} Xtst)
endif()

include(GNUInstallDirs)
install(TARGETS appSpaceBuddyApp
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
