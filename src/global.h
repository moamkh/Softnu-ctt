#ifndef GLOBAL_H
#define GLOBAL_H

#include <QSettings>
#include "database/db_controller.h"
#include "utils/static_file_utils.h"
#include "staticfilecontroller.h"
#include "httpserver/utils/jwt_manager.h"
#include "httpserver/utils/swagger_doc_generator.h"

using stefanfrings::StaticFileController;

extern const QSettings* myGlobalSettings;
extern DbController* myDbController;
extern StaticFolderUtils* myStaticFolderUtils;
extern StaticFileController* myStaticFileController;
extern SwaggerGenerator* mySwaggerGenerator;
extern JWTManager* myJWTManager;


#endif // GLOBAL_H
