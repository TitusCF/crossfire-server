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
<th>can use</th>
<th>can_see_in_dark</th>
<th>body slots</th>
</tr>

<xsl:for-each select="arch">
<tr bgcolor='#ccf' align='center'>
<td><xsl:value-of select="object"/></td>
<td>
    <xsl:value-of select="name"/>
    <!-- If plural name if defined, also show that on its own line. -->
    <xsl:if test="name_pl">
        <br /><xsl:value-of select="name_pl" />
    </xsl:if>
</td>
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
<td>
    <!-- Also display the different things that can be used by a monster
         in a compact and readable format. -->
    <xsl:if test="can_use_shield">
        shield: <xsl:value-of select="can_use_shield" /><br />
    </xsl:if>
    <xsl:if test="can_use_weapon">
        weapon: <xsl:value-of select="can_use_weapon" /><br />
    </xsl:if>
    <xsl:if test="can_use_armour">
        armour: <xsl:value-of select="can_use_armour" /><br />
    </xsl:if>
    <xsl:if test="can_use_scroll">
        scroll: <xsl:value-of select="can_use_scroll" /><br />
    </xsl:if>
    <xsl:if test="can_use_skill">
        skill: <xsl:value-of select="can_use_skill" /><br />
    </xsl:if>
    <xsl:if test="can_cast_spell">
        spell: <xsl:value-of select="can_cast_spell" /><br />
    </xsl:if>
    <xsl:if test="can_use_ring">
        ring: <xsl:value-of select="can_use_ring" /><br />
    </xsl:if>
    <xsl:if test="can_use_bow">
        bow: <xsl:value-of select="can_use_bow" /><br />
    </xsl:if>
    <xsl:if test="can_use_range">
        range: <xsl:value-of select="can_use_range" /><br />
    </xsl:if>
</td>
<td><xsl:value-of select="can_see_in_dark"/></td>
<td>
    <!-- Give all the body slots a line in the same cell, like has been
         done for resistances and can_use_xxx. -->
    <xsl:if test="body_skill">
        skill: <xsl:value-of select="body_skill" /><br />
    </xsl:if>
    <xsl:if test="body_finger">
        finger: <xsl:value-of select="body_finger" /><br />
    </xsl:if>
    <xsl:if test="body_leg">
        leg: <xsl:value-of select="body_leg" /><br />
    </xsl:if>
    <xsl:if test="body_arm">
        arm: <xsl:value-of select="body_arm" /><br />
    </xsl:if>
    <xsl:if test="body_torso">
        torso: <xsl:value-of select="body_torso" /><br />
    </xsl:if>
    <xsl:if test="body_range">
        range: <xsl:value-of select="body_range" /><br />
    </xsl:if>
    <xsl:if test="body_neck">
        neck: <xsl:value-of select="body_neck" /><br />
    </xsl:if>
    <xsl:if test="body_head">
        head: <xsl:value-of select="body_head" /><br />
    </xsl:if>
    <xsl:if test="body_shoulder">
        shoulder: <xsl:value-of select="body_shoulder" /><br />
    </xsl:if>
    <xsl:if test="body_foot">
        foot: <xsl:value-of select="body_foot" /><br />
    </xsl:if>
    <xsl:if test="body_hand">
        hand: <xsl:value-of select="body_hand" /><br />
    </xsl:if>
    <xsl:if test="body_wrist">
        wrist: <xsl:value-of select="body_wrist" /><br />
    </xsl:if>
    <xsl:if test="body_waist">
        waist: <xsl:value-of select="body_waist" /><br />
    </xsl:if>
</td>

</tr>
</xsl:for-each>
</table>
</xsl:template>
</xsl:stylesheet>
