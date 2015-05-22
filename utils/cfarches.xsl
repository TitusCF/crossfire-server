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
<th>resistances</th>
<th>attacktype</th>
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
<td>
    <!-- Give all resistances their own line in the same data cell. -->
    <xsl:if test="resist_physical">
        armour: <xsl:value-of select="resist_physical"/><br />
    </xsl:if>
    <xsl:if test="resist_magic">
        magic: <xsl:value-of select="resist_magic"/><br />
    </xsl:if>
    <xsl:if test="resist_fire">
        fire: <xsl:value-of select="resist_fire"/><br />
    </xsl:if>
    <xsl:if test="resist_cold">
        cold: <xsl:value-of select="resist_cold"/><br />
    </xsl:if>
    <xsl:if test="resist_electricity">
        electricity: <xsl:value-of select="resist_electricity"/><br />
    </xsl:if>
    <xsl:if test="resist_poison">
        poison: <xsl:value-of select="resist_poison"/><br />
    </xsl:if>
    <xsl:if test="resist_acid">
        acid: <xsl:value-of select="resist_acid"/><br />
    </xsl:if>
    <xsl:if test="resist_weaponmagic">
        weaponmagic: <xsl:value-of select="resist_weaponmagic"/><br />
    </xsl:if>
    <xsl:if test="resist_fear">
        fear: <xsl:value-of select="resist_fear"/><br />
    </xsl:if>
    <xsl:if test="resist_confusion">
        confusion: <xsl:value-of select="resist_confusion"/><br />
    </xsl:if>
    <xsl:if test="resist_slow">
        slow: <xsl:value-of select="resist_slow"/><br />
    </xsl:if>
    <xsl:if test="resist_paralyze">
        paralyze: <xsl:value-of select="resist_paralyze"/><br />
    </xsl:if>
    <xsl:if test="resist_blind">
        blind: <xsl:value-of select="resist_blind"/><br />
    </xsl:if>
    <xsl:if test="resist_drain">
        drain: <xsl:value-of select="resist_drain"/><br />
    </xsl:if>
    <xsl:if test="resist_deplete">
        deplete: <xsl:value-of select="resist_deplete"/><br />
    </xsl:if>
    <xsl:if test="resist_death">
        death: <xsl:value-of select="resist_death"/><br />
    </xsl:if>
    <xsl:if test="resist_ghosthit">
        ghosthit: <xsl:value-of select="resist_ghosthit"/><br />
    </xsl:if>
    <xsl:if test="resist_turn_undead">
        turn undead: <xsl:value-of select="resist_turn_undead"/><br />
    </xsl:if>
    <xsl:if test="resist_cancellation">
        cancellation: <xsl:value-of select="resist_cancellation"/><br />
    </xsl:if>
    <xsl:if test="resist_chaos">
        chaos: <xsl:value-of select="resist_chaos"/><br />
    </xsl:if>
    <xsl:if test="resist_godpower">
        godpower: <xsl:value-of select="resist_godpower"/><br />
    </xsl:if>
    <xsl:if test="resist_holyword">
        holyword: <xsl:value-of select="resist_holyword"/><br />
    </xsl:if>
</td>
<td><xsl:value-of select="attacktype"/></td>
<td><xsl:value-of select="can_use_shield"/></td>
<td><xsl:value-of select="can_use_armour"/></td>
<td><xsl:value-of select="can_use_weapon"/></td>
<td><xsl:value-of select="can_see_in_dark"/></td>

</tr>
</xsl:for-each>
</table>
</xsl:template>
</xsl:stylesheet>
