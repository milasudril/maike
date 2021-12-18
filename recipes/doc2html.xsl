<?xml version="1.0"?>
<xsl:stylesheet
  version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns="http://www.w3.org/1999/xhtml">

<xsl:output method="html" indent="yes" encoding="UTF-8"/>

<xsl:template match="@*|node()">
	<xsl:copy>
		<xsl:apply-templates select="@*|node()" />
	</xsl:copy>
</xsl:template>

<xsl:template match="@*|node()" mode="appendix">
	<xsl:copy>
		<xsl:apply-templates select="@*|node()" mode="appendix"/>
	</xsl:copy>
</xsl:template>

<xsl:template match="figure/caption">
	<figcaption>
		<xsl:apply-templates select="@*|node()" />
	</figcaption>
</xsl:template>

<xsl:template match="chapter|section|subsection|subsubsection|paragraph">
	<section>
		<xsl:attribute name="class">
			<xsl:value-of select="name()" />
			<xsl:if test="count(ancestor::appendix) > 0">_appendix</xsl:if>
		</xsl:attribute>
		<xsl:attribute name="id"><xsl:value-of select="@id" /></xsl:attribute>
			<h1><xsl:value-of select="@title" /></h1>
		<xsl:apply-templates select="./*"/>
	</section>
</xsl:template>

<xsl:template match="chapter|section|subsection|subsubsection|paragraph" mode="appendix">
	<section>
		<xsl:attribute name="class"><xsl:value-of select="name()" />_appendix</xsl:attribute>
		<xsl:attribute name="id"><xsl:value-of select="@id" /></xsl:attribute>
			<h1><xsl:value-of select="@title" /></h1>
		<xsl:apply-templates select="./*"/>
	</section>
</xsl:template>

<xsl:template match="main">
	<xsl:apply-templates select="chapter" />
</xsl:template>

<xsl:template match="appendix">
	<xsl:apply-templates select="chapter" />
</xsl:template>

<xsl:template match="main">
<xsl:apply-templates />
</xsl:template>

<xsl:template match="property-list/property">
	<tr>
		<td class="propname"><xsl:value-of select="@name"/></td>
		<td class="proptype"><xsl:apply-templates select="type"/></td>
		<td class="propdesc"><xsl:apply-templates select="desc"/></td>
	</tr>
</xsl:template>

<xsl:template match="property-list">
<table class="proplist">
<tr><th>Name</th><th>Type</th><th>Description</th></tr>
<xsl:apply-templates select="property"/>
</table>
</xsl:template>

<xsl:template match="enum/item">
	<tr>
		<td class="propname"><xsl:value-of select="@name"/></td>
		<td class="propdesc"><xsl:apply-templates select="node()"/></td>
	</tr>
</xsl:template>

<xsl:template match="enum">
<table class="enum">
<tr><th>Value</th><th>Description</th></tr>
<xsl:apply-templates select="item"/>
</table>
</xsl:template>

<xsl:template match="manpage">
<a><xsl:attribute name="href">https://linux.die.net/man/<xsl:value-of select="@section"/>/<xsl:value-of select="@name"/></xsl:attribute>
<code><xsl:value-of select="@name"/></code>(<xsl:value-of select="@section"/>)
</a>
</xsl:template>

<xsl:key name="item" match="chapter|section|subsection|subsubsection|figure" use="@id"/>

<xsl:template match="ref">
<a><xsl:attribute name="href">#<xsl:value-of select="@item"/></xsl:attribute>
<xsl:choose>
<xsl:when test=". = ''">
<xsl:value-of select="key('item', @item)/@title" />
</xsl:when>
<xsl:otherwise>
<xsl:apply-templates select="node()"/>
</xsl:otherwise>
</xsl:choose>
</a>
</xsl:template>

<xsl:template match="subsection" mode="toc">
	<li><a><xsl:attribute name="href">#<xsl:value-of select="@id" /></xsl:attribute><xsl:value-of select="@title" /></a></li>
</xsl:template>

<xsl:template match="section" mode="toc">
	<li><a><xsl:attribute name="href">#<xsl:value-of select="@id" /></xsl:attribute><xsl:value-of select="@title" /></a>
	<ol><xsl:apply-templates select="subsection" mode="toc"/></ol>
	</li>
</xsl:template>

<xsl:template match="chapter" mode="toc">
	<li><a><xsl:attribute name="href">#<xsl:value-of select="@id" /></xsl:attribute><xsl:value-of select="@title" /></a>
	<ol><xsl:apply-templates select="section" mode="toc"/></ol>
	</li>
</xsl:template>

<xsl:template match="main" mode="toc">
	<ol class="main">
		<xsl:apply-templates mode="toc"/>
	</ol>
</xsl:template>

<xsl:template match="appendix" mode="toc">
	<ol class="appendix">
		<xsl:apply-templates mode="toc"/>
	</ol>
</xsl:template>

<xsl:template match="tableofcontents">
<nav id="toc">
	<h1>Table of contents</h1>
	<xsl:apply-templates select="/document/main" mode="toc"/>
	<xsl:apply-templates select="/document/appendix" mode="toc"/>
</nav>
</xsl:template>

<xsl:template match="prelims">
<xsl:apply-templates />
</xsl:template>

<xsl:template match="main">
<xsl:apply-templates />
</xsl:template>

<xsl:template match="appendix">
<xsl:apply-templates />
</xsl:template>

<xsl:template match="include">
	<xsl:choose>
		<xsl:when test="count(ancestor::appendix) > 0">
			<xsl:apply-templates select="document(concat($current_dir, '/', @src))/content/*" mode="appendix"/>
		</xsl:when>
		<xsl:otherwise>
			<xsl:apply-templates select="document(concat($current_dir, '/', @src))/content/*"/>
		</xsl:otherwise>
	</xsl:choose>
</xsl:template>

<xsl:template match="/document">
<xsl:text disable-output-escaping='yes'>&lt;!DOCTYPE html&gt;
</xsl:text>
<html>
<head>
<title>
<xsl:value-of select="@title" />
</title>
<xsl:apply-templates select="resources/*"/>
</head>
<body>
<h1><xsl:value-of select="@title" /></h1>
<xsl:apply-templates select="prelims" />
<xsl:apply-templates select="main"/>
<xsl:apply-templates select="appendix" />
</body>
</html>
</xsl:template>

</xsl:stylesheet>