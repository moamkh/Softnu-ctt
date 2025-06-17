#ifndef API_ERROR_FACTORY_H
#define API_ERROR_FACTORY_H
#include "base_api_error.h"

class ApiErrorFactory{

public:
    // Generic Errors
    static BaseApiError InternalServerError();
    static BaseApiError NotImplementedError();
    static BaseApiError PathNotFoundError();
    static BaseApiError InvalidMethodError();
    static BaseApiError BadRequest();

    // Request headers error
    static BaseApiError AuthorizationHeaderRequired();

    // Json Validator Errors
    static BaseApiError NoJsonSchemaError();
    static BaseApiError InvalidJsonSchemaFormatError();
    static BaseApiError MissingJsonFieldError(QString field);
    static BaseApiError JsonFieldMismatchError(QString field);

    // Active directory error
    static BaseApiError ActiveDirectoryConnectionFailed();

    // Authentication errors
    static BaseApiError InvalidCredentialsProvided();
    static BaseApiError InvalidTokenProvided();

    // Dashboard errors
    static BaseApiError FailedToUpdatedDashboard();
    static BaseApiError FailedToUpdatedProfilePicture();
    static BaseApiError FailedToFetchProfilePhotoPath();
    static BaseApiError FailedToRemoveExistingProfilePictureFile();
    static BaseApiError FailedToSaveProfilePicture();

    // Conversation errors
    static BaseApiError FailedToFetchConversationList();
    static BaseApiError FailedToFetchConversatinMessageList();
    static BaseApiError FailedToFetchSearchedConversations();
    static BaseApiError FailedToFetchMessageFeedBack();
    static BaseApiError FailedToSubmitUserFeedback();
    static BaseApiError InvalidConversationID();
    static BaseApiError InvalidAiMessageID();
    static BaseApiError InvalidValuesForFeedBack();

    //permission errors
    static BaseApiError UnAuthorizedAccess(QString permission);

    //rag file errors
    static BaseApiError DuplicateRagFileName(QString name="");
    static BaseApiError FailedToFindRagFile(QString name);
    static BaseApiError FailedToFetchRagFileList();
    static BaseApiError InvalidRagCategoryId();
    static BaseApiError RagCategoryNameAlreadyExists();
    static BaseApiError InvalidRagId();
    static BaseApiError InvalidRagType();

    //role errors
    static BaseApiError DuplicateRoleName(QString name);
    static BaseApiError InvalidRoleName();
    static BaseApiError FailedToDeleteRole();
    static BaseApiError FailedToUpdateRole();
    static BaseApiError FailedToValidatePermAndRole(int dbErrCode);
    static BaseApiError FailedToValidateUserManagableRoles(int dbErrCode);
    static BaseApiError FailedToUpdateManagableRole(int dbErrCode);
    static BaseApiError FailedToFetchManagableRoles();
    static BaseApiError FailedToFetchManagableUsers();
    static BaseApiError FailedToUpdateUserRole(int dbErrCode);
    static BaseApiError FailedToValidatePermAndUser(int dbErrCode);
    //bot errors
    static BaseApiError InvalidBot();
    static BaseApiError FailedToFetchManagableBots();
    static BaseApiError BotNameAlreadyExists();


};




#endif
