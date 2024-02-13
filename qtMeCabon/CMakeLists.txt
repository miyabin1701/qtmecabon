cmake_minimum_required(VERSION 3.5)

project(qtMeCabon VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(BUILD_SHARED_LIBS YES)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools)

set(TS_FILES qtMeCabon_ja_JP.ts)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        ${TS_FILES}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(qtMeCabon
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        exTextEdit.h exTextEdit.cpp


        winpretranslatemessage.h winpretranslatemessage.cpp
        qtMeCabon.qrc
        corpasdialog.h corpasdialog.cpp corpasdialog.ui
        csvdictionarysarchdialog.h csvdictionarysarchdialog.cpp csvdictionarysarchdialog.ui
        settingdlg.h settingdlg.cpp settingdlg.ui
        settingValues.h

        profilevalues.h profilevalues.cpp
        cmultilang.h cmultilang.cpp
        pretalk.h pretalk.cpp

        extdial.h extdial.cpp
        ../../AquesTalk.h
        text2wave.h text2wave.cpp
        playlinesapi.h playlinesapi.cpp
        mecabdll.h
        mecab.h
        mecabif.h mecabif.cpp
        portaudioif.h portaudioif.cpp
        portaudio_x64.lib
        portaudio/include/portaudio.h
        messagebuff.h messagebuff.cpp
        qaudioif.h qaudioif.cpp
        portaudio/include/portaudio.h
        portaudiodll.h
	playlinevvoxcore.h playlinevvoxcore.cpp
	voicevox_core.h
	premecab.h premecab.cpp
	ttsthred.h ttsthred.cpp

	talkline.h talkline.cpp
	talktext.h talktext.cpp
	soundplaystate.h soundplaystate.cpp
	waveque.h waveque.cpp
	corpusedit.h corpusedit.cpp
	buildsysdic.h buildsysdic.cpp
	about.h about.cpp about.ui
	pdf2txt.cpp pdfRdTxt.cpp
	RdPdfTkn.cpp RdPdfTkn.h
	PdfRdTxt.h
	recentsel.h recentsel.cpp recentsel.ui





    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET qtMeCabon APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation

    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    if(ANDROID)
        add_library(qtMeCabon SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(qtMeCabon
            ${PROJECT_SOURCES}
        )
    endif()

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

#find_package(portaudio REQUIRED)

#add_library(portaudio_x64)
#add_library(portaudio_x64 MODULE EXCLUDE_FROM_ALL)

target_link_libraries(qtMeCabon PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
#target_link_libraries(portaudio_x64 PUBLIC portaudio_x64.lib)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.qtMeCabon)
endif()
set_target_properties(qtMeCabon PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS qtMeCabon
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(qtMeCabon)
endif()
