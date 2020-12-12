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
#include "cxp.h"
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
  ImageInfo *image_info;
  Image *image;

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
  image_info=AcquireImageInfo();

  CopyMagickString(image_info->filename,(const char*)resNodeGetNameNormalizedNative(prnArg),MaxTextExtent);
  image = ReadImage(image_info,exception);
  CatchException(exception);
  DestroyExceptionInfo(exception);

  if (image == NULL) {
    PrintFormatLog(1,"Read Error");
    xmlSetProp(pndArg, BAD_CAST "type", BAD_CAST"error/format");
    DestroyImageInfo(image_info);
    return FALSE;
  }
  else {
    xmlChar mpucValue[BUFFER_LENGTH];
    char *value;

    xmlStrPrintf(mpucValue,sizeof(mpucValue),BAD_CAST"image/%s",image->magick);
    xmlSetProp(pndArg, BAD_CAST "type", mpucValue);

    pndInfo = xmlNewChild(pndArg, NULL, BAD_CAST"image", NULL);
    pndInfoChild = xmlNewChild(pndInfo, NULL, BAD_CAST"size",NULL);
    /*  */
    xmlStrPrintf(mpucValue,sizeof(mpucValue),BAD_CAST"%i",image->columns);
    xmlSetProp(pndInfoChild, BAD_CAST "col",mpucValue);
    /*  */
    xmlStrPrintf(mpucValue,sizeof(mpucValue),BAD_CAST"%i",image->rows);
    xmlSetProp(pndInfoChild, BAD_CAST "row",mpucValue);

    pndInfoChild = xmlNewChild(pndInfo, NULL, BAD_CAST"color",NULL);
    /*  */
    xmlStrPrintf(mpucValue,sizeof(mpucValue),BAD_CAST"%i",image->depth);
    xmlSetProp(pndInfoChild, BAD_CAST "depth",mpucValue);
    /*  */
    xmlStrPrintf(mpucValue,sizeof(mpucValue),BAD_CAST"%i",image->colors);
    xmlSetProp(pndInfoChild, BAD_CAST "colors",mpucValue);

    pndInfoChild = xmlNewChild(pndInfo, NULL, BAD_CAST"resolution",NULL);
    /*  */
    xmlStrPrintf(mpucValue,sizeof(mpucValue),BAD_CAST"%.3f",image->x_resolution);
    xmlSetProp(pndInfoChild, BAD_CAST "x",mpucValue);
    /*  */
    xmlStrPrintf(mpucValue,sizeof(mpucValue),BAD_CAST"%.3f",image->y_resolution);
    xmlSetProp(pndInfoChild, BAD_CAST "y",mpucValue);

    /* default is DPI */
    xmlSetProp(pndInfoChild,
	       BAD_CAST "dotper",
	       BAD_CAST ((image->units == PixelsPerCentimeterResolution) ? "cm" : "inch"));
#if 0
    /*
      code derived from ImageMagick-6.4.3/magick/identify.c
    */
    GetImageProperty(image,"exif:*");
    ResetImagePropertyIterator(image);
    property = GetNextImageProperty(image);
    if (property) {
      /*! Add image properties. */
      /* xmlNodePtr pndInfoExif; */
      xmlNodePtr pndInfoProp;

      /* pndInfoExif = xmlNewChild(pndInfo, NULL, BAD_CAST"exif",NULL); */
      while (property != (const char *) NULL) {
	if (strlen(property) > 1 && xmlStrstr(BAD_CAST property,BAD_CAST"xmlns")==NULL) {
	  value=(char *)GetImageProperty(image,property);
	  if (value) {
	    pndInfoProp = xmlNewChild(pndInfo, NULL, BAD_CAST property,NULL);
	    xmlSetProp(pndInfoProp, BAD_CAST "value",BAD_CAST value);
	  }
	}
	property=GetNextImageProperty(image);
      }
    }
#endif

    DestroyImage(image);
  }
  DestroyImageInfo(image_info);
#endif

  return TRUE;
}
/* end of imgParseFile() */


/*! process the image

  \return TRUE if success
 */
BOOL_T
imgProcessImage(xmlNodePtr pndArg, resNodePtr prnArgSrc, resNodePtr prnArgTo)
{
#ifdef HAVE_LIBMAGICK
  ExceptionInfo *pException;
  ImageInfo *pInfoImage;
  ImageInfo *pInfoImageWrite;
  MagickBooleanType fStatus;

  pException=AcquireExceptionInfo();
  pInfoImage=AcquireImageInfo();

  PrintFormatLog(1,"Processing IMAGE");

  if (resNodeIsReadable(prnArgSrc)) {
    /*
      Read an image file.
     */
    xmlChar *pucAttrFrame;
    xmlChar *pucAttrNote;
    unsigned long width_frame, height_frame;
    float scale;
    float scale_cmp;
    Image *pImage = NULL;
    Image *pImageOrientated;

    if (resNodeIsFileInArchive(prnArgSrc)) {
      char *pcResult;

      PrintFormatLog(2,"Read IMAGE from ZIP '%s'",resNodeGetNameNormalized(prnArgSrc));
      if((pcResult = resNodeGetContent(prnArgSrc,1024))) {
	pImage = BlobToImage(pInfoImage, pcResult, resNodeGetSize(prnArgSrc),pException);
      }
    }
    else {    
      PrintFormatLog(2,"Read IMAGE '%s'",resNodeGetNameNormalized(prnArgSrc));
      CopyMagickString(pInfoImage->filename,resNodeGetNameNormalizedNative(prnArgSrc),MaxTextExtent);
      pImage=ReadImage(pInfoImage,pException);
    }
    
    CatchException(pException);
    if (pImage == NULL) {
      /*!\todo Error handling */
      PrintFormatLog(1,"Read Error");
      return FALSE;
    }

    /*
	Write the image then destroy it.
     */
    height_frame = 0;
    width_frame  = 0;
    pucAttrFrame = xmlGetProp(pndArg,BAD_CAST "frame");
    if (pucAttrFrame) {
      char *pcEnd;
      width_frame = (unsigned long) strtol((const char *)pucAttrFrame, &pcEnd, 10);
      if (*pcEnd=='x') {
	pcEnd++;
	height_frame = (unsigned long) strtol((const char *)pcEnd, &pcEnd, 10);
	PrintFormatLog(2,"Frame for IMAGE '%ix%i'",width_frame,height_frame);
      }
      else {
	PrintFormatLog(1,"No usable frame definition for IMAGE '%s'",pucAttrFrame);
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

      PrintFormatLog(2,"Scale IMAGE '%.2f'",scale);
      pImageResize = ResizeImage(pImage,
	  (unsigned long) ceil((double)pImage->columns * scale),
	  (unsigned long) ceil((double)pImage->rows * scale),
	  BoxFilter,
	  1.0,
	  pException);
      CatchException(pException);
      if (pImageResize == NULL) {
	PrintFormatLog(1,"Error scaling IMAGE");
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
      PrintFormatLog(2,"No rotation operation neccessary");
      break;
    }
    CatchException(pException);
    if (pImageOrientated) {
      DestroyImage(pImage);
      pImage = pImageOrientated;
      pImageOrientated = NULL;
    }

    /*! allow visible annotation of pImage */

    pucAttrNote = xmlGetProp(pndArg,BAD_CAST "note");
    if (pucAttrNote != NULL && xmlStrlen(pucAttrNote) > 0) {
      DrawInfo *pDrawInfo;
      int res;
      char mcBuffer[BUFFER_LENGTH];
      xmlChar *pucT;

      PrintFormatLog(2,"Add note '%s'",pucAttrNote);

      pDrawInfo = AcquireDrawInfo();
      GetDrawInfo(pInfoImage,pDrawInfo);

      pucT = xmlStrdup(pucAttrNote);
      if (pucT) {
	NormalizeStringSpaces((char *)pucT);
	pDrawInfo->text = (char *)pucT;

	snprintf(mcBuffer,BUFFER_LENGTH, "%+d%+d", pImage->columns - 5, pImage->rows - 5);
	pDrawInfo->geometry = mcBuffer;
	pDrawInfo->gravity = NorthGravity;
	pDrawInfo->pointsize = 11.0;
	pDrawInfo->text_antialias = MagickTrue;
	pDrawInfo->family = "helvetica";
	pDrawInfo->style = ItalicStyle;
	pDrawInfo->align = RightAlign;
	/* 	draw_info->opacity = (Quantum) (QuantumRange* +0.5); */

	pDrawInfo->border_color.blue  = 0xff;
	pDrawInfo->border_color.green = 0xff;
	pDrawInfo->border_color.red   = 0xff;

	res = AnnotateImage(pImage, pDrawInfo);
      }
      //DestroyDrawInfo(pDrawInfo);
    }

    /*!\bug the image_info doesnt fit for pImageOrientated */

    PrintFormatLog(2,"Write IMAGE to '%s'",resNodeGetNameNormalized(prnArgTo));

    if (resNodeIsStd(prnArgTo)) {
      /* write to stdout */

      if (cxpRunmodeIsCgi()) {
	xmlChar *pucFilename;
	printf("Content-type: image/%s\n", pImage->magick);
	if (pImage->filename != NULL
	    && (pucFilename = resPathGetBasename(BAD_CAST pImage->filename)) != NULL) {
	  printf("Content-Disposition: filename=\"%s\"\n",pucFilename);
	  xmlFree(pucFilename);
	}
	printf("\n");
      }

      pInfoImageWrite=CloneImageInfo(pInfoImage);
      SetImageInfoFile(pInfoImageWrite,stdout);
      pInfoImageWrite->adjoin=MagickTrue;

      fStatus=WriteImage(pInfoImageWrite,pImage);
      pInfoImageWrite=DestroyImageInfo(pInfoImageWrite);
      if (fStatus == MagickFalse)
	InheritException(pException,&pImage->exception);
    }
    else {
      /* create target directory first */
      if (resNodeMakeDirectoryStr(resNodeGetNameBaseDir(prnArgSrc),MODE_DIR_CREATE)) {
	/* needed basedir created successfully */
      }
      WriteImages(pInfoImage,pImage,resNodeGetNameNormalizedNative(prnArgTo),pException);
    }
    CatchException(pException);

    //DestroyExceptionInfo(exception);
    MagickCoreTerminus();
    DestroyImage(pImage);
  }

  DestroyExceptionInfo(pException);
  DestroyImageInfo(pInfoImage);
#endif

  return TRUE;
}
/* end of imgProcessImage() */


/*!
 */
void
imgMain(xmlNodePtr pndArg, cxpContextPtr pccArg)
{
  xmlChar *pucAttrTo;

  pucAttrTo  = xmlGetProp(pndArg,BAD_CAST "to");
  if ((pucAttrTo != NULL && xmlStrlen(pucAttrTo) > 0)) {
    xmlNodePtr pndChild;
    resNodePtr prnTo;

    prnTo = resNodeConcatNew(cxpCtxtLocationGetStr(pccArg),pucAttrTo);

    if ((pndChild = domGetFirstChild(pndArg,NAME_IMAGE))) {
      xmlChar *pucAttrSrc;

      pucAttrSrc = xmlGetProp(pndChild,BAD_CAST "src");
      if ((pucAttrSrc != NULL && xmlStrlen(pucAttrSrc) > 0)) {
	resNodePtr prnSrc;

	prnSrc = resNodeConcatNew(cxpCtxtLocationGetStr(pccArg),pucAttrSrc);
#ifdef HAVE_LIBMAGICK
	imgProcessImage(pndArg,prnSrc,prnTo);
#else
	PrintFormatLog(1,"Copy IMAGE without image processing");
#ifdef HAVE_CGI
	  xmlNodePtr pndCopy;

	  pndCopy = xmlNewNode(NULL,BAD_CAST"copy");
	  xmlSetProp(pndCopy, BAD_CAST "from", resNodeGetNameNormalized(prnSrc));
	  xmlSetProp(pndCopy, BAD_CAST "to", resNodeGetNameNormalized(prnTo));
	  //dirCopyFile(pndCopy,NULL);
	  xmlFreeNode(pndCopy);
#else
	  resNodeTransfer(prnSrc,prnTo,FALSE);
#endif
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

	PrintFormatLog(2,"Comment IMAGE '%s'",resNodeGetNameNormalized(prnTo));

	prnComment = resNodeCommentNew(prnTo);
	if (prnComment) {
	  if (resNodeOpen(prnComment,"w")) {
	    if (fputs((const char*)pucComment,(FILE *)resNodeGetHandleIO(prnComment)) == EOF) {
	      PrintFormatLog(1,"Write error file '%s'", resNodeGetNameNormalized(prnComment));
	    }
	    else {
	      PrintFormatLog(2,"Wrote comment file '%s'", resNodeGetNameNormalized(prnComment));
	    }
	  }
	  else {
	    PrintFormatLog(1,"Error resNodeOpen()\n");
	  }
	  resNodeFree(prnComment);
	}
	else {
	  PrintFormatLog(1,"Error resNodeCommentNew()\n");
	}
      }
      xmlFree(pucComment);
    }
    resNodeFree(prnTo);
  }
  else {
    PrintFormatLog(1,"No valid attributes in '%s'",pndArg->name);
  }
}
/* end of imgMain() */


#ifdef TESTCODE
#include "test/test_image.c"
#endif

