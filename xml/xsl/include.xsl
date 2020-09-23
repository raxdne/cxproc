<?xml version="1.0" encoding="ISO-8859-1"?>
<xsl:stylesheet version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template match="INCLUDE">
  <xsl:choose>
    <xsl:when test="@FILE">
      <xsl:apply-templates select="document(@FILE)/*"/>
    </xsl:when>
    <xsl:otherwise>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>


</xsl:stylesheet>
