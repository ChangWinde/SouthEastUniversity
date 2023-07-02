<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template match="/">
	<html>
		<body style="display:flex; align-items; center; flex-direction:row; height:1000px; margin:0">
			<!-- bg -->
			<img src="background.svg" style="z-index:-10; position:fixed; width:100%"/>
			<!-- logo -->
			<div style="display:flex; position:relative; margin-left:100px">
				<p style="position:absolute; top:300px;left:70px; width:300px">
					<a href="dataPage.xml"><img src="dataicon.svg" style="width:80px;"/></a>
					<a href="advertisePage.xml" style="margin-left:100px"><img src="advertiseicon.svg" style="width:80px;"/></a>
				</p>
				<img src="dracula.svg" style="width:400px;z-index:-10; opacity: 0.7"/>
			</div>
			<!-- personal logo -->
			<div style="display:flex; align-items:center; justify-content:space-around; flex-direction:column; margin-left:50px">
				<img src="bao.svg" style="width:320; margin-left: -100px"/>
				<img src="jun.svg" style="width:280;"/>
			</div >
			<!-- info -->
			<xsl:apply-templates select="team/teamMember"/>
		</body>
	</html>
</xsl:template>

<xsl:template match="team/teamMember">
	<div style="display:flex;flex-direction:column; font-family:cursive; font-size:35px; margin-top:100px; margin-left:50px">
		<xsl:for-each select="member">
			<div style="margin-bottom:300px">
				<div><xsl:value-of select="name"/></div>
				<div><xsl:value-of select="number"/></div>
				<div><xsl:value-of select="info"/></div>
			</div>	
		</xsl:for-each>
	</div>
</xsl:template>

</xsl:stylesheet>