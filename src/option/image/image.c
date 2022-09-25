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

#ifdef HAVE_LIBMAGICK
#include <magick/ImageMagick.h>
#endif

#include "basics.h"
#include <res_node/res_node_ops.h>
#include <cxp/cxp.h>
#include "dom.h"
#include "utils.h"
#include <cxp/cxp_dir.h>
#include "plain_text.h"


/*! parse the 'pucNameFile' named image file and append results at 'pndFile'

  \return TRUE if success
 */
BOOL_T
imgParseFile(xmlNodePtr pndArg, resNodePtr prnArg)
{
#ifdef HAVE_LIBMAGICK
  xmlNodePtr pndInfo;
  xmlNodePtr pndInfoChild;
  ExceptionInfo *exception;
  ImageInfo *pInfoImageRead;
  Image *pImage;

  const char *property;

  /*! Read an image file.
  */
  if (resNodeIsFile(prnArg)) {
    PrintFormatLog(3,"Read IMAGE info from file '%s'",resNodeGetNameNormalized(prnArg));
  }
  else {
    PrintFormatLog(3,"Permission at IMAGE file '%s' denied",resNodeGetNameNormalized(prnArg));
    return FALSE;
  }

  exception=AcquireExceptionInfo();
  pInfoImageRead=AcquireImageInfo();

  CopyMagickString(pInfoImageRead->filename,(const char*)resNodeGetNameNormalizedNative(prnArg),MaxTextExtent);
  pImage = ReadImage(pInfoImageRead,exception);
  CatchException(exception);
  DestroyExceptionInfo(exception);

  if (pImage == NULL) {
    PrintFormatLog(1,"Read Error");
    xmlSetProp(pndArg, BAD_CAST "type", BAD_CAST"error/format");
    DestroyImageInfo(pInfoImageRead);
    return FALSE;
  }
  else {
    xmlChar mpucValue[BUFFER_LENGTH];
    char *value;

    xmlStrPrintf(mpucValue,sizeof(mpucValue),"image/%s",pImage->magick);
    xmlSetProp(pndArg, BAD_CAST "type", mpucValue);

    pndInfo = xmlNewChild(pndArg, NULL, BAD_CAST"pImage", NULL);
    pndInfoChild = xmlNewChild(pndInfo, NULL, BAD_CAST"size",NULL);
    /*  */
    xmlStrPrintf(mpucValue,sizeof(mpucValue),"%i",pImage->columns);
    xmlSetProp(pndInfoChild, BAD_CAST "col",mpucValue);
    /*  */
    xmlStrPrintf(mpucValue,sizeof(mpucValue),"%i",pImage->rows);
    xmlSetProp(pndInfoChild, BAD_CAST "row",mpucValue);

    pndInfoChild = xmlNewChild(pndInfo, NULL, BAD_CAST"color",NULL);
    /*  */
    xmlStrPrintf(mpucValue,sizeof(mpucValue),"%i",pImage->depth);
    xmlSetProp(pndInfoChild, BAD_CAST "depth",mpucValue);
    /*  */
    xmlStrPrintf(mpucValue,sizeof(mpucValue),"%i",pImage->colors);
    xmlSetProp(pndInfoChild, BAD_CAST "colors",mpucValue);

    pndInfoChild = xmlNewChild(pndInfo, NULL, BAD_CAST"resolution",NULL);
    /*  */
    xmlStrPrintf(mpucValue,sizeof(mpucValue),"%.3f",pImage->x_resolution);
    xmlSetProp(pndInfoChild, BAD_CAST "x",mpucValue);
    /*  */
    xmlStrPrintf(mpucValue,sizeof(mpucValue),"%.3f",pImage->y_resolution);
    xmlSetProp(pndInfoChild, BAD_CAST "y",mpucValue);

    /* default is DPI */
    xmlSetProp(pndInfoChild,
	       BAD_CAST "dotper",
	       BAD_CAST ((pImage->units == PixelsPerCentimeterResolution) ? "cm" : "inch"));
#if 0
    /*
      code derived from ImageMagick-6.4.3/magick/identify.c
    */
    GetImageProperty(pImage,"exif:*");
    ResetImagePropertyIterator(pImage);
    property = GetNextImageProperty(pImage);
    if (property) {
      /*! Add image properties. */
      /* xmlNodePtr pndInfoExif; */
      xmlNodePtr pndInfoProp;

      /* pndInfoExif = xmlNewChild(pndInfo, NULL, BAD_CAST"exif",NULL); */
      while (property != (const char *) NULL) {
	if (strlen(property) > 1 && xmlStrstr(BAD_CAST property,BAD_CAST"xmlns")==NULL) {
	  value=(char *)GetImageProperty(pImage,property);
	  if (value) {
	    pndInfoProp = xmlNewChild(pndInfo, NULL, BAD_CAST property,NULL);
	    xmlSetProp(pndInfoProp, BAD_CAST "value",BAD_CAST value);
	  }
	}
	property=GetNextImageProperty(pImage);
      }
    }
#endif

    DestroyImage(pImage);
  }
  DestroyImageInfo(pInfoImageRead);
#endif

  return TRUE;
}
/* end of imgParseFile() */


/*! process the image

  \param pndArg - DOM node with all processing attributes
  \param prnArgSrc - resource node of source
  \param pccArg - context for logging

  \return TRUE if success
 */
BOOL_T
imgProcessImage(xmlNodePtr pndArg, resNodePtr prnArgSrc, resNodePtr prnArgTo, cxpContextPtr pccArg)
{
  BOOL_T fResult = TRUE;
  
#ifdef HAVE_LIBMAGICK

  assert(prnArgSrc != NULL);
  assert(prnArgTo != NULL);
  assert(prnArgSrc != prnArgTo);

#ifdef DEBUG
  cxpCtxtLogPrint(pccArg,1,"Processing IMAGE");
#endif
  
  if (resNodeGetContent(prnArgSrc,1024) != NULL && resNodeGetSize(prnArgSrc) > 0) {
    /*
      Read an image file.
    */
    char *pcResult = NULL;
    xmlChar *pucAttrFrame;
    xmlChar *pucAttrNote;
    unsigned long width_frame, height_frame;
    float scale;
    float scale_cmp;
    Image *pImage = NULL;
    Image *pImageOrientated = NULL;
    ExceptionInfo *pException;
    ImageInfo *pInfoImageRead;
    ImageInfo *pInfoImageWrite;
    MagickBooleanType fStatus;

    pException = AcquireExceptionInfo();
    pInfoImageRead = AcquireImageInfo();

    pImage = BlobToImage(pInfoImageRead, resNodeGetContentPtr(prnArgSrc), resNodeGetSize(prnArgSrc), pException);
    CatchException(pException);
    if (pImage == NULL) {
      /*!\todo Error handling */
      DestroyImageInfo(pInfoImageRead);
      DestroyExceptionInfo(pException);
      cxpCtxtLogPrint(pccArg,1,"Read Error");
      return FALSE;
    }

    /*
      Write the image then destroy it.
    */
    height_frame = 0;
    width_frame  = 0;
    pucAttrFrame = domGetPropValuePtr(pndArg,BAD_CAST "frame");
    if (pucAttrFrame) {
      char *pcEnd;
      width_frame = (unsigned long) strtol((const char *)pucAttrFrame, &pcEnd, 10);
      if (*pcEnd=='x') {
	pcEnd++;
	height_frame = (unsigned long) strtol((const char *)pcEnd, &pcEnd, 10);
	cxpCtxtLogPrint(pccArg,2,"Frame for IMAGE '%ix%i'",width_frame,height_frame);
      }
      else {
	cxpCtxtLogPrint(pccArg,1,"No usable frame definition for IMAGE '%s'",pucAttrFrame);
      }
    }

    /*! scale the image before transformation to improve performance
     */

    /* There is a bug in ImageMagick-6.3.7: "exif:Orientation" is NULL */

    scale = 1.1;

    scale_cmp = (float)((pImage->orientation == RightTopOrientation
			 || pImage->orientation == LeftBottomOrientation)
			? height_frame : width_frame) / (float)pImage->columns;
    if (scale_cmp < scale) {
      scale = scale_cmp;
    }
    scale_cmp = (float)((pImage->orientation == RightTopOrientation
			 || pImage->orientation == LeftBottomOrientation)
			? width_frame : height_frame) / (float)pImage->rows;
    if (scale_cmp < scale) {
      scale = scale_cmp;
    }

    if (scale < 1.0) {
      Image *pImageResize;

      cxpCtxtLogPrint(pccArg,2,"Scale IMAGE '%.2f'",scale);
      pImageResize = ResizeImage(pImage,
				 (unsigned long) ceil((double)pImage->columns * scale),
				 (unsigned long) ceil((double)pImage->rows * scale),
				 BoxFilter,
				 1.0,
				 pException);
      CatchException(pException);
      if (pImageResize == NULL) {
	cxpCtxtLogPrint(pccArg,1,"Error scaling IMAGE");
      }
      else {
	DestroyImage(pImage);
	pImage = pImageResize;
      }
    }

    pImageOrientated = NULL;
    /* s. wand/mogrify.c */
    switch (pImage->orientation)
      {
      case TopRightOrientation:
	{
	  pImageOrientated=FlopImage(pImage,pException);
	  break;
	}
      case BottomRightOrientation:
	{
	  pImageOrientated=RotateImage(pImage,180.0,pException);
	  break;
	}
      case BottomLeftOrientation:
	{
	  pImageOrientated=FlipImage(pImage,pException);
	  break;
	}
      case LeftTopOrientation:
	{
	  pImageOrientated=TransposeImage(pImage,pException);
	  break;
	}
      case RightTopOrientation:
	{
	  pImageOrientated=RotateImage(pImage,90.0,pException);
	  break;
	}
      case RightBottomOrientation:
	{
	  pImageOrientated=TransverseImage(pImage,pException);
	  break;
	}
      case LeftBottomOrientation:
	{
	  pImageOrientated=RotateImage(pImage,270.0,pException);
	  break;
	}
      default:
	cxpCtxtLogPrint(pccArg,2,"No rotation operation neccessary");
	break;
      }
    CatchException(pException);
    if (pImageOrientated) {
      DestroyImage(pImage);
      pImage = pImageOrientated;
      pImageOrientated = NULL;
    }

    /*! allow visible annotation of pImage */

    pucAttrNote = domGetPropValuePtr(pndArg,BAD_CAST "note");
    if (STR_IS_NOT_EMPTY(pucAttrNote)) {
      DrawInfo *pDrawInfo;
      int res;
      char mcBuffer[BUFFER_LENGTH];
      xmlChar *pucT;

      cxpCtxtLogPrint(pccArg,2,"Add watermark '%s'",pucAttrNote);

      if ((pDrawInfo = AcquireDrawInfo()) != NULL ) {
	GetDrawInfo(pInfoImageRead,pDrawInfo);

	if ((pucT = xmlStrdup(pucAttrNote)) != NULL) {
	  NormalizeStringSpaces((char *)pucT);
	  pDrawInfo->text = xmlStrdup(pucT);

	  snprintf(mcBuffer,BUFFER_LENGTH, "%+d%+d", pImage->columns - 5, pImage->rows - 5);
	  pDrawInfo->geometry = xmlStrdup(mcBuffer);
	  pDrawInfo->gravity = NorthGravity;
	  pDrawInfo->pointsize = 11.0;
	  pDrawInfo->text_antialias = MagickTrue;
	  pDrawInfo->family = xmlStrdup("helvetica");
	  pDrawInfo->style = ItalicStyle;
	  pDrawInfo->align = RightAlign;
	  /* 	draw_info->opacity = (Quantum) (QuantumRange* +0.5); */

	  pDrawInfo->border_color.blue  = 0xff;
	  pDrawInfo->border_color.green = 0xff;
	  pDrawInfo->border_color.red   = 0xff;

	  res = AnnotateImage(pImage, pDrawInfo);
	}
	DestroyDrawInfo(pDrawInfo);
      }
    }

    /*!\bug the pInfoImageRead doesnt fit for pImageOrientated */

    cxpCtxtLogPrint(pccArg,2,"Write IMAGE to '%s'",resNodeGetNameNormalized(prnArgTo));

    if (resNodeIsStd(prnArgTo)) {
      /* write to stdout */

#ifdef HAVE_CGI
      xmlChar *pucFilename;
      printf("Content-type: image/%s\n", pImage->magick);
      if (pImage->filename != NULL
	  && (pucFilename = resPathGetBasename(BAD_CAST pImage->filename)) != NULL) {
	printf("Content-Disposition: filename=\"%s\"\n",pucFilename);
	xmlFree(pucFilename);
      }
      printf("\n");
#endif

      pInfoImageWrite = CloneImageInfo(pInfoImageRead);
      SetImageInfoFile(pInfoImageWrite,stdout);
      pInfoImageWrite->adjoin = MagickTrue;

      fStatus = WriteImage(pInfoImageWrite,pImage);
      (void) fflush(stdout);

      pInfoImageWrite = DestroyImageInfo(pInfoImageWrite);
      if (fStatus == MagickFalse) {
	InheritException(pException,&pImage->exception);
      }
    }
    else {
      /* create target directory first */
      if (resNodeMakeDirectoryStr(resNodeGetNameBaseDir(prnArgSrc),MODE_DIR_CREATE)) {
	/* needed basedir created successfully */
      }
      WriteImages(pInfoImageRead,pImage,resNodeGetNameNormalizedNative(prnArgTo),pException);
    }
    CatchException(pException);

    //DestroyExceptionInfo(exception);
    DestroyImage(pImage);
    DestroyExceptionInfo(pException);
    DestroyImageInfo(pInfoImageRead);
  }
#else
  fResult = FALSE;
#endif

  return fResult;
} /* end of imgProcessImage() */


/*!
 */
void
imgProcessImageNode(xmlNodePtr pndArg, cxpContextPtr pccArg)
{
  xmlChar *pucAttrTo;

  pucAttrTo  = domGetPropValuePtr(pndArg,BAD_CAST "to");
  if (STR_IS_NOT_EMPTY(pucAttrTo)) {
    xmlNodePtr pndChild;
    resNodePtr prnTo;

    prnTo = resNodeConcatNew(cxpCtxtLocationGetStr(pccArg),pucAttrTo);

    if ((pndChild = domGetFirstChild(pndArg,NAME_IMAGE))) {
      xmlChar *pucAttrSrc;

      pucAttrSrc = domGetPropValuePtr(pndChild,BAD_CAST "src");
      if (STR_IS_NOT_EMPTY(pucAttrSrc)) {
	resNodePtr prnSrc;

	prnSrc = resNodeConcatNew(cxpCtxtLocationGetStr(pccArg),pucAttrSrc);
#ifdef HAVE_CGI
	imgProcessImage(pndArg,prnSrc,prnTo,pccArg);
	/*!\todo extend and use resNodeOpen() and resNodeTransfer() ... ??? */
#else
	{
	  xmlNodePtr pndCopy;

	  cxpCtxtLogPrint(pccArg,1,"No image processing of '%s' enabled",resNodeGetNameNormalized(prnSrc));
	  pndCopy = xmlNewNode(NULL,NAME_FILECOPY);
	  xmlSetProp(pndCopy, BAD_CAST "from", resNodeGetNameNormalized(prnSrc));
	  xmlSetProp(pndCopy, BAD_CAST "to", resNodeGetNameNormalized(prnTo));
	  cxpProcessCopyNode(pndCopy,pccArg); /* to use full error handling/logging */
	  xmlFreeNode(pndCopy);
	}
#endif
	resNodeFree(prnSrc);
      }
    }
    else if ((pndChild = domGetFirstChild(pndArg,NAME_PLAIN))) {
      /*
	there is a simple comment text
       */
      xmlChar *pucComment;

      pucComment = cxpProcessPlainNode(pndChild,NULL);
      if (pucComment != NULL && xmlStrlen(pucComment) > 0) {
	resNodePtr prnComment;

	//NormalizeStringSpaces((char *)pucComment);

	cxpCtxtLogPrint(pccArg,2,"Comment IMAGE '%s'",resNodeGetNameNormalized(prnTo));

	prnComment = resNodeCommentNew(prnTo);
	if (prnComment) {
	  if (resNodeOpen(prnComment,"w")) {
	    if (fputs((const char*)pucComment,(FILE *)resNodeGetHandleIO(prnComment)) == EOF) {
	      cxpCtxtLogPrint(pccArg,1,"Write error file '%s'", resNodeGetNameNormalized(prnComment));
	    }
	    else {
	      cxpCtxtLogPrint(pccArg,2,"Wrote comment file '%s'", resNodeGetNameNormalized(prnComment));
	    }
	  }
	  else {
	    cxpCtxtLogPrint(pccArg,1,"Error resNodeOpen()\n");
	  }
	  resNodeFree(prnComment);
	}
	else {
	  cxpCtxtLogPrint(pccArg,1,"Error resNodeCommentNew()\n");
	}
      }
      xmlFree(pucComment);
    }
    resNodeFree(prnTo);
  }
  else {
    cxpCtxtLogPrint(pccArg,1,"No valid attributes in '%s'",pndArg->name);
  }
} /* end of imgProcessImageNode() */


#ifdef TESTCODE
#include "test/test_image.c"
#endif

