<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

  <xsl:variable name="flag_result" select="//result"/>
  <xsl:variable name="dir_plain" select="''"/>
  <xsl:variable name="prefix_tmp" select="''"/>
  <xsl:variable name="prefix_tmpi" select="translate($prefix_tmp,'\','/')"/>

  <xsl:output method="html" encoding="UTF-8"/>  
  <!--  -->
  <xsl:template match="/">
    <xsl:element name="html">
      <xsl:element name="body">
      <xsl:element name="h3">Dir</xsl:element>
        <xsl:apply-templates/>
      </xsl:element>
    </xsl:element>
  </xsl:template>
      
  <xsl:template match="dir|pie">
    <xsl:choose>
      <xsl:when test="@name = 'tmp'"/>
      <xsl:when test="starts-with(@name,'_')"/>
      <xsl:when test="starts-with(@name,'.') and not(@name = '.svn')"/>
      <xsl:otherwise>
        <xsl:apply-templates select="dir|file">
          <xsl:sort order="ascending" data-type="text" case-order="lower-first" select="@name"/>
        </xsl:apply-templates>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>
      
<xsl:template match="file">
  <xsl:variable name="dir_name">
    <xsl:for-each select="ancestor::dir">
      <xsl:if test="@prefix">
        <xsl:value-of select="@prefix"/>
        <xsl:text>/</xsl:text>
      </xsl:if>
      <xsl:value-of select="@name"/>
      <xsl:text>/</xsl:text>
    </xsl:for-each>
  </xsl:variable>
  <!--  -->
  <xsl:choose>
    <xsl:when test="contains('pngjpgjpeggif',@ext)">
      <xsl:element name="p">
          <xsl:element name="img">
            <xsl:attribute name="title">
              <xsl:value-of select="@name"/>
            </xsl:attribute>
            <xsl:attribute name="src">
              <xsl:value-of select="@name"/>
            </xsl:attribute>
          </xsl:element>
      </xsl:element>
    </xsl:when>
    <xsl:when test="true()">
      <xsl:element name="p">
          <xsl:element name="a">
            <xsl:attribute name="href">
              <xsl:value-of select="@name"/>
            </xsl:attribute>
            <xsl:value-of select="@name"/>
          </xsl:element>
      </xsl:element>
    </xsl:when>
    <xsl:otherwise>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

</xsl:stylesheet>
