#ifndef AI_REQUEST_TYPES
#define AI_REQUEST_TYPES

#include <QObject>
#include <QJsonObject>
#include <QJsonArray>
#include <QMutex>
#include <QMetaType>
#include <queue>

namespace AiBackendService {

struct RequestPayload{
    QString unique_id;
    qint64 call_summary_id;

    RequestPayload(){}

    RequestPayload(
        QString in_unique_id,
        qint64 in_call_summary_id
        ):unique_id(in_unique_id),call_summary_id(in_call_summary_id){}
};

class ThreadSafeRequestQueue{
    std::queue<RequestPayload> m_queue;
    mutable QMutex m_mutex;
public:
    ThreadSafeRequestQueue() = default;
    void push(const RequestPayload &item){
        QMutexLocker locker(&m_mutex);
        m_queue.push(item);
    }

    RequestPayload front(){
        return m_queue.front();
    }

    std::pair<bool,RequestPayload> pop(){
        QMutexLocker locker(&m_mutex);
        RequestPayload item;
        if (!m_queue.empty()) {
            item = std::move(m_queue.front());
            m_queue.pop();
            locker.unlock();  // Unlock before emitting signal
            return std::make_pair(true,item);
        }
        return std::make_pair(false,item);
    }

    bool isEmpty() const{
        QMutexLocker locker(&m_mutex);
        return m_queue.empty();
    }
};



}

#endif
