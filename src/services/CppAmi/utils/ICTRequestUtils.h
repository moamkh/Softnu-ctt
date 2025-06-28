#ifndef ICTREQUESTUTILS_H
#define ICTREQUESTUTILS_H

#include <QJsonArray>


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

    static QJsonArray getAllowedExtentions();
    static QByteArray getLastAudioFile(QString extNum , qint64 epocDate);
    static QByteArray getAudioFileByUniqueId(QString unique_id);
    static QJsonArray getTodaysUserAudioFiles(QString extNum);

    static ApiServer getApiServerCreds();
};

#endif // ICTREQUESTUTILS_H
