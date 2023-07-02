<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

	<xsl:template match="/">
		<html>
			<body style="margin:0;">
				<!-- bg -->
				<img src="background.svg" style="z-index:-10; position:fixed; width:100%"/>
				<!-- logo -->
				<img src="dracula.svg" style="width:400px;z-index:-10; opacity: 0.7; position:fixed; left:40%; top: 50%"/>
				<!-- info -->
				<div style="padding-top: 5%">
					<!-- home link -->
					<p style=" text-align:center">
						<a href="teamPage.xml">
							<img src="home.svg" style="width:70px;"/>
						</a>
					</p>
					<div style="display:flex; flex-direction:column; align-items:center; justify-content:flex-start;">
						<xsl:for-each select="team">
							<div style="text-align:center; font-family:cursive; font-size:35px;margin-top:40px">
								Ranking:<xsl:value-of select="ranking" />
							</div>
							<div style="text-align:center; font-family:cursive; font-size:35px">
								Scores:<xsl:value-of select="scores"/>
							</div>
							<div style="display: flex; flex-direction: row; justify-content: space-around">
								<div>
									<div style="text-align:center; font-family:cursive; font-size:35px">
										Name:<xsl:value-of select="womanPlayer/name"/>
									</div>
									<div style="text-align:center; font-family:cursive; font-size:35px">
										Country:<xsl:value-of select="womanPlayer/nationality"/>
									</div>
									<div style="text-align:center; font-family:cursive; font-size:35px">
										Age:<xsl:value-of select="womanPlayer/age"/>
									</div>
								</div>
								<div style="margin-left: 20px">
									<div style="text-align:center; font-family:cursive; font-size:35px">
										Name:<xsl:value-of select="manPlayer/name"/>
									</div>
									<div style="text-align:center; font-family:cursive; font-size:35px">
										Country<xsl:value-of select="manPlayer/nationality"/>
									</div>
									<div style="text-align:center; font-family:cursive; font-size:35px">
										Age<xsl:value-of select="manPlayer/age"/>
									</div>
								</div>
							</div>
							
						</xsl:for-each>
					</div>
				</div>
			</body>
		</html>
	</xsl:template>

</xsl:stylesheet>