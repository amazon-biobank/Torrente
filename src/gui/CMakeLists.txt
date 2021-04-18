set(CMAKE_AUTORCC True)
set(CMAKE_AUTOUIC True)

add_library(qbt_gui_headers INTERFACE)
target_include_directories(qbt_gui_headers INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

find_library(Lyra2FileEncryptor NAMES Lyra2FileEncryptor HINTS "/usr/local/lib/" REQUIRED)
include_directories("/usr/local/include/")

find_library(JSONCPPLIB NAMES jsoncpp 
    PATHS   /usr/local/lib/jsoncpp
            /usr/lib/jsoncpp
    REQUIRED)
include_directories("/usr/local/include/jsoncpp/json/")

add_subdirectory(properties)
add_subdirectory(powermanagement)
add_subdirectory(rss)
add_subdirectory(search)

add_library(qbt_gui STATIC
# headers
aboutdialog.h
addnewtorrentdialog.h
advancedsettings.h
autoexpandabledialog.h
authdialog.h
banlistoptionsdialog.h
categoryfiltermodel.h
categoryfilterproxymodel.h
categoryfilterwidget.h
cookiesdialog.h
cookiesmodel.h
deletionconfirmationdialog.h
downloadfromurldialog.h
executionlogwidget.h
fspathedit.h
hidabletabwidget.h
ipsubnetwhitelistoptionsdialog.h
lineedit.h
log/logfiltermodel.h
log/loglistview.h
log/logmodel.h
mainwindow.h
optionsdialog.h
previewlistdelegate.h
previewselectdialog.h
private/fspathedit_p.h
private/tristatewidget.h
raisedmessagebox.h
scanfoldersdelegate.h
shutdownconfirmdialog.h
speedlimitdialog.h
statsdialog.h
statusbar.h
tagfiltermodel.h
tagfilterproxymodel.h
tagfilterwidget.h
torrentcategorydialog.h
torrentcontentfiltermodel.h
torrentcontentmodel.h
torrentcontentmodelfile.h
torrentcontentmodelfolder.h
torrentcontentmodelitem.h
torrentcontenttreeview.h
torrentcreatordialog.h
trackerentriesdialog.h
transferlistdelegate.h
transferlistfilterswidget.h
transferlistmodel.h
transferlistsortmodel.h
transferlistwidget.h
tristateaction.h
uithememanager.h
updownratiodialog.h
utils.h

# sources
aboutdialog.cpp
addnewtorrentdialog.cpp
advancedsettings.cpp
autoexpandabledialog.cpp
authdialog.cpp
banlistoptionsdialog.cpp
categoryfiltermodel.cpp
categoryfilterproxymodel.cpp
categoryfilterwidget.cpp
cookiesdialog.cpp
cookiesmodel.cpp
deletionconfirmationdialog.cpp
downloadfromurldialog.cpp
executionlogwidget.cpp
fspathedit.cpp
hidabletabwidget.cpp
ipsubnetwhitelistoptionsdialog.cpp
lineedit.cpp
log/logfiltermodel.cpp
log/loglistview.cpp
log/logmodel.cpp
mainwindow.cpp
optionsdialog.cpp
previewlistdelegate.cpp
previewselectdialog.cpp
private/fspathedit_p.cpp
private/tristatewidget.cpp
raisedmessagebox.cpp
scanfoldersdelegate.cpp
shutdownconfirmdialog.cpp
speedlimitdialog.cpp
statsdialog.cpp
statusbar.cpp
tagfiltermodel.cpp
tagfilterproxymodel.cpp
tagfilterwidget.cpp
torrentcategorydialog.cpp
torrentcontentfiltermodel.cpp
torrentcontentmodel.cpp
torrentcontentmodelfile.cpp
torrentcontentmodelfolder.cpp
torrentcontentmodelitem.cpp
torrentcontenttreeview.cpp
torrentcreatordialog.cpp
trackerentriesdialog.cpp
transferlistdelegate.cpp
transferlistfilterswidget.cpp
transferlistmodel.cpp
transferlistsortmodel.cpp
transferlistwidget.cpp
tristateaction.cpp
uithememanager.cpp
updownratiodialog.cpp
utils.cpp

# forms
aboutdialog.ui
addnewtorrentdialog.ui
autoexpandabledialog.ui
authdialog.ui
banlistoptionsdialog.ui
cookiesdialog.ui
deletionconfirmationdialog.ui
downloadfromurldialog.ui
executionlogwidget.ui
ipsubnetwhitelistoptionsdialog.ui
mainwindow.ui
optionsdialog.ui
previewselectdialog.ui
shutdownconfirmdialog.ui
speedlimitdialog.ui
statsdialog.ui
torrentcategorydialog.ui
torrentcreatordialog.ui
trackerentriesdialog.ui
updownratiodialog.ui
)

target_link_libraries(qbt_gui
    PRIVATE
        qbt_powermanagement qbt_rss qbt_properties qbt_searchengine
        qbt_base
        Lyra2FileEncryptor
        ${JSONCPPLIB}
)

target_include_directories(qbt_gui
    PRIVATE ../app
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
    PUBLIC "/usr/local/include/"
    PUBLIC "/usr/local/include/jsoncpp/"
)

if (UNIX AND Qt5DBus_FOUND)
   add_subdirectory(qtnotify)
   target_link_libraries(qbt_gui PRIVATE qbt_qtnotify)
endif (UNIX AND Qt5DBus_FOUND)

if (APPLE)
    target_sources(qbt_gui PRIVATE macutilities.h macutilities.mm)
    find_package(Qt5 ${requiredQtVersion} REQUIRED COMPONENTS MacExtras)
    target_link_libraries(qbt_gui PRIVATE Qt5::MacExtras objc)
endif (APPLE)

if (WIN32 OR APPLE)
    target_sources(qbt_gui PRIVATE programupdater.h programupdater.cpp)
endif (WIN32 OR APPLE)

qbt_target_sources(qBittorrent PRIVATE about.qrc)

if(WIN32)
    find_package(Qt5 ${requiredQtVersion} REQUIRED COMPONENTS WinExtras)
    target_link_libraries(qbt_gui PRIVATE Qt5::WinExtras)
endif(WIN32)
