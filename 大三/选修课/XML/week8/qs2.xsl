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
						<xsl:for-each select="players/player">
							<div style="text-align:center; font-family:cursive; font-size:35px;margin-top:40px">
								Name:<xsl:value-of select="name" />
							</div>
							<div style="text-align:center; font-family:cursive; font-size:35px">
								Age:<xsl:value-of select="age"/>
							</div>
							
						</xsl:for-each>
					</div>
				</div>
			</body>
		</html>
	</xsl:template>

</xsl:stylesheet>