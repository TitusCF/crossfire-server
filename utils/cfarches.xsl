<?xml version="1.0" encoding="ISO-8859-1" ?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="html"/>
<xsl:template match="ARCHES">

<table border="1" bgcolor="#fff">
<tr bgcolor="#aaf" align ='center'>
<th>Object</th>
<th>Name</th>
<th>Face</th>
<th>level</th>
<th>wc</th>
<th>ac</th>
<th>dam</th>
<th>exp</th>
<th>speed</th>
<th>str</th>
<th>dex</th>
<th>con</th>
<th>int</th>
<th>wis</th>
<th>pow</th>
<th>cha</th>
<th>resist_physical</th>
<th>resist_poison</th>
<th>can_use_shield</th>
<th>can_use_armour</th>
<th>can_use_weapon</th>
<th>can_see_in_dark</th>
</tr>

<xsl:for-each select="arch">
<tr bgcolor='#ccf' align='center'>
<td><xsl:value-of select="object"/></td>
<td><xsl:value-of select="name"/></td>
<td><xsl:value-of select="face"/></td>
<td><xsl:value-of select="level"/></td>
<td><xsl:value-of select="wc"/></td>
<td><xsl:value-of select="ac"/></td>
<td><xsl:value-of select="dam"/></td>
<td><xsl:value-of select="exp"/></td>
<td><xsl:value-of select="speed"/></td>
<td><xsl:value-of select="str"/></td>
<td><xsl:value-of select="dex"/></td>
<td><xsl:value-of select="con"/></td>
<td><xsl:value-of select="int"/></td>
<td><xsl:value-of select="wis"/></td>
<td><xsl:value-of select="pow"/></td>
<td><xsl:value-of select="cha"/></td>
<td><xsl:value-of select="resist_physical"/></td>
<td><xsl:value-of select="resist_poison"/></td>
<td><xsl:value-of select="can_use_shield"/></td>
<td><xsl:value-of select="can_use_armour"/></td>
<td><xsl:value-of select="can_use_weapon"/></td>
<td><xsl:value-of select="can_see_in_dark"/></td>

</tr>
</xsl:for-each>
</table>
</xsl:template>
</xsl:stylesheet>
