<?xml version="1.0"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

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

  <xsl:decimal-format grouping-separator="" decimal-separator=","/>

  <xsl:output method="text"/>

  <xsl:variable name="depth" select="2"/>

  <xsl:variable name="separator" select="'/'"/>

  <xsl:variable name="newline">
    <xsl:text>
</xsl:text>
  </xsl:variable>

  <xsl:variable name="flag_global" select="false()"/>

  <xsl:variable name="tab">
    <xsl:text>	</xsl:text>
  </xsl:variable>

  <xsl:template match="/">
    <xsl:apply-templates/>
  </xsl:template>
      
  <xsl:template match="pie">
    <xsl:choose>
      <xsl:when test="$flag_global">
        <xsl:apply-templates select="//dir">
          <!--          <xsl:sort order="ascending" data-type="number" select="sum(descendant::file/attribute::size)"/> -->
        </xsl:apply-templates>
      </xsl:when>
      <xsl:otherwise>
        <xsl:apply-templates select="dir"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <xsl:template match="dir">
    <xsl:if test="$depth &lt; 0 or count(ancestor::dir) &lt; $depth">
      <xsl:variable name="size" select="format-number((sum(descendant::file/attribute::size) div 1048576),'######0,00')"/>
      <xsl:choose>
        <xsl:when test="$flag_global">
          <xsl:value-of select="concat($size,$tab)"/>
          <xsl:for-each select="ancestor::dir">
            <xsl:value-of select="translate(@name,'\','/')"/>
            <xsl:value-of select="$separator"/>
          </xsl:for-each>
          <xsl:value-of select="@name"/>
          <xsl:value-of select="$newline"/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:for-each select="ancestor::dir">
            <xsl:value-of select="$tab"/>
          </xsl:for-each>
          <xsl:value-of select="concat($size,$tab,@name,$tab,count(file),$newline)"/>
          <xsl:apply-templates select="dir"/>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:if>
  </xsl:template>

  <xsl:template match="*"/>

</xsl:stylesheet>
