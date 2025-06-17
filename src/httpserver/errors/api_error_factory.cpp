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

BaseApiError ApiErrorFactory::FailedToUpdatedDashboard()
{
    return BaseApiError(
       "Could not update the dashboard data.",
        "به روزرسانی داشبورد با مشکل مواجه شد.",
        "Impossible de mettre à jour les données du tableau de bord.",
         "تعذر تحديث بيانات لوحة المعلومات.",
        400,
        1010
        );
}

BaseApiError ApiErrorFactory::FailedToUpdatedProfilePicture()
{
    return BaseApiError(
        "Could not update the profile picture.",
        "به روزرسانی عکس پروفایل با مشکل مواجه شد.",
        "Impossible de mettre à jour la photo de profil.",
         "تعذر تحديث صورة الملف الشخصي.",
        400,
        1011
        );
}

BaseApiError ApiErrorFactory::FailedToFetchProfilePhotoPath()
{
    return BaseApiError(
        "Could not retrieve the profile picture path.",
       "دریافت مسیر عکس پروفایل با مشکل مواجه شد.",
        "Impossible de récupérer le chemin de la photo de profil.",
         "تعذر استرداد مسار صورة الملف الشخصي.",
        400,
        1012
        );
}

BaseApiError ApiErrorFactory::FailedToRemoveExistingProfilePictureFile()
{
    return BaseApiError(
         "Could not remove the old profile picture file.",
        "حذف فایل عکس پروفایل قبلی با مشکل مواجه شد.",
        "Impossible de supprimer l'ancien fichier de photo de profil.",
        "تعذر إزالة ملف صورة الملف الشخصي القديم.",
        400,
        1013
        );
}

BaseApiError ApiErrorFactory::FailedToSaveProfilePicture()
{
    return BaseApiError(
         "Could not save the new profile picture.",
        "ذخیره عکس پروفایل جدید با مشکل مواجه شد.",
        "Impossible d'enregistrer la nouvelle photo de profil.",
        "تعذر حفظ صورة الملف الشخصي الجديدة.",
        400,
        1014
        );
}

BaseApiError ApiErrorFactory::FailedToFetchConversationList()
{
    return BaseApiError(
         "Could not retrieve the conversation list.",
        "دریافت لیست گفتگوها با مشکل مواجه شد.",
        "Impossible de récupérer la liste des conversations.",
         "تعذر استرداد قائمة المحادثات.",
        400,
        1015
        );
}

BaseApiError ApiErrorFactory::InvalidConversationID()
{
    return BaseApiError(
        "The provided conversation ID is invalid.",
        "شناسه گفتگوی ارائه شده نامعتبر است.",
         "L'ID de conversation fourni n'est pas valide.",
        "معرّف المحادثة المقدم غير صالح.",
        400,
        1016
        );
}

BaseApiError ApiErrorFactory::InvalidAiMessageID()
{
    return BaseApiError(
         "The provided AI message ID is invalid.",
        "شناسه پیام هوش مصنوعی ارائه شده نامعتبر است.",
        "L'ID du message AI fourni n'est pas valide.",
         "معرّف رسالة الذكاء الاصطناعي المقدم غير صالح.",
        400,
        1017
        );
}

BaseApiError ApiErrorFactory::FailedToFetchConversatinMessageList()
{
    return BaseApiError(
         "Could not retrieve the message list for this conversation.",
        "دریافت لیست پیام های این گفتگو با مشکل مواجه شد.",
        "Impossible de récupérer la liste des messages pour cette conversation.",
        "تعذر استرداد قائمة الرسائل لهذه المحادثة.",
        400,
        1018
        );
}

BaseApiError ApiErrorFactory::FailedToFetchSearchedConversations()
{
    return BaseApiError(
        "Could not retrieve the searched conversation list.",
        "دریافت لیست گفتگوهای جستجو شده با مشکل مواجه شد.",
        "Impossible de récupérer la liste des conversations recherchées.",
        "تعذر استرداد قائمة المحادثات التي تم البحث عنها.",
        400,
        1019
        );
}

BaseApiError ApiErrorFactory::FailedToFetchMessageFeedBack()
{
    return BaseApiError(
         "Could not retrieve message feedback.",
        "دریافت بازخورد پیام با مشکل مواجه شد.",
        "Impossible de récupérer les commentaires du message.",
         "تعذر استرداد تعليقات الرسالة.",
        400,
        1020
        );
}

BaseApiError ApiErrorFactory::FailedToSubmitUserFeedback()
{
    return BaseApiError(
         "Could not submit user feedback.",
        "ثبت بازخورد کاربر با مشکل مواجه شد.",
        "Impossible de soumettre les commentaires de l'utilisateur.",
         "تعذر إرسال ملاحظات المستخدم.",
        400,
        1021
        );
}

BaseApiError ApiErrorFactory::InvalidValuesForFeedBack()
{
    return BaseApiError(
        "Invalid values were provided for feedback submission.",
        "مقادیر ارسالی برای بازخورد نامعتبر است.",
        "Des valeurs non valides ont été fournies pour la soumission des commentaires.",
         "تم تقديم قيم غير صالحة لإرسال التعليقات.",
        400,
        1022
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
BaseApiError ApiErrorFactory::UnAuthorizedAccess(QString permission)
{

    return BaseApiError(
        "Unauthorized access. Required permission: '" + permission + "'.",
        "دسترسی غیرمجاز. نیاز به مجوز: '" + permission + "'.",
        "Accès non autorisé. Permission requise : '" + permission + "'.",
        "وصول غير مصرح به. إذن مطلوب: '" + permission + "'.",
        400,
        1025
    );

}

BaseApiError ApiErrorFactory::DuplicateRagFileName(QString name)
{
    if(name.isEmpty()){
        return BaseApiError(
            "A file with the selected name already exists.",
            "فایلی با نام انتخاب شده از قبل وجود دارد.",
            "Un fichier avec le nom sélectionné existe déjà.",
            "يوجد بالفعل ملف بالاسم المختار.",
            409,
            1027
        );
    }
    return BaseApiError(
        "A file with the name '" + name + "' already exists.",
        "فایلی با نام '" + name + "' از قبل وجود دارد.",
        "Un fichier avec le nom '" + name + "' existe déjà.",
        "يوجد بالفعل ملف بالاسم '" + name + "'.",
        409,
        1027
    );
}

BaseApiError ApiErrorFactory::FailedToFindRagFile(QString name)
{
    return BaseApiError(
        "The file '" + name + "' was not found or might have been deleted.",
        "فایل '" + name + "' پیدا نشد یا ممکن است حذف شده باشد.",
        "Le fichier '" + name + "' n'a pas été trouvé ou pourrait avoir été supprimé.",
        "الملف '" + name + "' لم يتم العثور عليه أو قد تم حذفه.",
        404,
        1028
                );
}

BaseApiError ApiErrorFactory::FailedToFetchRagFileList()
{
    return BaseApiError(
        "Failed to fetch RAG files.",
        "خطا در دریافت فایل‌های RAG.",
        "Échec de la récupération des fichiers RAG.",
        "فشل في جلب ملفات RAG.",
        500,
        1029
    );

}



BaseApiError ApiErrorFactory::DuplicateRoleName(QString name)
{
    return BaseApiError(
        "A role with the name '" + name + "' already exists.",
        "نقشی با نام '" + name + "' از قبل وجود دارد.",
        "Un rôle avec le nom '" + name + "' existe déjà.",
        "يوجد بالفعل دور بالاسم '" + name + "'.",
        409,
        1030
                );
}

BaseApiError ApiErrorFactory::InvalidRoleName()
{
    return BaseApiError(
        "Invalid role name.",
        "نام نقش نامعتبر است.",
        "Nom de rôle invalide.",
        "اسم الدور غير صالح.",
        400,
        1031
                );
}

BaseApiError ApiErrorFactory::FailedToDeleteRole()
{
    return BaseApiError(
        "Failed to delete role.",
        "خطایی در حذف نقش پیش آمد.",
        "Échec de la suppression du rôle.",
        "فشل في حذف الدور.",
        400,
        1032
    );
}

BaseApiError ApiErrorFactory::FailedToUpdateRole()
{
    return BaseApiError(
        "Failed to update role.",
        "خطایی در بروزرسانی نقش پیش آمد.",
        "Échec de la mise à jour du rôle.",
        "فشل في تحديث الدور.",
        400,
        1033
    );
}

BaseApiError ApiErrorFactory::FailedToValidatePermAndRole(int dbErrCode)
{
    switch (dbErrCode)
    {
        case 1:
            return BaseApiError(
                "Role does not exist.",
                "نقش مورد نظر وجود ندارد.",
                "Le rôle n'existe pas.",
                "الدور غير موجود.",
                400,
                1034
            );

        case 2:
            return BaseApiError(
                "User does not have the required permission.",
                "کاربر مجوز لازم را ندارد.",
                "L'utilisateur n'a pas la permission requise.",
                "المستخدم لا يمتلك الصلاحية المطلوبة.",
                400,
                1035
            );

        case 3:
            return BaseApiError(
                "User cannot manage the specified role.",
                "کاربر نمی‌تواند این نقش را مدیریت کند.",
                "L'utilisateur ne peut pas gérer ce rôle.",
                "لا يمكن للمستخدم إدارة هذا الدور المحدد.",
                400,
                1036
            );

        default:
            return BaseApiError(
                "Failed to update role.",
                "خطایی در بروزرسانی نقش پیش آمد.",
                "Échec de la mise à jour du rôle.",
                "فشل في تحديث الدور.",
                400,
                1033
            );
    }
}
BaseApiError ApiErrorFactory::FailedToValidateUserManagableRoles(int dbErrCode)
{
    switch (dbErrCode)
    {
        case 1:
            return BaseApiError(
                "User has no assigned roles.",
                "کاربر هیچ نقشی ندارد.",
                "L'utilisateur n'a aucun rôle assigné.",
                "المستخدم ليس لديه أدوار مخصصة.",
                400,
                1040
            );

        case 2:
            return BaseApiError(
                "Some roles are unmanageable or do not exist.",
                "برخی از نقش‌ها قابل مدیریت نیستند یا وجود ندارند.",
                "Certains rôles ne sont pas gérables ou n'existent pas.",
                "بعض الأدوار غير قابلة للإدارة أو غير موجودة.",
                400,
                1041
            );
        case 3:
            return BaseApiError(
                "Role ID list is empty.",
                "لیست شناسه‌های نقش خالی است.",
                "La liste des identifiants de rôles est vide.",
                "قائمة معرفات الأدوار فارغة.",
                400,
                1042
            );
        default:
            return BaseApiError(
                "Failed to validate user manageable roles.",
                "خطا در اعتبارسنجی نقش‌های قابل مدیریت کاربر.",
                "Échec de la validation des rôles gérables de l'utilisateur.",
                "فشل في التحقق من الأدوار القابلة للإدارة للمستخدم.",
                400,
                1039
            );
    }
}

BaseApiError ApiErrorFactory::FailedToUpdateManagableRole(int dbErrCode)
{
    switch (dbErrCode)
    {
        case 1:
            return BaseApiError(
                "Failed to insert manageable role.",
                "خطا در درج نقش قابل مدیریت.",
                "Échec de l'insertion du rôle gérable.",
                "فشل في إدراج الدور القابل للإدارة.",
                400,
                1053
            );
        case 2:
            return BaseApiError(
                "Failed to delete manageable role.",
                "خطا در حذف نقش قابل مدیریت.",
                "Échec de la suppression du rôle gérable.",
                "فشل في حذف الدور القابل للإدارة.",
                400,
                1054
            );

        default:
            return BaseApiError(
                "Failed to update manageable role.",
                "خطا در به‌روزرسانی نقش قابل مدیریت.",
                "Échec de la mise à jour du rôle gérable.",
                "فشل في تحديث الدور القابل للإدارة.",
                400,
                1053
            );
    }
}

BaseApiError ApiErrorFactory::BadRequest()
{
    return BaseApiError(
        "Bad request.",
        "درخواست نامعتبر.",
        "Requête invalide.",
        "طلب غير صالح.",
        400,
        1055
    );
}

BaseApiError ApiErrorFactory::FailedToFetchManagableRoles()
{
    return BaseApiError(
        "Failed to fetch managable roles list.",
        "خطا در دریافت لیست نقش‌های قابل مدیریت.",
        "Échec de la récupération de la liste des rôles gérables.",
        "فشل في جلب قائمة الأدوار القابلة للإدارة.",
        400,
        1056
    );

}
BaseApiError ApiErrorFactory::FailedToFetchManagableUsers()
{
    return BaseApiError(
        "Failed to fetch manageable users list.",
        "خطا در دریافت لیست کاربران قابل مدیریت.",
        "Échec de la récupération de la liste des utilisateurs gérables.",
        "فشل في جلب قائمة المستخدمين القابلين للإدارة.",
        400,
        1057
    );
}
BaseApiError ApiErrorFactory::FailedToUpdateUserRole(int dbErrCode)
{
    switch (dbErrCode)
    {
        case 1:
            return BaseApiError(
                "Failed to assign user role.",
                "خطا در اختصاص نقش به کاربر.",
                "Échec de l'attribution du rôle à l'utilisateur.",
                "فشل في تعيين الدور للمستخدم.",
                400,
                1058
            );
        case 2:
            return BaseApiError(
                "Failed to delete user role.",
                "خطا در حذف نقش کاربر.",
                "Échec de la suppression du rôle utilisateur.",
                "فشل في حذف دور المستخدم.",
                400,
                1059
            );
        default:
            return BaseApiError(
                "Failed to update user role.",
                "خطا در به‌روزرسانی نقش کاربر.",
                "Échec de la mise à jour du rôle utilisateur.",
                "فشل في تحديث دور المستخدم.",
                400,
                1060
            );
    }
}
BaseApiError ApiErrorFactory::FailedToValidatePermAndUser(int dbErrCode)
{
    switch (dbErrCode)
    {
        case 1:
            return BaseApiError(
                "User does not exist.",
                "کاربر مورد نظر وجود ندارد.",
                "L'utilisateur n'existe pas.",
                "المستخدم غير موجود.",
                400,
                1061
            );

        case 2:
            return BaseApiError(
                "Manager does not have the required permission.",
                "مدیر مجوز لازم را ندارد.",
                "Le gestionnaire n'a pas la permission requise.",
                "المدير لا يمتلك الصلاحية المطلوبة.",
                400,
                1062
            );

        case 3:
            return BaseApiError(
                "Manager cannot manage the specified user.",
                "مدیر نمی‌تواند این کاربر را مدیریت کند.",
                "Le gestionnaire ne peut pas gérer cet utilisateur.",
                "لا يمكن للمدير إدارة هذا المستخدم المحدد.",
                400,
                1063
            );

        default:
            return BaseApiError(
                "Failed to validate user management and permission.",
                "خطا در اعتبارسنجی مدیریت کاربر و مجوز.",
                "Échec de la validation de la gestion de l'utilisateur et de la permission.",
                "فشل في التحقق من إدارة المستخدم والصلاحية.",
                400,
                1064
            );
    }
}



BaseApiError ApiErrorFactory::InvalidRagCategoryId()
{
    return BaseApiError(
        "The provided RAG category ID is invalid.",
        "شناسه‌ی دسته‌بندی RAG نامعتبر است.",
        "L'ID de catégorie RAG fourni est invalide.",
        "معرف فئة RAG المقدم غير صالح.",
        400,
        1065
    );
}

BaseApiError ApiErrorFactory::RagCategoryNameAlreadyExists()
{
    return BaseApiError(
        "The RAG category name already exists.",
        "نام دسته‌بندی RAG از قبل وجود دارد.",
        "Le nom de la catégorie RAG existe déjà.",
        "اسم فئة RAG موجود بالفعل.",
        400,
        1066
    );
}

BaseApiError ApiErrorFactory::InvalidBot()
{
    return BaseApiError(
         "Invalid bot information provided.", // English
         "اطلاعات ربات ارائه شده نامعتبر است.", // Persian (Farsi)
         "Informations du bot fournies non valides.", // French
         "معلومات البوت المقدمة غير صالحة.", // Arabic
         400, // HTTP status code (Bad Request)
         1067 // Custom error code
     );
}

BaseApiError ApiErrorFactory::FailedToFetchManagableBots()
{
    return BaseApiError(
        "Failed to fetch manageable bots.", // English
        "خطا در دریافت لیست ربات‌های قابل مدیریت.", // Persian (Farsi)
        "Échec de la récupération des bots gérables.", // French
        "فشل في جلب قائمة البوتات القابلة للإدارة.", // Arabic
        500, // HTTP status code (Internal Server Error)
        1068 // Custom error code
    );
}

BaseApiError ApiErrorFactory::BotNameAlreadyExists()
{
    return BaseApiError(
         "The provided bot name already exists.", // English
         "نام ربات ارائه شده از قبل وجود دارد.", // Persian (Farsi)
         "Le nom du bot fourni existe déjà.", // French
         "اسم البوت المقدم موجود بالفعل.", // Arabic
         400, // HTTP status code (Bad Request)
         1069 // Custom error code
     );
}

BaseApiError ApiErrorFactory::InvalidRagId()
{
    return BaseApiError(
        "The provided RAG ID is invalid.",                    // English
        "شناسه‌ی RAG ارائه‌شده نامعتبر است.",                // Persian (Farsi)
        "L'ID de RAG fourni est invalide.",                   // French
        "معرف RAG المقدم غير صالح.",                        // Arabic
        400,                                                  // HTTP status code (Bad Request)
        1070                                                  // Custom error code
    );
}

BaseApiError ApiErrorFactory::InvalidRagType()
{
    return BaseApiError(
        "Invalid RAG types have been supplied.",              // English
        "نوع‌های RAG ارائه‌شده نامعتبر هستند.",             // Persian (Farsi)
        "Les types de RAG fournis sont invalides.",           // French
        "أنواع RAG المقدمة غير صالحة.",                     // Arabic
        400,                                                  // HTTP status code (Bad Request)
        1071                                                  // Custom error code
    );
}


