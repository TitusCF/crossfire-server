<!DOCTYPE xsl:stylesheet [
<!ENTITY tab "<xsl:text>    </xsl:text>"> <!-- Tab -->
<!ENTITY tab2 "<xsl:text>        </xsl:text>"> <!-- Two Tabs -->
<!ENTITY tab3 "<xsl:text>            </xsl:text>"> <!-- Three Tabs -->
<!ENTITY tab4 "<xsl:text>                </xsl:text>"> <!-- Four Tabs
-->
<!ENTITY n "<xsl:text>
</xsl:text>"> <!-- New Line -->
<!ENTITY n2 "<xsl:text>

</xsl:text>"> <!-- Two new Lines -->
<!ENTITY n3 "<xsl:text>


</xsl:text>"> <!-- Three new Lines -->
<!ENTITY space "<xsl:text> </xsl:text>"> <!-- Space -->
<!ENTITY e "<xsl:text/>"> 
]>
<xsl:transform version="1.0"
xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
xmlns:check="http://check.sourceforge.net/ns">

<xsl:output method="text"/>

<xsl:variable name="baseUri" select="/config/baseSiteUri"/>
<xsl:variable name="checkFile" select="/config/checkFile"/>
<xsl:variable name="checkFolder" select="/config/checkFolder"/>
<xsl:template match="/">

    <xsl:text>-------------------------------</xsl:text>&n;
    <xsl:text>------- Problems summary ------</xsl:text>&n;
    <xsl:text>-------------------------------</xsl:text>&n;
    <xsl:for-each select="document(concat($checkFolder,'/.automenu.xml'))/menu/subDirectory">
        <xsl:call-template name="directoryEntry">
            <xsl:with-param name="baseDir" select="concat($checkFolder,'/',@name)"/>
            <xsl:with-param name="testHierarchy"/>
	</xsl:call-template>
    </xsl:for-each>
    &n;&n;
    <xsl:text>-------------------------------</xsl:text>&n;
    <xsl:text>-------- Details follow -------</xsl:text>&n;
    <xsl:text>-------------------------------</xsl:text>&n;
    <xsl:for-each select="document(concat($checkFolder,'/.automenu.xml'))/menu/subDirectory">
        <xsl:call-template name="detailsEntries">
            <xsl:with-param name="baseDir" select="concat($checkFolder,'/',@name)"/>
            <xsl:with-param name="testHierarchy"/>
	</xsl:call-template>
    </xsl:for-each>
</xsl:template>
<xsl:template name="directoryEntry">
    <xsl:param name="baseDir"/>
    <xsl:param name="testHierarchy"/>
    <xsl:variable name="currentHierarchy" select="concat($testHierarchy,@name,':')"/>
    <xsl:for-each select="document(concat($baseDir,'/.automenu.xml'))/menu/subDirectory">
        <xsl:call-template name="directoryEntry">
            <xsl:with-param name="baseDir" select="concat($baseDir,'/',@name)"/>
            <xsl:with-param name="testHierarchy" select="$currentHierarchy"/>
	</xsl:call-template>
    </xsl:for-each>
    <xsl:for-each select="document(concat($baseDir,'/.automenu.xml'))/menu/entry">
        <xsl:if test="@error !='0' or @failure !='0'">
            <xsl:value-of select="concat($currentHierarchy,text())"/>&tab;
	    <xsl:value-of select="@error"/><xsl:text> error(s), </xsl:text>
	    <xsl:value-of select="@failure"/><xsl:text> failure(s)!</xsl:text>
	    &n;
        </xsl:if>
    </xsl:for-each>
</xsl:template>
<xsl:template name="detailsEntries">
    <xsl:param name="baseDir"/>
    <xsl:param name="testHierarchy"/>
    <xsl:variable name="currentHierarchy" select="concat($testHierarchy,@name,':')"/>
    <xsl:for-each select="document(concat($baseDir,'/.automenu.xml'))/menu/subDirectory">
        <xsl:call-template name="detailsEntries">
            <xsl:with-param name="baseDir" select="concat($baseDir,'/',@name)"/>
            <xsl:with-param name="testHierarchy" select="$currentHierarchy"/>
	</xsl:call-template>
    </xsl:for-each>
    <xsl:for-each select="document(concat($baseDir,'/.automenu.xml'))/menu/entry">
        <xsl:if test="@error !='0' or @failure !='0'">
        <xsl:call-template name="detailTestCase">
            <xsl:with-param name="testHierarchy" select="$currentHierarchy"/>
            <xsl:with-param name="baseDir" select="$baseDir"/>
            <xsl:with-param name="testCaseName" select="text()"/>
	</xsl:call-template>
        </xsl:if>
    </xsl:for-each>
</xsl:template>
<xsl:template name="detailTestCase">
    <xsl:param name="baseDir"/>
    <xsl:param name="testHierarchy"/>
    <xsl:param name="testCaseName"/>
    <xsl:for-each select="document(concat($baseDir,'/',$testCaseName,'.xml'))/check:testsuites/check:suite/check:test[@result='error' or @result='failure']">
        <xsl:call-template name="showDetails">
	    <xsl:with-param name="testHierarchy" select="concat($testHierarchy,$testCaseName)"/>
	</xsl:call-template>
    </xsl:for-each>
</xsl:template>
<xsl:template name="showDetails">
    <xsl:param name="testHierarchy"/>
    <xsl:choose>
        <xsl:when test="check:message/text()='test not yet developped'">
            <xsl:text>TODO    </xsl:text>
        </xsl:when>
        <xsl:when test="@result='error'">
            <xsl:text>ERROR   </xsl:text>
        </xsl:when>
        <xsl:otherwise>
            <xsl:text>FAILURE </xsl:text>
        </xsl:otherwise>
    </xsl:choose>
    <xsl:value-of select="$testHierarchy"/><xsl:text>:</xsl:text>
    <xsl:value-of select="check:id"/><xsl:text>(</xsl:text>
    <xsl:value-of select="check:fn"/><xsl:text>) </xsl:text>
    <xsl:value-of select="check:message"/>&n;
</xsl:template>
</xsl:transform>