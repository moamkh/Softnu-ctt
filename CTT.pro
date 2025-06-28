QT += core
QT += sql
QT += network
QT += gui

CONFIG += c++11
CONFIG += console
CONFIG -= app_bundle
CONFIG += static
# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.

#DEFINES += QT_NO_DEBUG_OUTPUT
# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

HEADERS += \
    src/database/db_controller.h \
    src/database/db_connection_pool.h \
    src/database/scoped_db_connection.h \
    src/database/utils/string_utils.h \
    src/httpserver/services/ctt_service/CallReports.h \
    src/httpserver/utils/api_response_utils.h \
    src/httpserver/utils/jwt_manager.h \
    src/httpserver/utils/json_validator.h \
    src/httpserver/errors/base_api_error.h \
    src/httpserver/errors/api_error_factory.h \
    src/httpserver/request_router.h \
    src/httpserver/utils/swagger_doc_generator.h \
    src/services/CppAmi/core/AiRequestManager.h \
    src/services/CppAmi/core/AudioFileRetrievalScheduler.h \
    src/services/CppAmi/core/ICTAudioFileApiHandler.h \
    src/utils/static_file_utils.h \
    src/utils/utils.h \
    src/global.h

SOURCES += src/main.cpp \
    src/database/db_controller.cpp \
    src/database/db_connection_pool.cpp \
    src/database/scoped_db_connection.cpp \
    src/database/utils/string_utils.cpp \
    src/httpserver/services/ctt_service/CallReports.cpp \
    src/httpserver/utils/api_response_utils.cpp \
    src/httpserver/utils/jwt_manager.cpp \
    src/httpserver/utils/json_validator.cpp \
    src/httpserver/errors/base_api_error.cpp \
    src/httpserver/errors/api_error_factory.cpp \
    src/httpserver/request_router.cpp \
    src/httpserver/utils/swagger_doc_generator.cpp \
    src/services/CppAmi/core/AiRequestManager.cpp \
    src/services/CppAmi/core/AudioFileRetrievalScheduler.cpp \
    src/services/CppAmi/core/ICTAudioFileApiHandler.cpp \
    src/utils/static_file_utils.cpp \
    src/utils/utils.cpp \
    src/global.cpp



HEADERS += \
    src/httpserver/services/base_api_service.h \
    src/httpserver/services/swagger_documentation.h

SOURCES += \
    src/httpserver/services/base_api_service.cpp \
    src/httpserver/services/swagger_documentation.cpp


HEADERS += \
    src/services/CppAmi/core/AmiClient.h \
    src/services/CppAmi/core/AmiLogger.h \
    src/services/CppAmi/core/AmiTypes.h \
    src/services/CppAmi/core/CallMonitor.h \
    src/services/CppAmi/core/CallMonitorManager.h \
    src/services/CppAmi/utils/AmiDbUtils.h \
    src/services/CppAmi/utils/ICTRequestUtils.h \
    src/services/CppAmi/AmiController.h

SOURCES += \
    src/services/CppAmi/core/AmiClient.cpp \
    src/services/CppAmi/core/AmiLogger.cpp \
    src/services/CppAmi/core/AmiTypes.cpp \
    src/services/CppAmi/core/CallMonitor.cpp \
    src/services/CppAmi/core/CallMonitorManager.cpp \
    src/services/CppAmi/utils/AmiDbUtils.cpp \
    src/services/CppAmi/utils/ICTRequestUtils.cpp \
    src/services/CppAmi/AmiController.cpp





INCLUDEPATH += $$PWD/../../../../othersource/boost_1_86
INCLUDEPATH += $$PWD/../../../../othersource/jwt-cpp
INCLUDEPATH += $$PWD/../../../../othersource/picojson
INCLUDEPATH += $$PWD/../../../../othersource/FLAC/include

LIBS += /usr/local/lib/libadclient.so
LIBS += -L/usr/lib -lssl -lcrypto


OTHER_FILES +=
include($$PWD/../../../../othersource/QtWebApp/httpserver/httpserver.pri)

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    etc/Novinsettings.ini \
    etc/static/docs/swagger-ui.html \
    etc/static/docs/swagger-ui.css \
    etc/static/docs/swagger-ui.js \
    etc/static/docs/swagger.json


