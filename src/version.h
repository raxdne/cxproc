
/*
 s. http://www.zachburlingame.com/2011/02/versioning-a-native-cc-binary-with-visual-studio/ 
    http://www.zachburlingame.com/2011/03/integrating-the-subversion-revision-into-the-version-automatically-with-native-cc/
*/

#define STRINGIZE2(s) #s
#define STRINGIZE(s) STRINGIZE2(s)

#include "git_revision.h"

#define CXP_VER_FILE_DESCRIPTION_STR    "A Configurable Xml PROCessor."
#define CXP_VER_FILE_VERSION            CXP_VERSION_MAJOR, CXP_VERSION_MINOR, CXP_VERSION_REVISION, CXP_VERSION_BUILD
#define CXP_VER_FILE_VERSION_STR        CXP_VERSION_STR "-" CXP_REVISION_STR

#define CXP_VER_PRODUCTNAME_STR         "cxproc"
#define CXP_VER_PRODUCT_VERSION         CXP_VER_FILE_VERSION
#define CXP_VER_PRODUCT_VERSION_STR     CXP_VER_FILE_VERSION_STR
#define CXP_VER_ORIGINAL_FILENAME_STR   CXP_VER_PRODUCTNAME_STR ".exe"
#define CXP_VER_INTERNAL_NAME_STR       CXP_VER_ORIGINAL_FILENAME_STR
#define CXP_VER_COPYRIGHT_STR           "(C) 2006..2020 by Alexander Tenbusch"

#define CXP_VER_URL			"http://www.tenbusch.info/cxproc"

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
