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
	               			<xsl:value-of select="detaildiagnostics/data/definedconstant"/>
	            		</td>
	           			<td align="right">
	          				<a target="_new" href="/servlets/com.proactivenet.servlet.HelpServlet?title=JVMDetailDiagnostics">
	                        	<img border="0 " alt="Help" src="/ART/help.gif"/>
	                         </a>
	                    </td>
	                </tr>
	            </table>
	            <br/>
	      
	      		<table cellspacing="0" border="0" cellpadding="0">
	            	<tr>
	                	<td>
	                		&nbsp;Server Time&nbsp;&nbsp;&nbsp;
	                	</td>
	                    <td>
	                    	<xsl:value-of select='detaildiagnostics/servertime'/>
	                   	</td>
	                    <td>
	                    	&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Agent Time&nbsp;&nbsp;&nbsp;
	                    </td>
	                    <td>
	                    	<xsl:value-of select='detaildiagnostics/timecreated'/>
	                    </td>
	               </tr>
	            </table>
	            <br/>
            
         	 	<xsl:if test='detaildiagnostics/error'>
		        	<h3>Error displaying data :</h3>
		            <br/>
		            <p>     <xsl:value-of select='detaildiagnostics/error'/>       </p>
                </xsl:if>
         	

         		<table class="data" width="100%" border="0" cellspacing="0" cellpadding="0">
         	     	<thead>
		            	<tr class="header-column first">
		                	<th align="left" class="header-column first" style="text-align:left;">
		                    	<b>Attribute</b>
		                    </th>
		                 	<th align="left" class="header-column first" style="text-align:left;">
		                    	<b>Value</b>
		                    </th>
			        	</tr>
		    		</thead>
         	    	<tbody>	
	       				<xsl:for-each select="detaildiagnostics/data/jvm/attribute">
	       		    			<tr class="data even">
	           						<td class="data even" style="text-align:left;">
		    							<xsl:value-of select="name"/>
		    					 	</td>
		    						<td class="data even" style="text-align:left;">
										<xsl:value-of select="value"/>
		    			        	</td>
		    			     	</tr>
	                   	</xsl:for-each>
               		</tbody>
                </table>	
               	<br/>                    	<br/>            <b>  Active  Threads  Dump </b>           <br/>         	        	<br/>
           	    <table class="data" width="100%" border="0" cellspacing="0" cellpadding="0">
         	   		<thead>
		            	<tr class="header-column first">
		                	<th align="left" class="header-column first" style="text-align:left;">
		                		<b>CPU time (%)</b>
		                	</th> 
		                      	     
		                    <th align="left" class="header-column first" style="text-align:left;">
		                    	<b >Thread ID</b>
		                    </th>
		                                 
					     	<th align="left" class="header-column first" style="text-align:left;">
		                    	<b>Name</b>
		                    </th>
		                    
		                    <th align="left" class="header-column first" style="text-align:left;">
		                       <b >  State </b>
		                    </th>
		                                 
			    		    <th align="left" class="header-column first" style="text-align:left;">
		                    	<b>Is suspended?</b>
		                    </th>
		                    
		                    <th align="left" class="header-column first" style="text-align:left;">
		                    	<b > Stack Trace</b>
		                    </th>
		              	</tr>
		    		</thead>
         	   		<tbody>	
       					<xsl:for-each select="detaildiagnostics/data/jvm/activethreads/dump">
       		    			<tr class="data even">
           						<td class="data even" style="text-align:left;">
									<xsl:value-of select="cputime"/>
	    						</td>
           					
           						<td class="data even" style="text-align:left;">
	    							<xsl:value-of select="id"/>
	    						</td>
							
								<td class="data even" style="text-align:left;">
	    							<xsl:value-of select="name"/>
	    						</td>
	    					
	    						<td class="data even" style="text-align:left;">
	    							<xsl:value-of select="state"/>
	    						</td>
	    					
	    						<td class="data even" style="text-align:left;">
	    							<xsl:value-of select="issuspend"/>
	    						</td>
	    					
	    						<td class="data even" style="text-align:left;">
	    							<xsl:for-each select="stacktrace/traceinfo">
										<xsl:value-of select="."/><br/>
									</xsl:for-each>		
								</td>		    				
	    			   		</tr>
                      </xsl:for-each>
                   </tbody>
              </table>	
 	  </body>
     </html>
   </xsl:template>
   </xsl:stylesheet>
 