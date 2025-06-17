#include "string_utils.h"
#include <QRegularExpression>
#include <QTextCodec>
#include <QDebug>
QString DBStringUtils::CleanString(const QString &text)
{
    QString text2 = text;
    return text2.replace("\'","\"");
}
QString DBStringUtils::CleanString(QString &text)
{
    return text.replace("\'","\"");
}

QString DBStringUtils::IntArrayToDbString(QVector<qint64> &int_array)
{
    QString res = "";
    for (auto num : int_array) {
        res.append(QString::number(num));
        res.append(",");
    }
    res.chop(1);
    return res;

}

QStringList DBStringUtils::CleanDbStringList(QString strList)
{
    return strList.remove('{').remove('}').remove('"').remove('/').split(',');
}

QString DBStringUtils::cleanJsonString(const QString &input) {
    // 1. Remove any unwanted special characters or non-UTF-8 chars (if any)
    QString cleaned = input;

    // Remove control characters (ASCII characters below 32 and above 126, including non-printable ones)
    cleaned.remove(QRegularExpression("[\\x00-\\x1F\\x7F]"));

    // 2. Handle any unwanted escape characters (optional: remove the \u escape sequences)
    cleaned = cleaned.replace(QRegularExpression(R"(\\u[0-9A-Fa-f]{4})"), "");

    // 3. Ensure proper UTF-8 encoding
    if (!QTextCodec::codecForName("UTF-8")->canEncode(cleaned)) {
        qWarning() << "The string cannot be encoded in UTF-8.";
        cleaned = QTextCodec::codecForName("UTF-8")->fromUnicode(cleaned).data();
    }

    // 4. Optionally trim leading and trailing whitespaces
    cleaned = cleaned.trimmed();

    // 5. If needed, escape quotes and special characters for safe database storage
    cleaned = cleaned.replace("\"", "\\\"");

    return cleaned;
}
