#ifndef AI_REQUEST_TASKS
#define AI_REQUEST_TASKS
#include <QObject>
#include <QRunnable>
#include <QJsonObject>
#include <QJsonArray>
#include "ai_request_types.h"

namespace AiBackendService {

class CttPanelRequestTask :
        public QObject,
        public QRunnable
{
    Q_OBJECT

    RequestPayload m_request;

public:
    CttPanelRequestTask(RequestPayload reqPayload,QObject* parent = nullptr);
    ~CttPanelRequestTask() override;
    void run() override;

};
}

#endif
