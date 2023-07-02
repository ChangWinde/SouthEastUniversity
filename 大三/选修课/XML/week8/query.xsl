<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

	<xsl:template match="/">
		<html>
			<body style="margin:0;">
				<!-- bg -->
				<img src="background.svg" style="z-index:-10; position:fixed; width:100%"/>
				<!-- info -->
				<div style="padding-top: 5%">
					<!-- home link -->
					<p style=" text-align:center">
						<a href="teamPage.xml">
							<img src="home.svg" style="width:70px;"/>
						</a>
					</p>
					<div style="margin: 20px;display:flex; flex-direction:column; align-items:center; justify-content:flex-start; border:black 3px dashed">

						<xsl:for-each select="board/royalPlayers/player">
						<div style="margin: 20px;border:black 3px dashed; ">
							<div style="text-align:center; font-family:cursive; font-size:35px;margin-top:40px">
								Ranking:<xsl:value-of select="ranking" />
							</div>
							<div style="text-align:center; font-family:cursive; font-size:35px;margin-top:40px">
								Name:<xsl:value-of select="name" />
							</div>
							<div style="text-align:center; font-family:cursive; font-size:35px">
								Age:<xsl:value-of select="age"/>
							</div>
						</div>
						</xsl:for-each>
					</div>

					<div style="margin: 20px;display:flex; flex-direction:column; align-items:center; justify-content:flex-start; border:black 3px dashed">
						<xsl:for-each select="board/ordPlayers/player">
						<div style="margin: 20px;border:black 3px dashed; ">
							<div style="text-align:center; font-family:cursive; font-size:35px;margin-top:40px">
								Name:<xsl:value-of select="name" />
							</div>
							<div style="text-align:center; font-family:cursive; font-size:35px">
								Age:<xsl:value-of select="age"/>
							</div>
						</div>
						</xsl:for-each>
					</div>

					<div style="margin: 20px;display:flex; flex-direction:column; align-items:center; justify-content:flex-start; border:black 3px dashed">
						<xsl:for-each select="board/newPlayers/contestant">
						<div style="margin: 20px;border:black 3px dashed; ">
							<div style="text-align:center; font-family:cursive; font-size:35px;margin-top:40px">
								Name:<xsl:value-of select="name" />
							</div>
							<div style="text-align:center; font-family:cursive; font-size:35px;margin-top:40px">
								Country:<xsl:value-of select="nationality" />
							</div>
							<div style="text-align:center; font-family:cursive; font-size:35px">
								Age:<xsl:value-of select="age"/>
							</div>
							<div style="text-align:center; font-family:cursive; font-size:35px">
								Ranking:<xsl:value-of select="ranking"/>
							</div>
						</div>
						</xsl:for-each>
					</div>
				</div>
			</body>
		</html>
	</xsl:template>

</xsl:stylesheet>