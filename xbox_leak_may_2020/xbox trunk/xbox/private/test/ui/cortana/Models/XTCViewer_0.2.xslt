<?xml version="1.0" ?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/TR/WD-xsl">
	<xsl:script language="JScript">
	var icnt=0;
	function increment(i) {	icnt = icnt - 1; return i - icnt;}
	var icnt2=1;
	function incrementParam(i) {	icnt2 = icnt2 - 1; return i - icnt2;}
	var icnt3=1;
	function incrementParam2(i) {	icnt3 = icnt3 - 1; return i - icnt3;}
	var icnt4=1;
	function incrementStart(i) {	icnt4 = icnt4 - 1; return i - icnt4;}
	var icnt5=1;
	function incrementAction(i) {	icnt5 = icnt5 - 1; return i - icnt5;}
	var icnt6=1;
	function incrementInit(i) {	icnt6 = icnt6 - 1; return i - icnt6;}
	function store(x)
	{
	
	}
	function printout()
	{
	//return "thisStuff";
	}
	function toggleMe(n)
	{
	//return n;
	//n.style.display='block';
	}
</xsl:script>
	<xsl:template match="/">
		<HTML>
			<HEAD>
				<META name="author" content="Joe Gallagher [jog]" />
				<STYLE>
			.head {
					Font-family:Tahoma;
					font-size:12pt;
					font-weight:bold;
					}
			.test	{
					Font-Family:Tahoma;
					Font-size:10pt;
					}
			</STYLE>
			</HEAD>
			<BODY>
				<DIV class="head">
					<SPAN style="font-size:18pt; color:darkblue;">
						<U>Model Name:</U>
						<xsl:value-of select="//TRANSITION/@ModelName" />
					</SPAN>
					<BR />
					<DIV style="xcolor:#eee; xbackground-color:#333;font-weight:600;font-size:12pt;xmargin-left:20px;xmargin-right:20px;padding:6">
						<xsl:value-of select="//XTC/@Query" />
					</DIV>
				</DIV>
				<HR />
				<xsl:apply-templates select="//XTC/TEST" />
			</BODY>
		</HTML>
	</xsl:template>
	<xsl:template match="//XTC/TEST">
		<DIV class="test">
			<B style="xcolor:#eee; xbackground-color:#333;font-size:11pt;font-weight:900;padding-left:3; padding-right:3;">Test Case # 
							<xsl:value-of select="@ID" />
						:
							<xsl:value-of select="@Title" />
						</B>
			<BR />
					<BR />
			<xsl:apply-templates select=".//TRANSITION" />
			<PRE style="Font-Family:Tahoma;Font-size:10pt;">
				<xsl:value-of select="./STEP/@Verify" />
			</PRE>
			<HR />
		</DIV>
	</xsl:template>
	<xsl:template match="TRANSITION">
		<xsl:choose>
			<xsl:when test="./ACTION/@Name">
			
					<!-- here -->
					<DIV style="margin-left:20px;margin-bottom:8;font-family:Tahoma;font-size:10pt;xdisplay:none;">
					
					<SPAN style="cursor:hand;font-size:11pt">
						<xsl:attribute name="onclick">
								myAction<xsl:eval>incrementAction(0)</xsl:eval>.style.display='block'
							</xsl:attribute>
						<xsl:apply-templates select="ACTION[@Name]" />
					</SPAN>
					<SPAN style="font-size:9pt">
						(
						<xsl:for-each match="PARAM">
							<xsl:value-of select="./@Value" />
						</xsl:for-each>
						)
					</SPAN>
						<SPAN style="font-size:8pt"> --
						<I>
								<xsl:value-of select="./ACTION/@Comment" />
							</I>
						</SPAN>
					</DIV>
					<DIV style="margin-left:30px;display:none">
						<xsl:attribute name="ID">myAction<xsl:eval>incrementAction(-1)</xsl:eval></xsl:attribute>
						<!--
						<SPAN style="cursor:hand">
						<xsl:attribute name="onclick">
								myStart<xsl:eval>incrementStart(0)</xsl:eval>.style.display='block'
							</xsl:attribute>
							Start State
						</SPAN>
						<BR />
						<DIV xonclick="alert(this.id)" style="margin-left:20px;font-family:Tahoma;font-size:10pt;display:none;">
							<xsl:attribute name="ID">myStart<xsl:eval>incrementStart(-1)</xsl:eval></xsl:attribute>
					<xsl:eval>printout()</xsl:eval>
							StartState
						</DIV>
						<BR />
						-->
						<SPAN style="cursor:hand">
						<xsl:attribute name="onclick">
								myParam<xsl:eval>incrementParam(0)</xsl:eval>.style.display='block'
							</xsl:attribute>
							Parameters In
						
						</SPAN>
						<BR />
						<DIV xonclick="alert(this.id)" style="margin-left:20px;font-family:Tahoma;font-size:10pt;xdisplay:none;">
							<xsl:attribute name="ID">myParam<xsl:eval>incrementParam(-1)</xsl:eval></xsl:attribute>
							<xsl:apply-templates select="PARAM[@Type='In']" />
						</DIV>
						<BR /> <!--
						<xsl:apply-templates select="ACTION[@Name]" />
						<BR />-->
						<DIV style="xmargin-left:30px;xdisplay:none">
						<!--
							<SPAN style="cursor:hand">
						<xsl:attribute name="onclick">
								myParamOut<xsl:eval>incrementParam2(0)</xsl:eval>.style.display='block'
							</xsl:attribute>
							Parameters Out
						
						</SPAN>
							<BR />
							<DIV onclick="alert(this.id)" style="margin-left:20px;font-family:Tahoma;font-size:10pt;display:none;">
								<xsl:attribute name="ID">myParamOut<xsl:eval>incrementParam2(-1)</xsl:eval></xsl:attribute>
								<xsl:apply-templates select="PARAM[@Type='Out']" />
							</DIV>
							<BR />
						-->
							<SPAN style="cursor:hand">
						<xsl:attribute name="onclick">
								myDIV<xsl:eval>increment(0)</xsl:eval>.style.display='block'
							</xsl:attribute>
							End State 
						</SPAN>
							<BR />
							<xsl:apply-templates select="STATE" />
						</DIV>
					</DIV>
		<!-- to here -->
			</xsl:when>
			<xsl:otherwise>
		
			<B style="cursor:hand;margin-left:20px;margin-bottom:8;display:block;font-family:Tahoma;font-size:11pt;xdisplay:none;">
			<xsl:attribute name="onclick">
				myInit<xsl:eval>incrementInit(0)</xsl:eval>.style.display='block'
			</xsl:attribute>
			Initial State
			</B>
			
				
		<DIV xonclick="alert(this.id)" style="margin-left:35px;margin-bottom:8;font-family:Tahoma;font-size:10pt;display:none;">
			<xsl:attribute name="ID">myInit<xsl:eval>incrementInit(-1)</xsl:eval></xsl:attribute>
			<xsl:apply-templates select="./STATE" />
				</DIV>
		
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>
	<xsl:template match="ACTION">
			<B>
				<xsl:value-of select="./@Name" />
			</B>
			
	</xsl:template>
	<xsl:template match="PARAM">
		<!--
		<xsl:value-of select="@Name" />
		:-->
		<xsl:value-of select="@Value" />
		<SPAN style="font-size:8pt"> --
			<I>
				<xsl:value-of select="./@Comment" />
			</I>
			</SPAN>
		<BR />
	</xsl:template>
	<xsl:template match="STATE">
		<DIV xonclick="alert(this.id)" style="margin-left:20px;font-family:Tahoma;font-size:10pt;xdisplay:none;">
			<xsl:attribute name="ID">myDIV<xsl:eval>increment(-1)</xsl:eval></xsl:attribute>
			<xsl:for-each match="STATEVAR">
				<xsl:value-of select="@Name" />
				:
				<xsl:value-of select="@Value" />
				<SPAN style="font-size:8pt"> --
			<I>
						<xsl:value-of select="@Comment" />
					</I>
			</SPAN>
				<BR />
			</xsl:for-each>
			<BR />
		</DIV>
	</xsl:template>
</xsl:stylesheet>
