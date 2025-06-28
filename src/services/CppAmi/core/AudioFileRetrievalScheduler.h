#ifndef AUDIOFILERETRIEVALSCHEDULER_H
#define AUDIOFILERETRIEVALSCHEDULER_H

#include <QThread>
#include <QTimer>
#include <QTime>

class RequestManager : public QObject {
    Q_OBJECT

public:
    explicit RequestManager(QObject* parent = nullptr);
    ~RequestManager();

    void startScheduling();
    void setStartTime(const QTime& startTime); // Setter for startTime
    void setEndTime(const QTime& endTime);     // Setter for endTime

private slots:
    void startNightJob();
    void stopNightJob();

private:
    QTimer* m_startTimer;
    QTimer* m_stopTimer;

    void scheduleTimers();

    bool m_isProcessing = false;
    bool m_isScheduled = false;
    bool m_processedToday = false;  // Flag to track daily processing

    QTime m_startTime; // Start time for scheduling
    QTime m_endTime;   // End time for scheduling
};
#endif // AUDIOFILERETRIEVALSCHEDULER_H
