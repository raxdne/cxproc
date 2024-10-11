
/* https://docs.microsoft.com/en-us/cpp/preprocessor/predefined-macros */

#ifndef CXP_TIMESTAMP_BUILD
  #ifndef __DATE__
    #define CXP_TIMESTAMP_BUILD "0000-00-00 00:00:00"
  #else
    #define CXP_TIMESTAMP_BUILD __DATE__ " " __TIME__
  #endif
#endif

#define CXP_VER_FILE_VERSION            CXP_VERSION_MAJOR, CXP_VERSION_MINOR, CXP_VERSION_REVISION, CXP_VERSION_BUILD
#ifndef CXP_VER_FILE_VERSION_STR
  #define CXP_VER_FILE_VERSION_STR        CXP_VERSION_STR
#endif

#define CXP_VER_PRODUCTNAME_STR         "cxproc"
#define CXP_VER_PRODUCT_VERSION         CXP_VER_FILE_VERSION
#define CXP_VER_PRODUCT_VERSION_STR     CXP_VER_FILE_VERSION_STR
#define CXP_VER_ORIGINAL_FILENAME_STR   CXP_VER_PRODUCTNAME_STR ".exe"
#define CXP_VER_INTERNAL_NAME_STR       CXP_VER_ORIGINAL_FILENAME_STR

#define CXP_VER_URL			CXP_VERSION_URL

#ifdef _MSC_VER
#ifdef _DEBUG
#define CXP_VER_VER_DEBUG             VS_FF_DEBUG
#else
#define CXP_VER_VER_DEBUG             0
#endif

#define CXP_VER_FILEOS                  VOS_UNKNOWN
#define CXP_VER_FILEFLAGS               CXP_VER_VER_DEBUG
#define CXP_VER_FILETYPE                VFT_APP
#endif

#define GPL_LICENSE_STR           \
    "License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>.\n" \
    "This is free software: you are free to change and redistribute it.\n" \
    "There is NO WARRANTY, to the extent permitted by law.\n"
