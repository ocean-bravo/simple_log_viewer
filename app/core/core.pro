QT += quick serialport gui printsupport concurrent quickcontrols2

TEMPLATE = app

TARGET = simple_log_viewer

HEADERS += *.h

SOURCES += *.cpp

target.path = $${INSTALL_PATH}

message($${INSTALL_PATH})
INSTALLS += target
