#include "ai_request_manager.h"
#include "ai_request_tasks.h"
#include "../../../global.h"
#include <QException>
#include <QDebug>


AiBackendService::RequestManager::RequestManager(QObject *parent): QThread (parent)
{
    qWarning() << "Initializing Ctt request manager";
    qRegisterMetaType<AiBackendService::RequestPayload>("RequestPayload");
    m_threadPool.setMaxThreadCount(myGlobalSettings->value("aibackend/maxCttConnections").toInt());
    m_stop = false;
    qWarning() << "Initialized Ctt request manager";
}

AiBackendService::RequestManager::~RequestManager()
{
    qDebug() << "Destroying Ctt request manager";
    // 1. Set the stop flag *before* anything else.
    m_stop = true;

    // 2. Wake up the worker thread (if it's waiting).  Do this *before* shutting down the thread pool.
    m_condition.wakeOne();

    // 3. Wait for all tasks to finish in the thread pool *before* terminating.
    m_threadPool.waitForDone();

    // 4. Cleanly terminate the QThread *before* exiting the destructor.
    quit(); // Tell the QThread to exit its event loop.
    wait();  // Wait for the QThread to actually exit.

    qDebug() << "Ctt request manager destroyed";
}

void AiBackendService::RequestManager::run()
{
    qInfo() << "RequestManager thread started";
    while (!m_stop)
    {
        try {
            processQueue();
            m_mutex.lock();
            if (m_queue.isEmpty()) {

                qInfo() << "conversation Queue empty, waiting for new requests";
                m_condition.wait(&m_mutex);
                qInfo() << "queue not empty";
            }
            m_mutex.unlock();
        } catch (const QException& e) {
            qWarning() << "Error in RequestManager run loop:" << e.what();
        }
    }
    qInfo() << "RequestManager thread stopped";
}
void AiBackendService::RequestManager::processQueue()
{
    QMutexLocker locker(&m_mutex);
    while (!m_queue.isEmpty() && m_threadPool.activeThreadCount() < m_threadPool.maxThreadCount())
    {

        try {
            std::pair<bool,RequestPayload> request = m_queue.pop();
#ifdef QT_DEBUG
            qDebug() << "Active threads:" << m_threadPool.activeThreadCount() << "/" << m_threadPool.maxThreadCount();
#endif
            CttPanelRequestTask* task = new CttPanelRequestTask(request.second);
            task->setAutoDelete(true);
            m_threadPool.start(task);

        } catch (const QException& e) {
            qWarning() << "Error processing queue item:" << e.what();
        }
    }
}
void AiBackendService::RequestManager::AddRequestToQueue(AiBackendService::RequestPayload request)
{
    qWarning() << "recieved request from ctt call monitor";
    m_queue.push(request);
    m_condition.wakeOne();
#ifdef QT_DEBUG
    qWarning() << "Request in queue for call "<<request.call_summary_id <<" with unique_id "<<request.unique_id;
#endif
}

