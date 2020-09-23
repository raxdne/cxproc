<?xml version="1.0" encoding="iso-8859-1"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

  <xsl:output method="text" encoding="iso-8859-1"/>  

  <!-- ignore all elements with an id and valid="no" -->

  <xsl:template match="/">
    <xsl:apply-templates select="pie"/>
  </xsl:template>
      
  <xsl:template match="dir|pie">
    <xsl:choose>
      <xsl:when test="@name = 'tmp'"/>
      <xsl:when test="starts-with(@name,'_')"/>
      <xsl:when test="@name = '.svn'"/>
      <xsl:otherwise>
        <xsl:apply-templates select="dir|file"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>
      
<xsl:template match="file">
  <xsl:variable name="dir_name">
    <xsl:for-each select="ancestor::dir">
      <xsl:value-of select="@name"/>
      <xsl:text>\</xsl:text>
    </xsl:for-each>
  </xsl:variable>
  <!--  -->
  <xsl:choose>
    <!--<xsl:when test="count(result) &lt; 1"/> -->
    <xsl:when test="substring-before(@name,'~')"/>
    <xsl:otherwise>
      <xsl:for-each select="result">
        <xsl:value-of select="$dir_name"/>
        <xsl:value-of select="parent::file/attribute::name"/>
	<xsl:text>:</xsl:text>
	<xsl:value-of select="@line"/>
	<xsl:text>: </xsl:text>
	<xsl:value-of select="."/>
	<xsl:text>
</xsl:text>
      </xsl:for-each>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

</xsl:stylesheet>

