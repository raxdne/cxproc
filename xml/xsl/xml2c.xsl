<?xml version="1.0" encoding="ISO-8859-1"?>

<!--
 transform a XML into C code building an according DOM with
 libxml2 functions
 -->

<xsl:stylesheet version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

  <xsl:output method="text" encoding='ISO-8859-1'/>

  <xsl:strip-space elements="*"/>

<xsl:variable name="newpar">
<xsl:text>
</xsl:text>
</xsl:variable>

  <xsl:template match="/">
     <!-- 
          <xsl:variable name="ns_doc">
       <xsl:value-of select="namespace::*"/>
     </xsl:variable>
 -->

    <!-- declaration of all namespaces -->
    <xsl:for-each select="*/namespace::*"> <!-- TODO: too narrow ?? -->
      <xsl:text>xmlNsPtr pns</xsl:text>
      <xsl:value-of select="local-name()"/>
      <xsl:text>;</xsl:text>
      <xsl:value-of select="$newpar"/>
    </xsl:for-each>
    <!--  -->
    <xsl:text>xmlChar *pucT;</xsl:text>
    <xsl:value-of select="$newpar"/>
    <xsl:text>xmlDocPtr pdocResult;</xsl:text>
    <xsl:value-of select="$newpar"/>
    <xsl:for-each select="//*">
      <xsl:text>xmlNodePtr pnd</xsl:text>
      <xsl:value-of select="position()"/>
      <xsl:text>;</xsl:text>
      <xsl:value-of select="$newpar"/>
    </xsl:for-each>
    <xsl:value-of select="$newpar"/>
    <xsl:text>pdocResult = xmlNewDoc(BAD_CAST "1.0");</xsl:text>
    <xsl:value-of select="$newpar"/>
    <xsl:for-each select="//*">
      <xsl:variable name="node_pos" select="position()"/>
      <xsl:value-of select="$newpar"/>
      <xsl:choose>
        <xsl:when test="$node_pos = 1">
          <!-- root element -->
          <xsl:text>pnd1 = xmlNewDocNode(pdocResult, NULL, BAD_CAST </xsl:text>
          <xsl:text>&quot;</xsl:text>
          <xsl:value-of select="local-name()"/>
          <xsl:text>&quot;, NULL);</xsl:text>
          <xsl:value-of select="$newpar"/>
          <xsl:text>xmlDocSetRootElement(pdocResult, pnd1);</xsl:text>
          <xsl:value-of select="$newpar"/>
          <!-- append all namespaces -->
          <xsl:for-each select="namespace::*[not(local-name()='xml')]">
            <xsl:text>pns</xsl:text>
            <xsl:value-of select="local-name()"/>
            <xsl:text> = xmlNewNs(pnd1,</xsl:text>
            <xsl:text>BAD_CAST "</xsl:text>
            <xsl:value-of select="."/>
            <xsl:text>",BAD_CAST "</xsl:text>
            <xsl:value-of select="local-name()"/>
            <xsl:text>");</xsl:text>
            <xsl:value-of select="$newpar"/>
          </xsl:for-each>
          <!-- detect namespace -->
          <xsl:if test="substring-before(name(),':')">
            <xsl:text>xmlSetNs(pnd1,</xsl:text>
            <xsl:text>pns</xsl:text>
            <xsl:value-of select="substring-before(name(),':')"/>
            <xsl:text>);</xsl:text> 
            <xsl:value-of select="$newpar"/>
          </xsl:if>
          <xsl:for-each select="attribute::*">
            <!-- all attributes -->
            <xsl:text>xmlSetProp(pnd</xsl:text> 
            <xsl:value-of select="$node_pos"/>
            <xsl:text>, BAD_CAST "</xsl:text> 
            <xsl:value-of select="name()"/>
            <xsl:text>", BAD_CAST "</xsl:text> 
            <xsl:value-of select="."/>
            <xsl:text>");</xsl:text> 
            <xsl:value-of select="$newpar"/>
          </xsl:for-each>
        </xsl:when>
        <xsl:otherwise>
          <!-- non-root element -->
          <xsl:text>pnd</xsl:text>
          <xsl:value-of select="$node_pos"/>
          <xsl:text> = xmlNewChild(pnd</xsl:text>
          <xsl:value-of select="count(../preceding::*) + count(../ancestor::*) + 1"/>
          <!-- detect namespace -->
          <xsl:text>,</xsl:text>
          <xsl:choose>
            <xsl:when test="substring-before(name(),':')">
              <xsl:text>pns</xsl:text>
              <xsl:value-of select="substring-before(name(),':')"/>
            </xsl:when>
            <xsl:otherwise>
              <xsl:text>NULL</xsl:text>
            </xsl:otherwise>
          </xsl:choose>
          <!-- element name -->
          <xsl:text>,&quot;</xsl:text>
          <xsl:value-of select="local-name()"/>
          <xsl:text>&quot;,NULL);</xsl:text>
          <xsl:value-of select="$newpar"/>
          <xsl:for-each select="attribute::*">
            <!-- all attributes -->
            <xsl:text>xmlSetProp(pnd</xsl:text> 
            <xsl:value-of select="$node_pos"/>
            <xsl:text>, BAD_CAST "</xsl:text> 
            <xsl:value-of select="name()"/>
            <xsl:text>", BAD_CAST "</xsl:text> 
            <xsl:value-of select="."/>
            <xsl:text>");</xsl:text> 
            <xsl:value-of select="$newpar"/>
          </xsl:for-each>
          <xsl:for-each select="text()">
            <!-- all text nodes -->
            <xsl:text>xmlNodeAddContent(pnd</xsl:text> 
            <xsl:value-of select="$node_pos"/>
            <xsl:text>, BAD_CAST "</xsl:text> 
            <xsl:call-template name="lf2br">
              <xsl:with-param name="StringToTransform" select="."/>
            </xsl:call-template>
            <xsl:text>");</xsl:text> 
            <xsl:value-of select="$newpar"/>
          </xsl:for-each>
        </xsl:otherwise>
      </xsl:choose>
          <!-- element namespace -->
          <xsl:choose>
            <xsl:when test="false() and substring-before(name(),':')">
              <xsl:text>xmlNewNs(</xsl:text>
              <xsl:text>pnd</xsl:text>
              <xsl:value-of select="$node_pos"/>
              <xsl:text>,</xsl:text>
<!-- 
              <xsl:text>BAD_CAST &quot;</xsl:text>
              <xsl:text>http://aaa.de/</xsl:text>
              <xsl:text>&quot;,</xsl:text>
 -->
              <xsl:text>NULL,</xsl:text>
              <xsl:text>BAD_CAST &quot;</xsl:text>
              <xsl:value-of select="substring-before(name(),':')"/>
              <xsl:text>&quot;);</xsl:text>
            <xsl:value-of select="$newpar"/>
            </xsl:when>
            <xsl:otherwise>
              <xsl:text></xsl:text>
            </xsl:otherwise>
          </xsl:choose>
    </xsl:for-each>
    <xsl:value-of select="concat($newpar,'// xmlFreeDoc(pdocResult);',$newpar)"/>
  </xsl:template>


<xsl:template name="lf2br">
  <!-- import $StringToTransform -->
  <xsl:param name="StringToTransform"/>
  <xsl:choose>
    <!-- string contains linefeed -->
    <xsl:when test="contains($StringToTransform,'&#xA;')">
      <xsl:value-of select="substring-before($StringToTransform,'&#xA;')"/>
      <xsl:text>\n\&#xA;</xsl:text> 
      <xsl:call-template name="lf2br">
        <xsl:with-param name="StringToTransform">
          <xsl:value-of select="substring-after($StringToTransform,'&#xA;')"/>
        </xsl:with-param>
      </xsl:call-template>
    </xsl:when>
    <!-- string does not contain newline, so just output it -->
    <xsl:otherwise>
      <xsl:value-of select="$StringToTransform"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

</xsl:stylesheet>
