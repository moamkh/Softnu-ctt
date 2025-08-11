#ifndef AI_REQUEST_MANAGER
#define AI_REQUEST_MANAGER

#include <QThread>
#include <QThreadPool>
#include <QWaitCondition>

#include "ai_request_types.h"

namespace AiBackendService {

class RequestManager : public QThread{
    Q_OBJECT
private:
    ThreadSafeRequestQueue m_queue;
    QThreadPool m_threadPool;
    QMutex m_mutex;
    QWaitCondition m_condition;
    bool m_stop = false;

    void processQueue();
public:
    explicit RequestManager(QObject* parent = nullptr);
    ~RequestManager() override;
    void run() override;

public slots:
    void AddRequestToQueue(AiBackendService::RequestPayload request);

};





}

#endif
