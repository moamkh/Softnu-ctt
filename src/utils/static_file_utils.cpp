#include "static_file_utils.h"


#include "../global.h"

#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QCryptographicHash>
#include <QRandomGenerator>
#include <QDir>
// Function to set full permissions for a file or directory
bool setFullPermissions(const QString &path) {
    QFile file(path);

    // Set full permissions (read, write, execute for owner, group, and others)
    if (file.setPermissions(QFile::ReadOwner | QFile::WriteOwner | QFile::ExeOwner |
                            QFile::ReadGroup | QFile::WriteGroup | QFile::ExeGroup |
                            QFile::ReadOther | QFile::WriteOther | QFile::ExeOther)) {
        qDebug() << "Full permissions set successfully for:" << path;
        return true;
    } else {
        qWarning() << "Failed to set permissions for:" << path;
        return false;
    }
}

// Recursive function to set permissions for a directory and all its contents
bool setPermissionsRecursive(const QString &dirPath) {
    QDir dir(dirPath);

    // Set permissions for the directory itself
    if (!setFullPermissions(dirPath)) {
        return false;
    }

    // Iterate over all files and subdirectories in the directory
    QFileInfoList entries = dir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
    for (const QFileInfo &entry : entries) {
        QString entryPath = entry.absoluteFilePath();

        if (entry.isDir()) {
            // Recursively set permissions for subdirectories
            if (!setPermissionsRecursive(entryPath)) {
                return false;
            }
        } else {
            // Set permissions for files
            if (!setFullPermissions(entryPath)) {
                return false;
            }
        }
    }

    return true;
}
StaticFolderUtils::StaticFolderUtils()
    :PROFILE_PICTURE_PATH(myGlobalSettings->value(INI_SETTINGS_PROFILE_PICTURE_PATH_KEY).toString()),
    RAG_FILES_PATH(myGlobalSettings->value(INI_SETTINGS_RAG_FILE_PATH_KEY).toString()){

    qInfo() << "Initializing Statitc Folder Utils.";

    QFileInfo configFile(myGlobalSettings->fileName());
    ROOT_STATIC_PATH = configFile.absolutePath();

    if (setPermissionsRecursive(ROOT_STATIC_PATH)) {
        qDebug() << "Permissions set successfully for directory and all its contents.";
    } else {
        qWarning() << "Failed to set permissions for directory and its contents.";
    }
}

QString StaticFolderUtils::GetRootStaticPath()
{
    QString staticDirPath = ROOT_STATIC_PATH;
    QString baseDir;
    if (staticDirPath.endsWith('/')) {
        staticDirPath.chop(1);
    }

    if(staticDirPath.startsWith("../")){
        staticDirPath = staticDirPath.mid(2);
        baseDir = QCoreApplication::applicationDirPath();
        baseDir.append(staticDirPath);
    }
    else if(staticDirPath.startsWith("/")){
        baseDir = staticDirPath;
        qWarning() <<baseDir;
    }
    else{
        qWarning()<<"The static folder path is invalid format";
    }
    return baseDir;
}

bool StaticFolderUtils::FileExists(QString& filePath)
{
    QFileInfo  file(filePath);
    return file.exists() && file.isFile();
}

bool StaticFolderUtils::RemoveStaticFile(QString &filePath)
{
    QFile file(filePath);

    // Check if the file exists
    if (!file.exists()) {
        qWarning() << "File" << filePath << "does not exist. Cannot remove.";
        return false;  // Return false if the file does not exist
    }

    // Attempt to remove the file
    if (!file.remove()) {
        qWarning() << "Failed to remove file" << filePath << ":" << file.errorString();
        return false;  // Return false if the file could not be removed
    }

    // If the file was successfully removed
#ifdef QT_DEBUG
    qDebug() << "File" << filePath << "was successfully removed.";
#endif

    return true;  // Return true if the file was successfully removed
}


QString StaticFolderUtils::GetProfilePicFolderPath()
{
    QString  profilePicFolderPath = StaticFolderUtils::GetRootStaticPath();
    profilePicFolderPath.append(PROFILE_PICTURE_PATH);
    // Return the full path to the image file
    return profilePicFolderPath;
}

QString StaticFolderUtils::CreateNewProfilePicPath(QString& username)
{
    // Generate a random salt
    QByteArray salt;
    int saltLength = 16 ;
    salt.resize(saltLength);
    for (int i = 0; i < saltLength; ++i) {
        salt[i] = static_cast<char>(QRandomGenerator::global()->bounded(256));
    }

    // Append the salt to the input string
    QByteArray saltedUsername = username.toUtf8() + salt;

    // Create a QCryptographicHash object with the specified algorithm
    QCryptographicHash hash(QCryptographicHash::Md5);

    // Add the salted input data to be hashed
    hash.addData(saltedUsername);

    // Retrieve the result as a QByteArray and Convert the hash result to a hexadecimal string
    QString _hashed_salted_username = hash.result().toHex();

    return GetProfilePicFolderPath().append("/"+_hashed_salted_username);
}

QString StaticFolderUtils::GetRagFileFolderPath()
{
    QString  ragFilePath = StaticFolderUtils::GetRootStaticPath();
    ragFilePath.append(RAG_FILES_PATH);
    // Return the full path to the image file
    return ragFilePath;
}

QString StaticFolderUtils::CreateNewRagFilePath(QString filename)
{
    // Generate a random salt
    QByteArray salt;
    int saltLength = 16 ;
    salt.resize(saltLength);
    for (int i = 0; i < saltLength; ++i) {
        salt[i] = static_cast<char>(QRandomGenerator::global()->bounded(256));
    }

    // Append the salt to the input string
    QByteArray saltedFilename = filename.toUtf8() + salt;

    // Create a QCryptographicHash object with the specified algorithm
    QCryptographicHash hash(QCryptographicHash::Md5);

    // Add the salted input data to be hashed
    hash.addData(saltedFilename);

    // Retrieve the result as a QByteArray and Convert the hash result to a hexadecimal string
    QString _hashed_salted_fileName = hash.result().toHex();

    return GetRagFileFolderPath().append("/"+_hashed_salted_fileName);
}

bool StaticFolderUtils::RagFileExists(QString path)
{
    QString fullPath = GetRootStaticPath().append(path);
    return FileExists(fullPath);
}
