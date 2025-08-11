#ifndef ICTREQUESTUTILS_H
#define ICTREQUESTUTILS_H

#include <QJsonArray>
#include <QJsonObject>


class IctApiUtils
{
public:
    struct ApiServer{
        QString url;
        QString token;
        QString tokenPrefix;
        ApiServer(
            QString apiServerUrl,
            QString apiServerToken,
            QString apiServerTokenPrefix);
    };

    struct FileData{
        QByteArray binary;
        QString filename;
        QJsonObject error;
        bool status;

        FileData(QJsonObject jsonError):error(jsonError){
            status = false;
        }
        FileData(QByteArray in_binary,QString in_file_name):binary(in_binary),filename(in_file_name){
            status = true;
        }
    };

    static QJsonArray getAllowedExtentions();
    static QByteArray getLastAudioFile(QString extNum , qint64 epocDate);
    static FileData getAudioFileByUniqueId(QString unique_id);
    static QJsonArray getTodaysUserAudioFiles(QString extNum);

    static ApiServer getApiServerCreds();
};


#endif // ICTREQUESTUTILS_H
