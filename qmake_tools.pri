isEmpty(QMAKE_TOOLS_PRI) {
QMAKE_TOOLS_PRI = used

# Флаг для вывода дополнительной информации
ENABLE_QMAKE_DEBUG = false

# Присваивает путь по умолчанию заданной переменной, если она не определена.
# Относительный путь исчисляется от каталога HOME (HOMEPATH под Windows, если HOME не определена)
# абсолютный - остается без изменений.
# Печатает значение переменной в виде VARIABLE = $$VARIABLE
# если использовалось значение по умолчанию - добавляет суффикc [ default ]
defineReplace(setPath) {
    TMP = $$unique($$1)
    isEmpty(TMP) {
        VAR = $$2

        !isEqual(VAR, $$absolute_path($$VAR)) {
            HOME = $$(HOME)
            win32: isEmpty(HOME): HOME = $$(HOMEPATH)

            VAR = $$absolute_path($${HOME}/$${VAR})
        }

        !isEmpty(VAR) {
            TMP = $$system_path($$VAR)
            SUFFIX = [ default ]
        } else {
            SUFFIX = [ empty ]
        }
    } else {
        # В описании функции указано, что путь присваивается, если переменная
        # неопределена, поэтому неясно, нужен ли этот код, он не используется
        TMP = $$system_path($$absolute_path($${TMP}, $${REPOS_ROOT}))
    }

    contains(ENABLE_QMAKE_DEBUG, true):message($$1 = $${TMP} $${SUFFIX})
    return($${TMP})
}


# Копирует файлы в директорию установки приложения.
# 1 параметр - файлы источника, может быть список
# 2 параметр - путь назначения. Может быть пустой. Тогда копируется в корень директории приложения. Ожидается в виде test или test/learn.
defineTest(copyFiles) {
    target = $$list() # генерируется уникальное имя
    files_list = $$1
    dest = $$2

    isEmpty(dest) {
         $${target}.path = $${INSTALL_PATH}
    }

    !isEmpty(dest) {
         $${target}.path = $${INSTALL_PATH}/$$dest
    }

    $${target}.files = $$files_list

    INSTALLS += $$target

    export($${target}.path)
    export($${target}.files)
    export(INSTALLS)

    return(true)
}

}
