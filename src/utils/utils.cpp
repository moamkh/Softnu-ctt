
//#include "utils.h"
//#include "default_settings.h"
//#include <QDebug>
//#include <QSet>
//#include <QList>
//#include <QByteArray>
//#include <QCryptographicHash>
////#include <QRandomGenerator>
//#include <QCoreApplication>
//#include <QDir>
//#include <QDateTime>

//#include "jwt-cpp/jwt.h"
//#include "../global.h"

///*
// * Searches the project folder to find the settings.ini
// *      Note: in our case Novinsettings.ini
//*/


//void staticFolderSetup(const QString &baseDir) {
//    // Ensure the base directory does not end with a slash
//    QString cleanBaseDir = baseDir;
//    if (cleanBaseDir.endsWith('/')) {
//        cleanBaseDir.chop(1);
//    }

//    // List of directories to be checked/created
//    QStringList directories = {
//        cleanBaseDir,
//        cleanBaseDir + "/files",
//        cleanBaseDir + "/files/static",
//        cleanBaseDir + "/files/static/profilePics",
//        cleanBaseDir + "/files/static/docs"
//    };

//    // Iterate over each directory in the list
//    for (const QString &dirPath : directories) {
//        QDir dir(dirPath);
//        if (!dir.exists()) {
//            // Attempt to create the directory and check for success
//            if (dir.mkpath(dirPath)) {
//                qDebug() << "Created directory:" << dirPath;
//            } else {
//                qWarning() << "Failed to create directory:" << dirPath;
//            }
//        } else {
//            qDebug() << "Directory already exists:" << dirPath;
//        }
//    }
//}
//QString getStaticFilePath()
//{
//    QString staticDirPath = myGlobalSettings->value("files/path").toString();
//    if (staticDirPath.endsWith('/')) {
//        staticDirPath.chop(1);
//    }
//    QString baseDir;
//    if(staticDirPath.startsWith("../")){
//        staticDirPath = staticDirPath.mid(2);
//        baseDir = QCoreApplication::applicationDirPath();
//        baseDir.append(staticDirPath);
//    }
//    else if(staticDirPath.startsWith("/")){
//        baseDir = staticDirPath;
//    }
//    else{
//        qWarning()<<"The static folder path is invalid format";
//    }
//    return baseDir;
//}
//QString searchConfigFile(QString fn="settings.ini") {
//    QString binDir=QCoreApplication::applicationDirPath();
//    QString appName=QCoreApplication::applicationName();
//    QString fileName(fn);

//    QStringList searchList;
//    searchList.append(binDir);
//    searchList.append(binDir+"/etc");
//    searchList.append(binDir+"/../etc");
//    searchList.append(binDir+"/../"+appName+"/etc");     // for development with shadow build (Linux)
//    searchList.append(binDir+"/../../"+appName+"/etc");  // for development with shadow build (Windows)
//    searchList.append(QDir::rootPath()+"etc/opt");
//    searchList.append(QDir::rootPath()+"etc");

//    foreach (QString dir, searchList)
//    {
//        QFile file(dir+"/"+fileName);
//        if (file.exists())
//        {
//            fileName=QDir(file.fileName()).canonicalPath();
//            qInfo("Using config file %s",qPrintable(fileName));
//            return fileName;
//        }
//    }

//    // not found
//    foreach (QString dir, searchList)
//    {
//        qWarning("%s/%s not found",qPrintable(dir),qPrintable(fileName));
//    }
//    qFatal("Cannot find config file %s",qPrintable(fileName));
//    return nullptr;
//}
//QString hashString(const QString& input, QCryptographicHash::Algorithm algorithm ) {
//    // Convert the input string to a QByteArray
//    QByteArray byteArray = input.toUtf8();

//    // Create a QCryptographicHash object with the specified algorithm
//    QCryptographicHash hash(algorithm);

//    // Add the data to be hashed
//    hash.addData(byteArray);

//    // Retrieve the result as a QByteArray
//    QByteArray hashResult = hash.result();

//    // Convert the hash result to a hexadecimal string
//    return QString(hashResult.toHex());
//}

//// Function to generate a random salt
//QByteArray generateRandomSalt(int length = 16) {
//    QByteArray salt;
//    salt.resize(length);
//    for (int i = 0; i < length; ++i) {
//        salt[i] = static_cast<char>(QRandomGenerator::global()->bounded(256));
//    }
//    return salt;
//}

//// Function to hash a string with a random salt
//QString hashStringWithSalt(const QString& input, QCryptographicHash::Algorithm algorithm=QCryptographicHash::Sha256) {
//    // Generate a random salt
//    QByteArray salt = generateRandomSalt();

//    // Append the salt to the input string
//    QByteArray saltedInput = input.toUtf8() + salt;

//    // Create a QCryptographicHash object with the specified algorithm
//    QCryptographicHash hash(algorithm);

//    // Add the salted input data to be hashed
//    hash.addData(saltedInput);

//    // Retrieve the result as a QByteArray
//    QByteArray hashResult = hash.result();

//    // Convert the hash result to a hexadecimal string
//    return QString(hashResult.toHex());
//}



//QString validateTokenGetUsername(const QByteArray &token)
//{
//    QString strToken = token;
//    QString res="";

//    if (token.isEmpty()){return res;}
//    if (!strToken.contains("Bearer")){return res;}

//    QStringList parts = strToken.split(" ");
//    if(parts[0].isEmpty()){return res;}
//    auto verifyier = jwt::verify()
//            .allow_algorithm(jwt::algorithm::hs256(myJWTDefaults->JWT_SECRET))
//            .with_issuer(myJWTDefaults->JWT_ISSUER);
//    auto decoded = jwt::decode(parts[1].toStdString());
//    try
//    {

//        verifyier.verify(decoded);
//        for (auto& e : decoded.get_payload_json()) {
//            if (e.first == myJWTDefaults->JWT_USERNAME_KEY) {

//                res = QString::fromStdString(e.second.serialize()).remove("\"");
//            }
//          }
//        return res;
//    }catch(const std::exception& e)
//    {
//        return res;
//    }
//}
//qint64 validateTokenGetUserID(const QByteArray &token){

//    QString strToken = token;

//    qint64 res=0;
//    QString resStr;

//    if (token.isEmpty()){return res;}
//    if (!strToken.contains(QString::fromStdString(myJWTDefaults->JWT_STARTER_PREFIX))){return res;}

//    QStringList parts = strToken.split(" ");

//    if(parts[0].isEmpty()){return res;}
//    try
//    {
//    auto verifyier = jwt::verify()
//            .allow_algorithm(jwt::algorithm::hs256(myJWTDefaults->JWT_SECRET))
//            .with_issuer(myJWTDefaults->JWT_ISSUER);

//    auto decoded = jwt::decode(parts[1].toStdString());

//    verifyier.verify(decoded);

//    for (auto& e : decoded.get_payload_json()) {
//        if (e.first == myJWTDefaults->JWT_USER_ID_KEY) {

//            resStr = QString::fromStdString(e.second.serialize());
//        }
//      }
//    return resStr.remove('"').toInt();

//    }catch(const std::exception& e)
//    {
//        qDebug()<<e.what();
//        return res;
//    }

//}
//QString generateToken(const QString& username,const int& userID)
//{
//    qint64 tokenExpirationEpoc =
//            QDateTime::currentDateTime()
//            .addSecs(myJWTDefaults->JWT_DEFAULT_TOKEN_LIFE_SECONDS)
//            .toSecsSinceEpoch();
//    std::string token = jwt::create()
//            .set_issuer(myJWTDefaults->JWT_ISSUER)
//            .set_type(myJWTDefaults->JWT_TYPE)
//            .set_payload_claim(myJWTDefaults->JWT_USER_ID_KEY,jwt::claim(std::to_string(userID)))
//            .set_payload_claim(myJWTDefaults->JWT_USERNAME_KEY,jwt::claim(username.toStdString()))
//            .set_expires_at(jwt::date(std::chrono::duration<int>(tokenExpirationEpoc)))
//            .sign(jwt::algorithm::hs256(myJWTDefaults->JWT_SECRET));
//    auto decoded = jwt::decode(token);
//    for(auto& e : decoded.get_payload_json())
//            std::cout << e.first << " = " << e.second.serialize() << std::endl;
//    return QString::fromStdString(token);
//}
//void setDefaultCorsHeaders(HttpResponse &response)
//{
//    response.setHeader("Access-Control-Allow-Origin","*");
//    response.setHeader("Access-Control-Allow-Method","GET,POST,OPTIONS,PUT");
//    response.setHeader("Access-Control-Allow-Headers","Content-Type,Authorization");
//}



