#include "api_error_factory.h"

BaseApiError ApiErrorFactory::InternalServerError()
{
    return BaseApiError(
        "An unexpected error occurred on the server.",
        "خطای غیر منتظره ای در سرور رخ داد.",
        "Une erreur inattendue est survenue sur le serveur.",
        "حدث خطأ غير متوقع على الخادم.",
        500,
        1000
        );
}

BaseApiError ApiErrorFactory::NotImplementedError()
{
    return BaseApiError(
        "This feature is not yet implemented.",
        "این قابلیت هنوز پیاده سازی نشده است.",
        "Cette fonctionnalité n'est pas encore implémentée.",
         "هذه الميزة لم يتم تنفيذها بعد.",
        501,
        1001
        );
}


BaseApiError ApiErrorFactory::PathNotFoundError()
{
    return BaseApiError(
        "The requested resource was not found.",
        "منبع درخواستی یافت نشد.",
        "La ressource demandée n'a pas été trouvée.",
         "المورد المطلوب غير موجود.",
        404,
        1002
    );
}

BaseApiError ApiErrorFactory::NoJsonSchemaError()
{
    return BaseApiError(
        "Server configuration error: No JSON schema defined.",
        "خطای پیکربندی سرور: هیچ طرحواره JSON تعریف نشده است.",
        "Erreur de configuration du serveur : aucun schéma JSON défini.",
         "خطأ في تكوين الخادم: لم يتم تعريف مخطط JSON.",
        500,
        1003
       );
}


BaseApiError ApiErrorFactory::InvalidJsonSchemaFormatError()
{
    return BaseApiError(
        "Server configuration error: Invalid JSON schema format.",
        "خطای پیکربندی سرور: قالب طرحواره JSON نامعتبر است.",
        "Erreur de configuration du serveur : format de schéma JSON non valide.",
         "خطأ في تكوين الخادم: تنسيق مخطط JSON غير صالح.",
        500,
        1004
        );
}

BaseApiError ApiErrorFactory::MissingJsonFieldError(QString field)
{
    return BaseApiError(
        "The required field '" + field + "' is missing in the request.",
        "فیلد اجباری '" + field + "' در درخواست ارسالی وجود ندارد.",
        "Le champ obligatoire '" + field + "' est manquant dans la requête.",
         "الحقل المطلوب '" + field + "' مفقود في الطلب.",
        400,
        1005
        );
}



BaseApiError ApiErrorFactory::JsonFieldMismatchError(QString field)
{
    return BaseApiError(
         "Invalid data type for field '" + field + "'.",
        "نوع داده ارسالی برای فیلد '" + field + "' معتبر نیست.",
        "Type de données non valide pour le champ '" + field + "'.",
        "نوع بيانات غير صالح للحقل '" + field + "'.",
        400,
        1006
        );
}

BaseApiError ApiErrorFactory::ActiveDirectoryConnectionFailed()
{
    return BaseApiError(
        "Failed to connect to Active Directory.",
        "اتصال به اکتیو دایرکتوری ناموفق بود.",
        "Échec de la connexion à Active Directory.",
         "فشل الاتصال بـ Active Directory.",
        500,
        1007
       );
}

BaseApiError ApiErrorFactory::InvalidCredentialsProvided()
{
    return BaseApiError(
        "Invalid username or password.",
        "نام کاربری یا رمز عبور معتبر نیست.",
        "Nom d'utilisateur ou mot de passe invalide.",
         "اسم المستخدم أو كلمة المرور غير صالحة.",
        401,
        1008
        );
}

BaseApiError ApiErrorFactory::InvalidTokenProvided()
{
    return BaseApiError(
        "Invalid or expired authentication token.",
        "توکن احراز هویت نامعتبر یا منقضی شده است.",
        "Jeton d'authentification invalide ou expiré.",
         "رمز المصادقة غير صالح أو منتهي الصلاحية.",
        401,
        1009
        );
}

BaseApiError ApiErrorFactory::FailedToDownloadAudioFile()
{
    return BaseApiError(
        "Failed to download audio file.",
        "دانلود فایل صوتی با خطا مواجه شد.",
        "Échec du téléchargement du fichier audio.",
        "فشل في تنزيل ملف الصوت.",
        500,
        1010
        );
}

BaseApiError ApiErrorFactory::InvalidMethodError()
{
    return BaseApiError(
        "This request method is not allowed for this resource.",
        "متد درخواستی برای این منبع مجاز نیست.",
        "Cette méthode de requête n'est pas autorisée pour cette ressource.",
         "طريقة الطلب هذه غير مسموح بها لهذا المورد.",
        405,
        1023
                );
}



BaseApiError ApiErrorFactory::AuthorizationHeaderRequired()
{
    return BaseApiError(
        "Authorization header is required for this request.",
        "هدر احراز هویت برای این درخواست الزامی است.",
        "L'en-tête d'autorisation est requis pour cette requête.",
         "عنوان التفويض مطلوب لهذا الطلب.",
        401,
        1024
       );
}
BaseApiError ApiErrorFactory::BadRequest(QString msg)
{
    if(msg.isEmpty()){
        return BaseApiError(
            "Bad request.",
            "درخواست نامعتبر.",
            "Requête invalide.",
            "طلب غير صالح.",
            400,
            1055
            );
    }else{
        return BaseApiError(
            msg,
            "درخواست نامعتبر.",
            "Requête invalide.",
            "طلب غير صالح.",
            400,
            1055
            );
    }

}
