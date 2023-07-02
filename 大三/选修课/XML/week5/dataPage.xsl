<?xml version="1.0" encoding="utf-8"?>

<xsl:stylesheet version="1.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

    <xsl:template match="/">
        <html>
            <body style="margin:0; font-size:150%">
                <!-- bg -->
                <img src="background.svg" style="z-index:-10; position:fixed; width:100%"/>
                <div style="display:flex; flex-direction:column;align-items:center; justify-content:center">
                    <a href="teamPage.xml" style="margin-top:3vh">
                        <img src="home.svg" style="width:70px;"/>
                    </a>
                    <div style="margin-top:3vh; ">
                        <table border="1" style="font-size:150%">
                            <xsl:copy-of select="$singleHeader" />
                            <xsl:apply-templates select="figureSkating/singleSkiMan" />
                        </table>
                    </div>


                    <div style="margin-top:3vh; ">
                        <table border="1" style="font-size:150%">
                            <xsl:copy-of select="$singleHeader" />
                            <xsl:apply-templates select="figureSkating/singleSkiWomen" />
                        </table>
                    </div>

                    <div style="margin-top:3vh; ">
                        <table border="1" style="font-size:150%">
                            <xsl:copy-of select="$mixedHeader" />
                            <xsl:apply-templates select="figureSkating/mixedPair" />
                        </table>
                    </div>
                </div>
            </body>
        </html>
    </xsl:template>

    <!-- 男女子单人表 -->
    <xsl:template name="singleSki">
        <xsl:if test="./@sex='male'">
            <h2>male</h2>
        </xsl:if>
        <xsl:if test="./@sex='female'">
            <h2>female</h2>
        </xsl:if>
        <xsl:for-each select="contestant">
            <tr>
                <td style="">
                    <xsl:value-of select="name" />
                </td>
                <td style="">
                    <xsl:value-of select="nationality" />
                </td>
                <td style="">
                    <xsl:value-of select="age" />
                </td>

                <xsl:choose>
                    <xsl:when test="ranking = 1">
                        <td bgcolor="#f3a694">
                            <xsl:value-of select="ranking" />
                        </td>
                    </xsl:when>
                    <xsl:otherwise>
                        <td style="">
                            <xsl:value-of select="ranking" />
                        </td>
                    </xsl:otherwise>
                </xsl:choose>
            </tr>
        </xsl:for-each>
    </xsl:template>

    <!-- 混合双人表 -->
    <xsl:template name="mixedPair">
        <h2>Mixed Pair</h2>
        <xsl:for-each select="team">
            <xsl:sort select="ranking"/>
            <tr>
                <xsl:if test="position() mod 2 = 1">
                    <xsl:attribute name="bgcolor">#c7e0f4</xsl:attribute>
                </xsl:if>
                <td style="">
                    <xsl:value-of select="ranking" />
                </td>
                <td style="">
                    <xsl:value-of select="*/nationality" />
                </td>
                <td style="">
                    <div style="display:flex-inline">
                        <div style="margin-right:2px">
                            <xsl:value-of select="womanPlayer/name"/>
                        </div>
                        <div>
                            <xsl:value-of select="manPlayer/name" />
                        </div>
                    </div>
                </td>
                <td style="">
                    <xsl:value-of select="scores" />
                </td>
            </tr>
        </xsl:for-each>
    </xsl:template>

    <xsl:variable name="singleHeader">
        <tr bgcolor="#fff">
            <th>name</th>
            <th>country</th>
            <th>age</th>
            <th>ranking</th>
        </tr>
    </xsl:variable>

    <xsl:variable name="mixedHeader">
        <tr bgcolor="#fff">
            <th>ranking</th>
            <th>country</th>
            <th>members</th>
            <th>scores</th>
        </tr>
    </xsl:variable>

    <xsl:template match="figureSkating/singleSkiMan">
        <xsl:call-template name="singleSki">
        </xsl:call-template>
    </xsl:template>

    <xsl:template match="figureSkating/singleSkiWomen">
        <xsl:call-template name="singleSki">
        </xsl:call-template>
    </xsl:template>

    <xsl:template match="figureSkating/mixedPair">
        <xsl:call-template name="mixedPair">
        </xsl:call-template>
    </xsl:template>

</xsl:stylesheet>