/*
  cxproc - Configurable Xml PROCessor

  Copyright (C) 2006..2020 by Alexander Tenbusch

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


#include <math.h>

/* 
 */
#include <libxml/parser.h>

#include <libexif/exif-data.h>

#include "basics.h"
#include <res_node/res_node_ops.h>
#include "dom.h"
#include "utils.h"
#include "plain_text.h"


/*! parse the 'pucNameFile' named image file and append results at 'pndFile'

s. http://libexif.cvs.sourceforge.net/viewvc/libexif/libexif/contrib/examples/photographer.c?view=markup

  \return TRUE if success
 */
BOOL_T
imgParseFileExif(xmlNodePtr pndArg, resNodePtr prnArg)
{
  BOOL_T fResult = FALSE;
  ExifData *pExifDataFile = NULL;

  /*! Read an image file.
  */
  if (resNodeIsFile(prnArg) == FALSE) {
    PrintFormatLog(1,"Permission at IMAGE file '%s' denied",resNodeGetNameNormalized(prnArg));
  }
  /*!\todo test exif-enabled picture formats */
  else if ((pExifDataFile = exif_data_new_from_file((const char*)resNodeGetNameNormalizedNative(prnArg))) == NULL) {
    PrintFormatLog(1,"No Exif data in '%s' found",resNodeGetNameNormalized(prnArg));
  }
  else {
    int i;
    xmlNodePtr pndExif;
    xmlNodePtr pndExifChild;
    xmlChar mpucValue[BUFFER_LENGTH];
    ExifEntry *pExifEntry;
    int iX = 0;
    int iY = 0;

    PrintFormatLog(2,"Read Exif info from file '%s'",resNodeGetNameNormalized(prnArg));

    pndExif = xmlNewChild(pndArg, NULL, BAD_CAST"exif", NULL);

    /* See if this tag exists */
    pExifEntry = exif_content_get_entry(pExifDataFile->ifd[EXIF_IFD_0],EXIF_TAG_MAKE);
    if (pExifEntry) {
      /* Get the contents of the tag in human-readable form */
      exif_entry_get_value(pExifEntry, (char *)mpucValue, sizeof(mpucValue));
      pndExifChild = xmlNewChild(pndExif, NULL, BAD_CAST "manufacturer",mpucValue);
    }

    pExifEntry = exif_content_get_entry(pExifDataFile->ifd[EXIF_IFD_0],EXIF_TAG_MODEL);
    if (pExifEntry) {
      /* Get the contents of the tag in human-readable form */
      exif_entry_get_value(pExifEntry, (char *)mpucValue, sizeof(mpucValue));
      pndExifChild = xmlNewChild(pndExif, NULL, BAD_CAST "model",mpucValue);
    }

    pndExifChild = NULL;
    for (i = (ExifIfd)0; i < 0xffff; i++) {
      char *n;
      ExifTag eTag;

      eTag = exif_tag_table_get_tag(i);
      n = (char *)exif_tag_get_title(eTag);
      if (n != NULL
	&& xmlStrcasestr(BAD_CAST n,BAD_CAST "date") && xmlStrcasestr(BAD_CAST  n,BAD_CAST "time")
	&& ((pExifEntry = exif_content_get_entry(pExifDataFile->ifd[EXIF_IFD_0],eTag))
	|| (pExifEntry = exif_content_get_entry(pExifDataFile->ifd[EXIF_IFD_1],eTag))
	|| (pExifEntry = exif_content_get_entry(pExifDataFile->ifd[EXIF_IFD_EXIF],eTag))
	|| (pExifEntry = exif_content_get_entry(pExifDataFile->ifd[EXIF_IFD_GPS],eTag))
	|| (pExifEntry = exif_content_get_entry(pExifDataFile->ifd[EXIF_IFD_INTEROPERABILITY],eTag)))
	&& exif_entry_get_value(pExifEntry,(char *)mpucValue,sizeof(mpucValue))) {
	/*  */
	int j;
	//int k;
	//BOOL_T fTime;

	PrintFormatLog(3,"Exif '%s'",n);
#if 0
	/* format as an ISO date */
	for (fTime=FALSE, k=j=0; mpucValue[j] != '\0'; j++, k++) {
	  if (mpucValue[j]==':' && ! fTime) {
	    /* date */
	    mpucValue[k] = '-';
	  }
	  else if (mpucValue[j]==' ') {
	    /* separator between date and time */
	    mpucValue[k] = 'T';
	    while (mpucValue[j]==' ') {
	      j++;
	    }
	    fTime = TRUE;
	  }
	  else {
	    mpucValue[k] = mpucValue[j];
	  }
	}
	mpucValue[k] = '\0';
#else
	/* fix some format errors */
	for (j=0; mpucValue[j] != '\0'; j++) {
	  if (j > 0 && mpucValue[j]==' ' && mpucValue[j-1]==':' && isdigit(mpucValue[j+1])) {
	    /* insert zero */
	    mpucValue[j] = '0';
	  }
	}
#endif
	if (pndExifChild == NULL) {
	  /* initial value */
	  pndExifChild = xmlNewChild(pndExif, NULL, BAD_CAST"date", mpucValue);
	}
	else if (eTag == EXIF_TAG_DATE_TIME_ORIGINAL || eTag == EXIF_TAG_DATE_TIME_DIGITIZED) {
	  /* override value */
	  xmlNodeSetContent(pndExifChild, mpucValue);
	  break;
	}
      }
    }
    
    pndExifChild = xmlNewChild(pndExif, NULL, BAD_CAST"size",NULL);
    pExifEntry = exif_content_get_entry(pExifDataFile->ifd[EXIF_IFD_EXIF], EXIF_TAG_PIXEL_X_DIMENSION);
    if (pExifEntry) {
      /* Get the contents of the tag in human-readable form */
      exif_entry_get_value(pExifEntry, (char *)mpucValue, sizeof(mpucValue));
      /*  */
      xmlSetProp(pndExifChild, BAD_CAST "x",mpucValue);
      iX = atoi((const char *)mpucValue);
    }

    pExifEntry = exif_content_get_entry(pExifDataFile->ifd[EXIF_IFD_EXIF], EXIF_TAG_PIXEL_Y_DIMENSION);
    if (pExifEntry) {
      /* Get the contents of the tag in human-readable form */
      exif_entry_get_value(pExifEntry, (char *)mpucValue, sizeof(mpucValue));
      /*  */
      xmlSetProp(pndExifChild, BAD_CAST "y",mpucValue);
      iY = atoi((const char *)mpucValue);
    }

    pndExifChild = xmlNewChild(pndExif, NULL, BAD_CAST"resolution",NULL);
    pExifEntry = exif_content_get_entry(pExifDataFile->ifd[EXIF_IFD_0],EXIF_TAG_X_RESOLUTION);
    if (pExifEntry) {
      /* Get the contents of the tag in human-readable form */
      exif_entry_get_value(pExifEntry, (char *)mpucValue, sizeof(mpucValue));
      /*  */
      xmlSetProp(pndExifChild, BAD_CAST "x",mpucValue);
    }
    pExifEntry = exif_content_get_entry(pExifDataFile->ifd[EXIF_IFD_0],EXIF_TAG_Y_RESOLUTION);
    if (pExifEntry) {
      /* Get the contents of the tag in human-readable form */
      exif_entry_get_value(pExifEntry, (char *)mpucValue, sizeof(mpucValue));
      /*  */
      xmlSetProp(pndExifChild, BAD_CAST "y",mpucValue);
    }
    pExifEntry = exif_content_get_entry(pExifDataFile->ifd[EXIF_IFD_0],EXIF_TAG_RESOLUTION_UNIT);
    if (pExifEntry) {
      /* Get the contents of the tag in human-readable form */
      exif_entry_get_value(pExifEntry, (char *)mpucValue, sizeof(mpucValue));
      /*  */
      xmlSetProp(pndExifChild, BAD_CAST "dotper",mpucValue);
    }

    if ((pExifEntry = exif_content_get_entry(pExifDataFile->ifd[EXIF_IFD_0],EXIF_TAG_ORIENTATION))) {
      /* Get the contents of the tag in human-readable form */
      exif_entry_get_value(pExifEntry, (char *)mpucValue, sizeof(mpucValue));
      /*  */
      pndExifChild = xmlNewChild(pndExif, NULL, BAD_CAST"orientation",mpucValue);
    }
    else {
      for (i = (ExifIfd)0; i < 0xffff; i++) {
	char *n;
	ExifTag eTag;

	eTag = exif_tag_table_get_tag(i);
	n = (char *)exif_tag_get_title(eTag);
	if (n != NULL
	  && xmlStrcasestr(BAD_CAST n,BAD_CAST "orientation")
	  && ((pExifEntry = exif_content_get_entry(pExifDataFile->ifd[EXIF_IFD_0],eTag))
	  || (pExifEntry = exif_content_get_entry(pExifDataFile->ifd[EXIF_IFD_1],eTag))
	  || (pExifEntry = exif_content_get_entry(pExifDataFile->ifd[EXIF_IFD_EXIF],eTag))
	  || (pExifEntry = exif_content_get_entry(pExifDataFile->ifd[EXIF_IFD_GPS],eTag))
	  || (pExifEntry = exif_content_get_entry(pExifDataFile->ifd[EXIF_IFD_INTEROPERABILITY],eTag)))
	  && exif_entry_get_value(pExifEntry,(char *)mpucValue,sizeof(mpucValue))) {
  	/*  */
  	pndExifChild = xmlNewChild(pndExif, NULL, BAD_CAST"orientation",mpucValue);
  	break;
        }
      }      
    }
    fResult = TRUE;
    
    /* Free the EXIF data */
    exif_data_unref(pExifDataFile);
  }

  return fResult;
} /* end of imgParseFileExif() */


#ifdef TESTCODE
#include "test/test_image_exif.c"
#endif

