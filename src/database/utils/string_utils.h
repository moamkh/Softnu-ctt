#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <QString>
#include <QChar>
#include <QVector>
class DBStringUtils{
public:
    static QString CleanString(const QString& text);
    static QString CleanString(QString& text);
    static QString IntArrayToDbString(QVector<qint64>& int_array);
    static QStringList CleanDbStringList(QString strList);
    static QString cleanJsonString(const QString &input);
};

#endif
