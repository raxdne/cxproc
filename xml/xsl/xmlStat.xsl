<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">

<xsl:variable name="newline">
<!-- a newline xsl:text element -->
<xsl:text>
</xsl:text>
</xsl:variable>

<!--  -->

  <xsl:output method="text"/>  

  <xsl:template match="/">
    <xsl:apply-templates select="//*"/>
  </xsl:template>
      
  <xsl:template match="*">
    <xsl:choose>
      <xsl:when test="true()">
        <xsl:value-of select="concat(position(),' ',count(ancestor::*),' ',name(),$newline)"/>
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="concat(name(),$newline)"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <xsl:template match="meta|word|tags"/>
      
</xsl:stylesheet>
