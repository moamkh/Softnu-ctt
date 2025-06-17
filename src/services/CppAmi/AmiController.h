#ifndef AMI_MONITOR_UTILS
#define AMI_MONITOR_UTILS

#include <QObject>


#include "core/CallMonitorManager.h"
#include "core/AmiClient.h"
#include "core/AmiLogger.h"
#include "utils/ICTRequestUtils.h"

#include "../../database/db_connection_pool.h"

class AmiController : public QObject{
    Q_OBJECT

public:
    AmiController() {
        // fetch extentions data
        QJsonArray ext_data = IctApiUtils::getAllowedExtentions();

    }

private:




};


#endif
