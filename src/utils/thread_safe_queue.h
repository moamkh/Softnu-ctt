#ifndef THREAD_SAFE_QUEUE_H
#define THREAD_SAFE_QUEUE_H

#include <queue>
#include <QMutex>
#include <QObject>

//class ThreadSafeQueueBase : public QObject {
//    Q_OBJECT

//public:
//    ThreadSafeQueueBase(QObject *parent = nullptr) : QObject(parent) {}

//signals:
//    void itemAdded();
//    void itemRemoved();
//};

//template<typename T>
//class ThreadSafeQueue : public ThreadSafeQueueBase {
//public:
//    ThreadSafeQueue(QObject *parent = nullptr) : ThreadSafeQueueBase(parent) {}

//    void push(const T &item) {
//        QMutexLocker locker(&m_mutex);
//        m_queue.push(item);
//        locker.unlock();  // Unlock before emitting signal
//        emit itemAdded();
//    }
//    T front(){
//        return m_queue.front();
//    }
//    std::pair<bool,T> pop(T item) {
//        QMutexLocker locker(&m_mutex);
//        if (!m_queue.empty()) {
//            item = std::move(m_queue.front());
//            m_queue.pop();
//            locker.unlock();  // Unlock before emitting signal
//            emit itemRemoved();
//            return std::make_pair(true,item);
//        }
//        return std::make_pair(false,item);
//    }

//    bool isEmpty() const {
//        QMutexLocker locker(&m_mutex);
//        return m_queue.empty();
//    }

//private:
//    std::queue<T> m_queue;
//    mutable QMutex m_mutex;
//};

#endif // THREAD_SAFE_QUEUE_H
