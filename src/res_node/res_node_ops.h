/*
  cxproc - Configurable Xml PROCessor

  Copyright (C) 2006..2024 by Alexander Tenbusch

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

*/

#include <res_node/res_node_io.h>

#ifdef _MSC_VER
#define MODE_DIR_CREATE (0)
#elif defined WIN32
#define MODE_DIR_CREATE (0)
#else
#define MODE_DIR_CREATE (S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH)
#endif

extern RN_ERROR
resNodeUnlinkRecursivelyStr(xmlChar *pucArgPath);

extern RN_ERROR
resNodeUnlink(resNodePtr prnArg, BOOL_T fRecursively);

extern RN_ERROR
resNodeUnlinkStr(xmlChar *pucArgPath);

extern RN_ERROR
resNodeTransfer(resNodePtr prnArgFrom, resNodePtr prnArgTo, BOOL_T fArgMove);

extern RN_ERROR
resNodeTransferStr(xmlChar *pucArgFrom, xmlChar *pucArgTo, BOOL_T fArgMove);

extern RN_ERROR
resNodeMakeDirectory(resNodePtr prnArg, int mode);

extern RN_ERROR
resNodeMakeDirectoryStr(xmlChar *pucArgPath, int mode);

#ifdef TESTCODE
extern int
resNodeTestOperations(void);
#endif

