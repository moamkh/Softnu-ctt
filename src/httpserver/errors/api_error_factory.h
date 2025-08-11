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
    static BaseApiError BadRequest(QString msg=QLatin1String(""));

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

    // Call Summary errors
    static BaseApiError FailedToDownloadAudioFile();


};




#endif
