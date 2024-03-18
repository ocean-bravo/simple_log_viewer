TEMPLATE = aux

DISTFILES = $$PWD/*.qml \
            $$PWD/*.js \


# копирование QML файлов
copyFiles($$DISTFILES, gui)
