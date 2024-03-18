isEmpty(PARAM_INSTALL_PATH_PRI) {
PARAM_INSTALL_PATH_PRI = used

# INSTALL_PATH
# Путь установки. По умолчанию принимает значение
# Linux: $HOME/deploy/simple_log_viewer
# Windows: %HOME%/deploy/simple_log_viewer, либо
# %HOMEPATH%/deploy/simple_log_viewer, если HOME не определена.

INSTALL_PATH = $$(INSTALL_PATH)
INSTALL_PATH = $$setPath(INSTALL_PATH, deploy/simple_log_viewer)

}
