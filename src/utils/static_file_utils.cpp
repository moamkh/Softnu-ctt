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
    :AUDIO_FILE_PATH(myGlobalSettings->value(INI_SETTINGS_AUDIO_FILE_PATH_KEY).toString()){

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

QString StaticFolderUtils::GetAudioFileFolderPath()
{
    // Get the root path
    QString rootPath = GetRootStaticPath();

    // The AUDIO_FILE_PATH from the settings gives the subdirectory
    // We use QDir to safely join the paths
    QDir dir(rootPath);
    QString fullAudioPath = dir.filePath(AUDIO_FILE_PATH);

    // Ensure the audio directory exists, create it if not.
    QDir audioDir(fullAudioPath);
    if (!audioDir.exists()) {
        qDebug() << "Audio directory does not exist. Creating at:" << fullAudioPath;
        if (!audioDir.mkpath(".")) {
            qWarning() << "Failed to create audio directory path:" << fullAudioPath;
            return QString(); // Return empty string on failure
        }
    }
    return fullAudioPath;
}


QString StaticFolderUtils::CreateNewAudioFilePath(QString filename)
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

    QDir audioDir(GetAudioFileFolderPath());
    return audioDir.filePath(_hashed_salted_fileName);
}

bool StaticFolderUtils::AudioFileExists(const QString& filename)
{
    QDir audioDir(GetAudioFileFolderPath());
    QString fullPath = audioDir.filePath(filename);
    return FileExists(fullPath);
}

// --- NEW FUNCTIONS ---

/**
 * @brief Creates a new audio file with the given name and content in the audio folder.
 * @param filename The name of the file to create (e.g., "my_sound.wav").
 * @param audioData The binary data of the audio file.
 * @return True if the file was created successfully, false otherwise.
 */
bool StaticFolderUtils::CreateAudioFile(const QString &filename, const QByteArray &audioData)
{
    QString audioFolderPath = GetAudioFileFolderPath();

    if (audioFolderPath.isEmpty()) {
        qWarning() << "Could not create audio file, audio folder path is invalid.";
        return false;
    }

    QDir audioDir(audioFolderPath);
    QString fullPath = audioDir.filePath(filename);

    QFile file(fullPath);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to open file for writing:" << fullPath << "Error:" << file.errorString();
        return false;
    }

    qint64 bytesWritten = file.write(audioData);
    file.close();

    if (bytesWritten != audioData.size()) {
        qWarning() << "Failed to write all data to file:" << fullPath << ". Wrote" << bytesWritten << "of" << audioData.size();
        // Clean up partially written file
        RemoveStaticFile(fullPath);
        return false;
    }

    qDebug() << "Successfully created audio file:" << fullPath;
    return true;
}

/**
 * @brief Deletes an audio file from the audio folder.
 * @param filename The name of the file to delete.
 * @return True if the file was deleted successfully, false otherwise.
 */
bool StaticFolderUtils::DeleteAudioFile(const QString &filename)
{
    QString audioFolderPath = GetAudioFileFolderPath();
    if (audioFolderPath.isEmpty()) {
        qWarning() << "Could not delete audio file, audio folder path is invalid.";
        return false;
    }

    QDir audioDir(audioFolderPath);
    QString fullPath = audioDir.filePath(filename);

    return RemoveStaticFile(fullPath);
}

/**
 * @brief Retrieves the content of an audio file.
 * @param filename The name of the file to read.
 * @return A QByteArray with the file's content. Returns an empty QByteArray if the file
 *         cannot be found or read.
 */
QByteArray StaticFolderUtils::GetAudioFile(const QString &filename)
{
    QString audioFolderPath = GetAudioFileFolderPath();
    if (audioFolderPath.isEmpty()) {
        qWarning() << "Could not get audio file, audio folder path is invalid.";
        return QByteArray();
    }

    QDir audioDir(audioFolderPath);
    QString fullPath = audioDir.filePath(filename);

    if (!FileExists(fullPath)) {
        qWarning() << "Audio file does not exist:" << fullPath;
        return QByteArray(); // Return empty array
    }

    QFile file(fullPath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open audio file for reading:" << fullPath << "Error:" << file.errorString();
        return QByteArray(); // Return empty array
    }

    QByteArray audioData = file.readAll();
    file.close();

    qDebug() << "Successfully read" << audioData.size() << "bytes from audio file:" << fullPath;
    return audioData;
}
