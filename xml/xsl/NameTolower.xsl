<?xml version="1.0" encoding="ISO-8859-1"?>
<xsl:stylesheet
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
  xmlns:cxp="http://www.tenbusch.info/cxproc">

<!--

PKG2 - ProzessKettenGenerator second implementation 
Copyright (C) 1999-2006 by Alexander Tenbusch

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

-->

  <xsl:output method="xml" doctype-system="file:///H:/opt/cxproc/xml/pie.dtd" encoding="ISO-8859-1"/>
  <!-- ../../xml/cxp.dtd -->

  <xsl:variable name="name_ns" select="''"/>

  <xsl:template match="/">
    <xsl:processing-instruction name="xml-stylesheet">href="file:///H:/opt/cxproc/contrib/pie/xsl/html/pie2html.xsl" type="text/xsl"</xsl:processing-instruction>
    <xsl:apply-templates/> 
  </xsl:template>

  <xsl:template match="*">
    <xsl:choose>
      <xsl:when test="contains(name(),':')">
        <!-- this element has a namespace already -->
        <xsl:copy-of select="."/> 
      </xsl:when>
      <xsl:when test="name()='SLIDE'">
        <!-- old slide naming -->
        <xsl:element name="section">
          <xsl:attribute name="type">slide</xsl:attribute>
          <xsl:apply-templates select="@*"/>
          <xsl:apply-templates select="*"/>
        </xsl:element>
      </xsl:when>
      <xsl:otherwise>
    <xsl:element name="{concat($name_ns,translate(name(),'ABCDEFGHIJKLMNOPQRSTUVWXYZ','abcdefghijklmnopqrstuvwxyz'))}">
      <xsl:apply-templates select="@*"/> 
      <xsl:apply-templates/> 
    </xsl:element>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <xsl:template match="@*">
    <xsl:choose>
      <xsl:when test="name()='VALUE'">
        <xsl:attribute name="select">
          <xsl:apply-templates/> 
        </xsl:attribute>
      </xsl:when>
      <xsl:otherwise>
    <xsl:attribute name="{translate(name(),'ABCDEFGHIJKLMNOPQRSTUVWXYZ','abcdefghijklmnopqrstuvwxyz')}">
      <xsl:apply-templates/> 
    </xsl:attribute>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>
      
  <xsl:template match="comment()">
    <xsl:copy-of select="."/>
  </xsl:template>
      
</xsl:stylesheet>
