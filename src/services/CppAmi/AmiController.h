#ifndef AMI_MONITOR_UTILS
#define AMI_MONITOR_UTILS

#include <QObject>


#include "core/CallMonitorManager.h"
#include "core/AmiClient.h"
#include "core/AmiLogger.h"

#include "utils/ICTRequestUtils.h"
#include "utils/AmiDbUtils.h"

#include "../../database/db_connection_pool.h"


class AmiController  : public QObject{
    Q_OBJECT

public:
    AmiController();

private:
    void check_users();
    void initialize_ami_monitor_instances();
    CallMonitorManager* m_callMonitorManager;
    AmiClient* m_amiClient;
};


#endif
