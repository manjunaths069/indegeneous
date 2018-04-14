<?xml version="1.0" encoding="ISO-8859-1"?>
<!-- Edited with XML Spy v4.2 -->
<!--<!DOCTYPE html
     PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
     "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">-->
<!DOCTYPE html [<!ENTITY nbsp "&#160;">] >
<xsl:stylesheet
    xmlns:xsl='http://www.w3.org/1999/XSL/Transform' version='1.0'>
    <xsl:output method="html" encoding="iso-8859-1"/>
    <xsl:template match="/">
        <!--      <LINK REL="StyleSheet" HREF="/scripts/style.css" TYPE="text/css" MEDIA="screen" />-->
        <html>
            <head>
                <meta http-equiv="Expires" content="now"/>
                <meta http-equiv="Cache-Control" content="no-cache,must-revalidate"/>
                <meta http-equiv="Pragma" content="no-cache"/>
                <basefont class="normaltext"/>
            </head>
   <body>
   <table class="section-header" width="100%" border="0" cellspacing="0" cellpadding="0">
                       <tr>
                           <td class="caption">
			   			                                      Top 10 VMs by Balloon Memory Size on 
			   			       			           <xsl:for-each select="detaildiagnostics/data/top/esx">
										   			               <xsl:value-of select="esxname"/> 
										           		           </xsl:for-each>
										           		           (
										           		           <xsl:for-each select="detaildiagnostics/data/top/ip">
										   				   			               <xsl:value-of select="ipadd"/> 
										           		           </xsl:for-each>
										           		           )
										           		           on VC/ESX: <xsl:for-each select="detaildiagnostics/data/top/vc">
										   				   		 <xsl:value-of select="vcname"/> 
										   				   	  </xsl:for-each>
        		           
                                </td>
                           <td align="right">
                               <a target="_new" href="/servlets/com.proactivenet.servlet.HelpServlet?title=TopVMsByBalloonSize">
                                   <img border="0
   " alt="Help" src="/ART/help.gif"/>
                               </a>
                           </td>
                       </tr>
                   </table>
                <br/>
	    <table cellspacing="0" border="0" cellpadding="0">
                       <tr>
                           <td>&nbsp;Server Time&nbsp;&nbsp;&nbsp;</td>
                           <td>
                               <xsl:value-of select='detaildiagnostics/servertime'/>
                           </td>
                           <td>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Agent Time&nbsp;&nbsp;&nbsp;</td>
                           <td>
                               <xsl:value-of select='detaildiagnostics/timecreated'/>
                           </td>
                       </tr>
                </table>
    <br/>
    <xsl:choose>
       <xsl:when test="detaildiagnostics/error[@code='117006']">
       Error code:
       <xsl:value-of select="detaildiagnostics/error/@code"/>
       Error message:
       <xsl:value-of select="detaildiagnostics/error"/>
       </xsl:when>
    <xsl:otherwise>
      <table class="data" width="100%" border="0" cellspacing="0" cellpadding="0">
                    <tr class="header-column first">
                      <th class="header-column first" style="text-align: center;">VM Name</th>
                      <th class="header-column second" style="text-align: center;">VM DNS Name</th>
                      <th class="header-column third" style="text-align: center;">Balloon Memory Size(MB)</th>
                    </tr>
                   <xsl:for-each select="detaildiagnostics/data/top/vm">
                   <tr class="data even">
                     <td class="data even" style="text-align:left;"><xsl:value-of select="vmname"/></td>
                     <td class="data even" style="text-align:left;"><xsl:value-of select="vmdns"/></td>
                     <td class="data even" style="text-align:left;"><xsl:value-of select="value"/></td>
                    </tr>
                    </xsl:for-each>
      </table>
    </xsl:otherwise>
    </xsl:choose>
    </body>
    </html>
  </xsl:template>
 </xsl:stylesheet>
