#ifndef STATIC_FILE_UTILS_H
#define STATIC_FILE_UTILS_H

#define INI_SETTINGS_PROFILE_PICTURE_PATH_KEY "static/profilePictures"
#define INI_SETTINGS_RAG_FILE_PATH_KEY "static/ragFiles"
#define INI_SETTINGS_ROOT_STATIC_FOLDER_PATH_KEY "files/path"
#include <QString>
class StaticFolderUtils{
public:
    explicit StaticFolderUtils();
    // general functions
    QString GetRootStaticPath();
    bool FileExists(QString& filePath);
    bool RemoveStaticFile(QString& filePath);

    //profile picture functios
    QString GetProfilePicFolderPath();
    QString CreateNewProfilePicPath(QString& username);

    //rag file functions
    QString GetRagFileFolderPath();
    QString CreateNewRagFilePath(QString filename);
    bool RagFileExists(QString path);
private:
    const QString PROFILE_PICTURE_PATH;
    const QString RAG_FILES_PATH;
    QString ROOT_STATIC_PATH ;
};


#endif // STATIC_FILE_UTILS_H
