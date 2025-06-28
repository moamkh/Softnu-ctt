#include "AudioFileRetrievalScheduler.h"


#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QJsonArray>
#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>

#include "../utils/ICTRequestUtils.h"
#include "../../../global.h"
RequestManager::RequestManager(QObject* parent)
    : QObject(parent) // Default times
{
    int start_time_h = myGlobalSettings->value("ragCategoryBackend/start_time_h",0).toInt();
    int start_time_m = myGlobalSettings->value("ragCategoryBackend/start_time_m",0).toInt();
    int stop_time_h  = myGlobalSettings->value("ragCategoryBackend/end_time_h",0).toInt();
    int stop_time_m  = myGlobalSettings->value("ragCategoryBackend/end_time_m",0).toInt();

    if (start_time_h==0 || stop_time_h==0){
        qFatal("Invalid start/stop time in Novinsettings.ini for RagCategoryBackend. Start time: %d, Stop time: %d", start_time_h, stop_time_h);
    }

    m_startTime = QTime(start_time_h,start_time_m);
    m_endTime = QTime(stop_time_h,stop_time_m);
    qDebug() << "Calculated start time is: " << m_startTime ;
    qDebug() << "Calculated end time is: " << m_endTime ;

    m_startTimer = new QTimer(this);
    m_stopTimer = new QTimer(this);

    connect(m_startTimer, &QTimer::timeout, this, &RequestManager::startNightJob);
    connect(m_stopTimer, &QTimer::timeout, this, &RequestManager::stopNightJob);
}

RequestManager::~RequestManager() {}

void RequestManager::setStartTime(const QTime& startTime) {
    m_startTime = startTime;
}

void RequestManager::setEndTime(const QTime& endTime) {
    m_endTime = endTime;
}

void RequestManager::startScheduling() {
    scheduleTimers();
}

void RequestManager::scheduleTimers() {
    QDateTime nowDateTime = QDateTime::currentDateTime();
    qint64 nowMs = nowDateTime.toMSecsSinceEpoch();

    QDate currentDate = QDate::currentDate();
    QDate nextDate = currentDate.addDays(1);

    // Calculate start and stop times for today
    qint64 todayStartMs = QDateTime(currentDate, m_startTime).toMSecsSinceEpoch();
    qint64 todayStopMs = QDateTime(currentDate, m_endTime).toMSecsSinceEpoch();

    // Adjust for overnight range
    if (m_endTime < m_startTime) {
        todayStopMs += 24 * 60 * 60 * 1000; // Add one day in milliseconds
    }

    // Calculate next day's start and stop times
    qint64 nextStartMs = todayStartMs + 24 * 60 * 60 * 1000;
    qint64 nextStopMs = todayStopMs + 24 * 60 * 60 * 1000;

    if (nowMs >= todayStartMs && nowMs < todayStopMs) {
        // If the current time is within the active range
        qDebug() << "Current time is within the active period. Starting night job in 1 minute.";

        // Schedule start timer for 1 minute later
        qint64 msToStart = 60 * 1000; // 1 minute in milliseconds
        m_startTimer->start(msToStart);

        // Schedule stop timer for the remaining active period
        qint64 msToStop = todayStopMs - nowMs;
        m_stopTimer->start(msToStop);

        qDebug() << "Scheduled start timer in 1 minute.";
        qDebug() << "Scheduled stop timer in ms:" << msToStop << ", epoch:" << todayStopMs;
    } else {
        // Outside of active range, schedule for the next period
        qint64 msToStart = (nowMs < todayStartMs) ? (todayStartMs - nowMs) : (nextStartMs - nowMs);
        m_startTimer->start(msToStart);

        qDebug() << "Scheduled start timer in ms:" << msToStart
                 << ", epoch:" << ((nowMs < todayStartMs) ? todayStartMs : nextStartMs);

        // Schedule stop timer for the next stop time
        qint64 msToStop = (nowMs < todayStopMs) ? (todayStopMs - nowMs) : (nextStopMs - nowMs);
        m_stopTimer->start(msToStop);

        qDebug() << "Scheduled stop timer in ms:" << msToStop
                 << ", epoch:" << ((nowMs < todayStopMs) ? todayStopMs : nextStopMs);
    }

    // Debug output for verification
    qDebug() << "Current epoch time:" << nowMs << " (" << nowDateTime.toString() << ")";
    qDebug() << "Today's start epoch:" << todayStartMs << " (" << QDateTime::fromMSecsSinceEpoch(todayStartMs).toString() << ")";
    qDebug() << "Today's stop epoch:" << todayStopMs << " (" << QDateTime::fromMSecsSinceEpoch(todayStopMs).toString() << ")";
    qDebug() << "Next start epoch:" << nextStartMs << " (" << QDateTime::fromMSecsSinceEpoch(nextStartMs).toString() << ")";
    qDebug() << "Next stop epoch:" << nextStopMs << " (" << QDateTime::fromMSecsSinceEpoch(nextStopMs).toString() << ")";
}

void RequestManager::startNightJob()
{
    // Check if already processed today
    if (m_processedToday) {
        qDebug() << "Already processed today. Skipping...";
        return;
    }

    qDebug() << "Starting night job processing...";
    m_isProcessing = true;
    m_isScheduled = false;

    // fetch todays audio calls
    DbResault dbRes = myDbController->getTodaysCallSummariesWithEmptyAiResponse();

    if(!dbRes.status){qWarning()<<"Failed to fetch the data";}
    QJsonArray todaysCalls = dbRes.data.value("call_summaries").toArray();

    // create a distinct list of extention numbers
    QStringList todaysActiveExts ;
    for(auto call : todaysCalls){
        QString extNo = call.toObject().value("extension_number").toString();
        if (!todaysActiveExts.contains(extNo)){
            todaysActiveExts.append(extNo);
        }
    }

    QMap<QString,QJsonArray> extFileNames;
    for(QString extNo : todaysActiveExts){
        // array of file_name and unique_id
        extFileNames.insert(extNo,IctApiUtils::getTodaysUserAudioFiles(extNo));
    }





}

void RequestManager::stopNightJob() {
    qWarning() << "Stopped night-time job";
    m_isProcessing = false;

    // Reset the flag for tomorrow
    m_processedToday = false;
    qDebug() << "Reset processedToday flag for next day";

    // Calculate the next day's start and stop times
    QDateTime nowDateTime = QDateTime::currentDateTime();
    QDate currentDate = nowDateTime.date();

    qint64 nextStartMs = QDateTime(currentDate.addDays(1), m_startTime).toMSecsSinceEpoch();
    qint64 nextStopMs = QDateTime(currentDate.addDays(1), m_endTime).toMSecsSinceEpoch();

    // Schedule the timers for the next day's start and stop
    qint64 msToNextStart = nextStartMs - nowDateTime.toMSecsSinceEpoch();
    qint64 msToNextStop = nextStopMs - nowDateTime.toMSecsSinceEpoch();

    m_startTimer->start(msToNextStart);
    m_stopTimer->start(msToNextStop);

    // Debug outputs for clarity
    qDebug() << "Scheduled next day's start time in ms:" << msToNextStart << " at epoch:" << nextStartMs;
    qDebug() << "Scheduled next day's stop time in ms:" << msToNextStop << " at epoch:" << nextStopMs;
}

