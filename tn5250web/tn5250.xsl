<?xml version="1.0" encoding="ISO-8859-1"?>

<!--
  XSLT stylesheet for generating tn5250 web pages.
  This code is placed in the public domain.
-->

<xsl:stylesheet
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">

<xsl:output method="html" indent="no" encoding="ISO-8859-1"
  doctype-public="-//W3C//DTD HTML 4.01//EN"
  doctype-system="http://www.w3.org/TR/html40/strict.dtd"/>


<!-- Parameters. -->

<xsl:param name="lastmod"/>

<!-- Top-level document elements. -->

<xsl:template match="/webpage">
 <html>
  <xsl:for-each select="@xml:lang">
   <xsl:attribute name="lang"><xsl:value-of select="."/></xsl:attribute>
  </xsl:for-each>

  <head>
   <title><xsl:value-of select="head/title"/></title>
   <link title="Default stylesheet" href="style.css" type="text/css"
     rel="stylesheet"/>
  </head>

  <body>
   <table class="titlebar" width="100%">
    <tr>
     <td class="titlecell">
      <a href="http://sourceforge.net/projects/tn5250/">
       <xsl:text>tn5250 at SourceForge</xsl:text>
      </a>
      <xsl:text> | </xsl:text>
      <xsl:apply-templates select="head/trail"/>
      <xsl:value-of select="head/title"/>
     </td>
    </tr>
   </table>

   <xsl:apply-templates select="body"/>

   <hr/>
   <p>
    <a href="http://sourceforge.net/">
     <img src="http://sflogo.sourceforge.net/sflogo.php?group_id=27533"
       width="88" height="31" alt="[SourceForge]" class="button"/>
    </a>
    <xsl:text> </xsl:text>
    <a href="http://validator.w3.org/check/referer">
     <img src="http://www.w3.org/Icons/valid-html401"
       height="31" width="88" alt="[Valid HTML 4]" class="button"/>
    </a>
    <xsl:text> </xsl:text>
    <a href="http://jigsaw.w3.org/css-validator/check/referer">
     <img src="http://jigsaw.w3.org/css-validator/images/vcss"
       height="31" width="88" alt="[Valid CSS]" class="button"/>
    </a>
   </p>

   <xsl:if test="$lastmod">
    <p class="lastmod">Last modified: <xsl:value-of select="$lastmod"/></p>
   </xsl:if>
  </body>
 </html>
</xsl:template>

<xsl:template match="trail">
 <a href="{@href}"><xsl:value-of select="text()"/></a>
 <xsl:text> &gt; </xsl:text>
</xsl:template>


<!-- Document text elements. -->

<xsl:template match="body">
 <xsl:apply-templates select="h1|h2|h3|p|dl|ul|ol|pre"/>
</xsl:template>

<xsl:template match="h1|h2|h3">
 <xsl:copy>
  <xsl:choose>
   <xsl:when test="@id">
    <a name="{@id}"><xsl:value-of select="text()"/></a>
   </xsl:when>
   <xsl:otherwise>
    <xsl:value-of select="text()"/>
   </xsl:otherwise>
  </xsl:choose>
 </xsl:copy>
</xsl:template>

<xsl:template match="dl">
 <dl>
  <xsl:apply-templates select="dt|dd"/>
 </dl>
</xsl:template>

<xsl:template match="ol|ul">
 <xsl:copy>
  <xsl:apply-templates select="li"/>
 </xsl:copy>
</xsl:template>

<xsl:template match="p|pre|dt|dd|li|kbd|var|em|strong">
 <xsl:copy>
  <xsl:apply-templates/>
 </xsl:copy>
</xsl:template>

<xsl:template match="a">
 <a>
  <xsl:for-each select="@id">
   <xsl:attribute name="name"><xsl:value-of select="."/></xsl:attribute>
  </xsl:for-each>
  <xsl:for-each select="@href|@title"><xsl:copy/></xsl:for-each>

  <xsl:apply-templates/>
 </a>
</xsl:template>


<!-- Shortcut elements. -->

<xsl:template match="rfc">
 <a href="http://www.faqs.org/rfcs/rfc{@number}.html">
  <xsl:text>RFC </xsl:text>
  <xsl:value-of select="@number"/>
 </a>
</xsl:template>

<xsl:template match="ibmbook">
 <a href="http://publib.boulder.ibm.com/cgi-bin/bookmgr/BOOKS/{@code}/CCONTENTS">
  <xsl:value-of select="@pubnum"/>
 </a>
</xsl:template>

</xsl:stylesheet>
