<?xml version="1.0" encoding="ISO-8859-1"?>
<xsl:stylesheet version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:output method='text' encoding='ISO-8859-1'/>


<xsl:template match="/">
  <xsl:apply-templates select="//entry[datum and not(datum='') and notiz and not(notiz='')]"/>
</xsl:template>


<xsl:template match="entry">
  <xsl:value-of select="translate(datum,'-','')"/>
  <xsl:text> </xsl:text>
  <xsl:value-of select="@betrag"/>
  <xsl:text> </xsl:text>
  <xsl:value-of select="notiz"/>
  <xsl:text>
</xsl:text>
</xsl:template>


</xsl:stylesheet>
