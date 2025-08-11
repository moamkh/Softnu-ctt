#ifndef STATIC_FILE_UTILS_H
#define STATIC_FILE_UTILS_H

#define INI_SETTINGS_AUDIO_FILE_PATH_KEY "static/audioFiles"
#define INI_SETTINGS_ROOT_STATIC_FOLDER_PATH_KEY "files/path"

#include <QString>
#include <QByteArray> // Include for QByteArray

class StaticFolderUtils{
public:
    explicit StaticFolderUtils();
    // general functions
    QString GetRootStaticPath();
    bool FileExists(QString& filePath);
    bool RemoveStaticFile(QString& filePath);

    //audio file functions
    QString GetAudioFileFolderPath();
    QString CreateNewAudioFilePath(QString filename);
    bool AudioFileExists(const QString& filename);

    // New functions
    bool CreateAudioFile(const QString& filename, const QByteArray& audioData);
    bool DeleteAudioFile(const QString& filename);
    QByteArray GetAudioFile(const QString& filename);

private:
    const QString AUDIO_FILE_PATH;
    QString ROOT_STATIC_PATH ;
};


#endif // STATIC_FILE_UTILS_H
