project(lokalize)

if(NOT WIN32)
    find_package(HUNSPELL REQUIRED)
else(NOT WIN32)
    find_package(HUNSPELL)
endif(NOT WIN32)

if(HUNSPELL_FOUND)
    add_definitions(-DHAVE_HUNSPELL)
    include_directories(
	${HUNSPELL_INCLUDE_DIRS}
    )
endif(HUNSPELL_FOUND)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/prefs
    ${CMAKE_CURRENT_SOURCE_DIR}/common
    ${CMAKE_CURRENT_SOURCE_DIR}/catalog
    ${CMAKE_CURRENT_SOURCE_DIR}/catalog/gettext
    ${CMAKE_CURRENT_SOURCE_DIR}/catalog/xliff
    ${CMAKE_CURRENT_SOURCE_DIR}/catalog/ts
    ${CMAKE_CURRENT_SOURCE_DIR}/cataloglistview
    ${CMAKE_CURRENT_SOURCE_DIR}/project
    ${CMAKE_CURRENT_SOURCE_DIR}/glossary
    ${CMAKE_CURRENT_SOURCE_DIR}/webquery
    ${CMAKE_CURRENT_SOURCE_DIR}/tm
    ${CMAKE_CURRENT_SOURCE_DIR}/filesearch
    ${CMAKE_CURRENT_SOURCE_DIR}/mergemode
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

set(lokalize_SRCS
    main.cpp
    lokalizemainwindow.cpp
    actionproxy.cpp

    editortab.cpp
    editortab_findreplace.cpp

    editorview.cpp
    xlifftextedit.cpp
    syntaxhighlighter.cpp

    completionstorage.cpp

    phaseswindow.cpp
    noteeditor.cpp
    msgctxtview.cpp

    binunitsview.cpp

    cataloglistview/cataloglistview.cpp
    cataloglistview/catalogmodel.cpp

    common/headerviewmenu.cpp
    common/domroutines.cpp
    common/htmlhelpers.cpp
    common/fastsizehintitemdelegate.cpp
    common/flowlayout.cpp
    common/termlabel.cpp
    common/languagelistmodel.cpp
    common/stemming.cpp
    glossary/glossaryview.cpp
    glossary/glossary.cpp
    glossary/glossarywindow.cpp

    mergemode/mergecatalog.cpp
    mergemode/mergeview.cpp
    alttransview.cpp
    common/diff.cpp
    project/project.cpp
    project/projectmodel.cpp
    project/projectwidget.cpp
    project/projecttab.cpp
    project/poextractor.cpp
    project/xliffextractor.cpp

    prefs/prefs.cpp

    webquery/webqueryview.cpp
    webquery/webquerycontroller.cpp
    webquery/myactioncollectionview.cpp
    tools/widgettextcaptureconfig.cpp

    filesearch/filesearchtab.cpp

    tm/tmview.cpp
    tm/tmscanapi.cpp
    tm/jobs.cpp
    tm/dbfilesmodel.cpp
    tm/tmmanager.cpp
    tm/tmtab.cpp
    tm/qaview.cpp
    tm/qamodel.cpp

    catalog/phase.cpp

    catalog/cmd.cpp
    catalog/pos.cpp
    catalog/catalog.cpp
    catalog/catalogstring.cpp
    catalog/gettextheader.cpp
    catalog/gettext/gettextstorage.cpp
    catalog/gettext/catalogitem.cpp
    catalog/gettext/importplugin.cpp
    catalog/gettext/gettextimport.cpp
    catalog/gettext/gettextexport.cpp
    catalog/xliff/xliffstorage.cpp
    catalog/ts/tsstorage.cpp
)

if(WIN32)
    list (APPEND lokalize_SRCS common/winhelpers.cpp)
else(WIN32)
    list (APPEND lokalize_SRCS common/unixhelpers.cpp)
endif(WIN32)

ecm_qt_declare_logging_category(lokalize_SRCS
    HEADER lokalize_debug.h
    IDENTIFIER LOKALIZE_LOG
    CATEGORY_NAME org.kde.lokalize
    DEFAULT_SEVERITY Warning
)

ki18n_wrap_ui(lokalize_SRCS
    prefs/prefs_identity.ui
    prefs/prefs_general.ui
    prefs/prefs_editor.ui
    prefs/prefs_appearance.ui
    prefs/prefs_tm.ui
    prefs/prefs_pology.ui
    project/prefs_project_advanced.ui
    project/prefs_project_local.ui
    project/prefs_projectmain.ui
    glossary/termedit.ui
    filesearch/filesearchoptions.ui
    filesearch/massreplaceoptions.ui
    tm/queryoptions.ui
    tm/managedatabases.ui
    tm/dbparams.ui
    kaider_findextension.ui
    webquery/querycontrol.ui
    tools/widgettextcaptureconfig.ui
)

kconfig_add_kcfg_files(lokalize_SRCS
    prefs/prefs_lokalize.kcfgc
    project/projectbase.kcfgc
    project/projectlocal.kcfgc
)

qt5_add_dbus_adaptor(lokalize_SRCS org.kde.lokalize.MainWindow.xml
                     lokalizemainwindow.h LokalizeMainWindow)
qt5_add_dbus_adaptor(lokalize_SRCS org.kde.lokalize.Editor.xml
                     editortab.h EditorTab)
qt5_add_dbus_adaptor(lokalize_SRCS filesearch/org.kde.lokalize.FileSearch.xml
                     filesearch/filesearchtab.h FileSearchTab)
qt5_add_dbus_adaptor(lokalize_SRCS tm/org.kde.lokalize.TranslationMemory.xml
                     tm/tmtab.h TM::TMTab)
qt5_add_dbus_adaptor(lokalize_SRCS project/org.kde.lokalize.Project.xml
                     project/project.h Project)
qt5_add_dbus_adaptor(lokalize_SRCS project/org.kde.lokalize.ProjectOverview.xml
                     project/projecttab.h ProjectTab)



file(GLOB ICONS_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/../icons/global/*-apps-lokalize.png")
ecm_add_app_icon(lokalize_SRCS ICONS ${ICONS_SRCS})

add_executable(lokalize ${lokalize_SRCS})

target_link_libraries(lokalize
KF5::KIOFileWidgets
KF5::ItemViews
KF5::Notifications
KF5::SonnetCore
KF5::SonnetUi
KF5::KrossCore
KF5::KrossUi
KF5::DBusAddons
KF5::Crash
Qt5::Sql
)
if(HUNSPELL_FOUND)
    target_link_libraries(lokalize ${HUNSPELL_LIBRARIES})
endif(HUNSPELL_FOUND)

install(TARGETS lokalize ${INSTALL_TARGETS_DEFAULT_ARGS} )


########### install files ###############

install( PROGRAMS org.kde.lokalize.desktop  DESTINATION  ${XDG_APPS_INSTALL_DIR} )
install( FILES prefs/lokalize.kcfg  DESTINATION  ${KCFG_INSTALL_DIR} )
install( FILES lokalize.notifyrc DESTINATION  ${KNOTIFYRC_INSTALL_DIR} )
install( FILES lokalize.categories DESTINATION ${KDE_INSTALL_CONFDIR} )

install( FILES
        editorui.rc
        lokalizemainwindowui.rc
        scriptsui.rc
        project/projectmanagerui.rc
        tm/translationmemoryrui.rc
        filesearch/filesearchtabui.rc
        DESTINATION  ${KXMLGUI_INSTALL_DIR}/lokalize )
