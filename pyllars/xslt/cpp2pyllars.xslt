<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:xs="http://www.w3.org/2001/XMLSchema"
    exclude-result-prefixes="xs"
    version="1.0">

    <xsl:output method="text"/>

    <xsl:param name="target_namespace" select="'::'"/>
    <xsl:param name="filename" select="'::'"/>
    <xsl:param name="module_name" select="$target_namespace" />

<!-- (1) Look for namespace
(2) (Handle global namespace differenty???)
(3)    Process each Struct/Class
         3a) process each public member
         3b) process each public method
(4)      Handle inheritance
(5)    Process each function
-->

<!--############################################################-->
<!--## Template to tokenize strings                           ##-->
<!--############################################################-->

 <xsl:template name="substring-after-last">
    <xsl:param name="string"/>
    <xsl:param name="char"/>

    <xsl:choose>
      <xsl:when test="contains($string, $char)">
        <xsl:call-template name="substring-after-last">
          <xsl:with-param name="string" select="substring-after($string, $char)"/>
          <xsl:with-param name="char" select="$char"/>
        </xsl:call-template>
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="$string"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

<!--##################
  Templates to generate  type names for various classes of types
  ################### -->

   <xsl:template match="//*" mode="is_incomplete"><xsl:variable name="typeid" select="@id"/><xsl:choose><xsl:when test="//*[@id=$typeid]/@type"><xsl:variable name="subtypeid" select="//*[@id=$typeid]/@type"/><xsl:apply-templates select="//*[@id=$subtypeid]" mode="is_incomplete"/></xsl:when><xsl:otherwise><xsl:value-of select="//*[@id=$typeid]/@incomplete"/></xsl:otherwise></xsl:choose></xsl:template>

   <!-- template to generate namespace prefix (scope name) -->
   <xsl:template match="//*[@context!='']" mode="generate_nsprefix"><xsl:param name="as_return" select="-1"/><xsl:variable name="contextid" select="@context"/><xsl:choose><xsl:when test="@context"><xsl:apply-templates select="//*[@id=$contextid]" mode="generate_nsprefix"/></xsl:when><xsl:otherwise></xsl:otherwise></xsl:choose><xsl:apply-templates select="." mode="generate_name"><xsl:with-param name="as_return" select="$as_return"/></xsl:apply-templates>::</xsl:template>
   <xsl:template match="//*[not(@context) and @type!='']" mode="generate_nsprefix"><xsl:variable name="typeid" select="@type"/><xsl:apply-templates select="//*[@id=$typeid]" mode="generate_nsprefix"/> </xsl:template>
   <xsl:template match="//*" mode="generate_nsprefix_parent"><xsl:variable name="contextid" select="@context"/><xsl:variable name="typeid" select="@type"/><xsl:choose><xsl:when test="$contextid!=''"><xsl:apply-templates select="//*[@id=$contextid]" mode="generate_nsprefix"/></xsl:when><xsl:otherwise><xsl:apply-templates select="//*[@id=$typeid]" mode="generate_nsprefix_parent"/></xsl:otherwise></xsl:choose></xsl:template>


   <xsl:template match="//*[@context!='']" mode="generate_scoped_name"><xsl:param name="as_return" select="'-1'"/><xsl:variable name="contextid" select="@context"/><xsl:apply-templates select="//*[@id=$contextid]" mode="generate_nsprefix"/><xsl:apply-templates select="." mode="generate_name"><xsl:with-param name="as_return" select="$as_return"/></xsl:apply-templates></xsl:template>
  <xsl:template match="//*[not(@context)]" mode="generate_scoped_name"><xsl:param name="as_return" select="'-1'"/><xsl:variable name="typeid" select="@type"/><xsl:variable name="contextid" select="//*[@id=$typeid]/@context"/><xsl:choose><xsl:when test="//*[@id=$contextid]"><xsl:apply-templates select="//*[@id=$contextid]" mode="generate_nsprefix"/></xsl:when><xsl:otherwise><xsl:apply-templates select="//*[@id=$typeid]" mode="generate_nsprefix_parent"/></xsl:otherwise></xsl:choose><xsl:apply-templates select="." mode="generate_name"><xsl:with-param name="as_return" select="$as_return"/></xsl:apply-templates></xsl:template>

   <xsl:template match="//Namespace" mode="generate_name"><xsl:if test="@name!='::'"><xsl:value-of select="@name"/></xsl:if></xsl:template>


   <!-- templates to generat the type name (bare, without scope) -->

   <xsl:template match="//Union" mode="generate_name"><xsl:param name="attrname" select="''"/><xsl:choose><xsl:when test="@name!=''"><xsl:value-of select="@name"/></xsl:when><xsl:when test="$attrname=''"><xsl:variable name="typeid" select="@id"/><xsl:variable name="fieldname" select="//Field[@type=$typeid]/@name"/><xsl:variable name="contextid" select="@context"/><xsl:if test="$fieldname!=''">decltype(<xsl:apply-templates select="//*[@id=$contextid]" mode="generate_name"/>::<xsl:value-of select="$fieldname"/>)</xsl:if></xsl:when><xsl:otherwise>decltype(<xsl:value-of select="$attrname"/>)</xsl:otherwise></xsl:choose></xsl:template>

  <xsl:template match="//FunctionType" mode="generate_name"><xsl:variable name="returntypeid" select="@returns"/><xsl:apply-templates select="//*[@id=$returntypeid]" mode="generate_name"/>(*)(<xsl:apply-templates select="." mode="template_arg_type_list"/>)</xsl:template>

   <xsl:template match="//Function" mode="generate_name"><xsl:value-of select="@name"/></xsl:template>

   <xsl:template match="//ReferenceType" mode="generate_name"><xsl:variable name="typeid" select="@type"/><xsl:apply-templates select="//*[@id=$typeid]" mode="generate_name"/>&amp;</xsl:template>

   <xsl:template match="//PointerType" mode="generate_name"><xsl:variable name="typeid" select="@type"/><xsl:apply-templates select="//*[@id=$typeid]" mode="generate_name"/>*</xsl:template>

   <xsl:template match="//CvQualifiedType" mode="generate_name"><xsl:variable name="typeid" select="@type"/><xsl:apply-templates select="//*[@id=$typeid]" mode="generate_name"/> const</xsl:template>

   <xsl:template match="//ArrayType" mode="generate_name"><xsl:param name="as_return" select="'-11'"/><xsl:variable name="typeid" select="@type"/><xsl:apply-templates select="//*[@id=$typeid]" mode="generate_name"/><xsl:choose><xsl:when test="number($as_return)&lt;=0 or string($as_return)='NaN'">[]</xsl:when><xsl:otherwise>[<xsl:value-of select="$as_return"/>]</xsl:otherwise></xsl:choose></xsl:template>

   <xsl:template match="//Enumeration|//FundamentalType|//Typedef|//Struct|//Class" mode="generate_name"><xsl:variable name="id" select="@id"/><xsl:variable name="name"><xsl:choose><xsl:when test="@name=''"><xsl:value-of select="//Typedef[@type=$id]/@name"/></xsl:when><xsl:otherwise><xsl:value-of select="@name"/></xsl:otherwise></xsl:choose></xsl:variable><xsl:value-of select="$name"/></xsl:template>

<!--#########################
   Templates for generating argument lists
   ##########################-->

   <xsl:template match="//Constructor|//Method|//FunctionType|//Function" mode="template_arg_type_list"><xsl:for-each select="./Argument"><xsl:variable name="typeid" select="@type"/><xsl:apply-templates select="//*[@id=$typeid]" mode="generate_scoped_name"/><xsl:if test="position()!=last()">,</xsl:if></xsl:for-each></xsl:template>

<!--##################
    Generate code for a class constructors, members, and methods
    ##################-->

   <xsl:template match="//Constructor">
     <xsl:param name="classname"></xsl:param>
     <xsl:param name="bareclassname"></xsl:param>

     <xsl:message terminate="no">
###
# Generate Constructor <xsl:value-of select="$classname"/>::<xsl:value-of select="@name"/>
###
     </xsl:message>

     {
        static const char* kwlist[] = {<xsl:for-each select="./Argument">&quot;<xsl:value-of select="@name"/>&quot;, </xsl:for-each>nullptr};
        PythonClassWrapper&lt;<xsl:value-of select="$classname"/>&gt;::addConstructor(kwlist, PythonClassWrapper&lt;<xsl:value-of select="$classname"/>&gt;::create&lt; <xsl:apply-templates select="." mode="template_arg_type_list"/>&gt;);
     }
   </xsl:template>

   <xsl:template match="//Method">
     <xsl:param name="classname"></xsl:param>
     <xsl:param name="bareclassname"></xsl:param>

     <xsl:message terminate="no">
###
# Generate Method <xsl:value-of select="$classname"/>::<xsl:value-of select="@name"/>
###
     </xsl:message>

     <xsl:variable name="returntypeid" select="@returns"/>
     <xsl:variable name="const_modifier"><xsl:if test="number(@const)=1">Const</xsl:if><xsl:if test="number(@static)=1">Class</xsl:if></xsl:variable>
     {
	static const char* kwlist[] = {<xsl:for-each select="./Argument">&quot;<xsl:value-of select="@name"/>&quot;, </xsl:for-each>nullptr};

        PythonClassWrapper&lt;<xsl:value-of select="$classname"/>&gt;::add<xsl:value-of select="$const_modifier"/>Method<xsl:text>
        </xsl:text>&lt;<xsl:text> </xsl:text>name__<xsl:value-of select="@id"/>, <xsl:apply-templates select="//*[@id=$returntypeid]" mode="generate_scoped_name"/><xsl:if test="count(//*[@id=$returntypeid])=0">void</xsl:if><xsl:if test="count(./Argument)>0">, </xsl:if><xsl:apply-templates select="." mode="template_arg_type_list"/><xsl:text> </xsl:text> &gt;<xsl:text>
        </xsl:text>( &amp;<xsl:value-of select="$classname"/>::<xsl:value-of select="@name"/>, kwlist);
     }
   </xsl:template>

<!--###############################
    Generate code for public attributes of structs/classes/unions
    ############################### -->

   <xsl:template match="//Field">
     <xsl:param name="classname"></xsl:param>

     <xsl:message terminate="no">
###
# Generate Attribute  <xsl:value-of select="$classname"/>::<xsl:value-of select="@name"/>
###
     </xsl:message>

     <xsl:variable name="typeid" select="@type"/>
     <xsl:variable name="const_modifier"><xsl:if test="number(@const)=1">Const</xsl:if><xsl:if test="number(@static)=1">Class</xsl:if></xsl:variable>
     <xsl:variable name="fieldname"><xsl:value-of select="@name"/></xsl:variable>
     <xsl:variable name="dim" select="string(number(//*[@id=$typeid]/@max)+1)"/>
     <xsl:if test="$fieldname!='' and $classname!='__va_list_tag'">
     {
        const size_t member_size =  sizeof(<xsl:value-of select="$classname"/>::<xsl:value-of select="@name"/>);
        const size_t type_size = sizeof(<xsl:apply-templates select="//*[@id=$typeid]" mode="generate_scoped_name"><xsl:with-param name="attrname"><xsl:value-of select="$classname"/>::<xsl:value-of select="$fieldname"/></xsl:with-param><xsl:with-param name="as_return" select="$dim"/></xsl:apply-templates>);
        const size_t array_size = member_size/type_size;
        PythonClassWrapper&lt;<xsl:value-of select="$classname"/>&gt;::add<xsl:value-of select="$const_modifier"/>Attribute<xsl:text>
        </xsl:text>&lt;name__<xsl:value-of select="@id"/>, <xsl:apply-templates select="//*[@id=$typeid]" mode="generate_scoped_name"><xsl:with-param name="attrname"><xsl:value-of select="$classname"/>::<xsl:value-of select="$fieldname"/></xsl:with-param><xsl:with-param name="as_return" select="$dim"/></xsl:apply-templates> &gt;<xsl:text>
        </xsl:text>(&amp;<xsl:value-of select="$classname"/>::<xsl:value-of select="@name"/>, array_size);
     }
     </xsl:if>
   </xsl:template>

<!-- ##################################
     Templates to generate required extern name definitons
     ################################## -->

    <xsl:template match="//Struct|//Class|//Union" mode="generate_externs">
       <xsl:variable name="classid" select="@id"/>
       <xsl:for-each select="//Method[@context=$classid]|//Field[@context=$classid]">
extern const char name__<xsl:value-of select="@id"/>[] = &quot;<xsl:value-of select="@name"/>&quot;;
       </xsl:for-each>

       <xsl:apply-templates select="//Struct[@context=$classid and (not(@access) or @access='public')]|//Class[@context=$classid and @access='public']|//Union[@context=$classid and  (not(@access) or @access='public')]" mode="generate_externs"/>

   </xsl:template>

<!-- ################################
    Templates for generating code to wrap various classes of types/functions
    ################################ -->
  <xsl:template match="//Function[@name!='_IO_cookie_init' and @name!='gets']">
     <xsl:param name="nsname"/>

     <xsl:message terminate="no">
###
# Generate Function <xsl:value-of select="@name"/>...
###
     </xsl:message>

// init function <xsl:value-of select="@name"/>
static int init_<xsl:value-of select="@id"/>( PyObject* module){
     int status = 0;
     <xsl:variable name="returntypeid" select="@returns"/>
     <xsl:variable name="return_type"><xsl:choose><xsl:when  test="not(@returns)">void</xsl:when><xsl:otherwise><xsl:apply-templates select="//*[@id=$returntypeid]" mode="generate_scoped_name"/></xsl:otherwise></xsl:choose></xsl:variable>
     <xsl:variable name="is_base_complete"><xsl:choose>
	<xsl:when test="//*[@id=$returntypeid]/@type"><xsl:variable name="typeid" select="//*[@id=$returntypeid]/@type"/><xsl:choose><xsl:when test="//*[@id=$typeid]/@incomplete='1'">false</xsl:when><xsl:otherwise>true</xsl:otherwise></xsl:choose></xsl:when>
        <xsl:otherwise><xsl:choose><xsl:when test="//*[@id=$returntypeid]/@incomplete='1'">false</xsl:when><xsl:otherwise>true</xsl:otherwise></xsl:choose></xsl:otherwise></xsl:choose></xsl:variable>
     {
	static const char* kwlist[] = {<xsl:for-each select="./Argument">&quot;<xsl:value-of select="@name"/>&quot;, </xsl:for-each>nullptr};

        PyObject* wrapper = (PyObject*)PythonFunctionWrapper<xsl:text>
        </xsl:text>&lt; <xsl:value-of select="$is_base_complete"/>, <xsl:value-of select="$return_type"/><xsl:if test="count(./Argument)>0">, </xsl:if><xsl:apply-templates select="." mode="template_arg_type_list"/><xsl:text> &gt;::create
        </xsl:text>(  &quot;<xsl:value-of select="@name"/>&quot;, <xsl:apply-templates select="." mode="generate_scoped_name"/> , kwlist);
	status = wrapper?0:-1;
        if (wrapper &amp;&amp; module) PyModule_AddObject( module, &quot;<xsl:value-of select="@name"/>&quot;, wrapper);
     }
     return status;
}
  </xsl:template>

  <xsl:template match="//Typedef[@name!='__va_list_tag']">
     <xsl:param name="nsname"/>

     <xsl:message terminate="no">
###
# Generate Typedef <xsl:value-of select="@name"/>...
###
     </xsl:message>

//init Typedef <xsl:value-of select="@name"/>
static int init_<xsl:value-of select="@id"/>( PyObject* module){
     int status = 0;
     <xsl:variable name="nsnamebare"><xsl:choose><xsl:when test="$nsname='::'"/><xsl:otherwise><xsl:value-of select="$nsname"/></xsl:otherwise></xsl:choose></xsl:variable>
     <xsl:variable name="typeid" select="@id"/>
    <xsl:if test="@name">
        <xsl:variable name="typealias" select="//*[@id=$typeid]/@name"/>
        <xsl:variable name="basetypeid" select="//*[@id=$typeid]/@type"/>
        <xsl:variable name="is_complete"><xsl:choose><xsl:when test="@incomplete='1'">false</xsl:when><xsl:otherwise>true</xsl:otherwise></xsl:choose></xsl:variable><xsl:text>
    </xsl:text><xsl:if test="@incomplete!='1' or not(@incomplete)">if (PythonClassWrapper&lt; <xsl:value-of select="$nsnamebare"/><xsl:if test="$nsnamebare!=''">::</xsl:if><xsl:value-of select="$typealias"/> &gt;::initialize<xsl:text>
       </xsl:text>(&quot;<xsl:value-of select="@name"/>&quot;, 
                   &quot;<xsl:value-of select="translate(@name,'&lt;&gt;:','___')"/>&quot;,
                   module,
                   &quot;<xsl:apply-templates select="." mode="generate_scoped_name"/>&quot; ) &lt; 0)<xsl:text>{
         PyErr_SetString( PyExc_RuntimeError, "Failed to initialize Python-wrapper-to-reference for '</xsl:text><xsl:value-of select="@name"/><xsl:text>'");
         status = -1;
    }

    </xsl:text></xsl:if><xsl:if test="//*[@id=$basetypeid]/@name!='void'">if (PythonClassWrapper&lt; <xsl:value-of select="$nsnamebare"/>::<xsl:value-of select="$typealias"/>&amp; &gt;::initialize<xsl:text>
                  </xsl:text>(&quot;<xsl:value-of select="@name"/>&amp;&quot;, 
				&quot;<xsl:value-of select="translate(@name,'&lt;&gt;:','___')"/>_ref&quot;, 
				module,
                 		&quot;<xsl:apply-templates select="." mode="generate_scoped_name"/>&amp;&quot;
				 ) &lt; 0)<xsl:text>{
         PyErr_SetString( PyExc_RuntimeError, "Failed to initialize Python-wrapper-to-reference for '</xsl:text><xsl:value-of select="@name"/><xsl:text>'");
         status = -1;
    }

    </xsl:text></xsl:if>if (PythonClassWrapper&lt; <xsl:value-of select="$nsnamebare"/>::<xsl:value-of select="$typealias"/>* &gt;::initialize(&quot;<xsl:value-of select="@name"/>*&quot;,
						 &quot;<xsl:value-of select="translate(@name,'&lt;&gt;:','___')"/>_ptr&quot;,  
						 module,
						 &quot;<xsl:apply-templates select="." mode="generate_scoped_name"/>*&quot;  ) &lt; 0)<xsl:text>{
         PyErr_SetString( PyExc_RuntimeError, "Failed to initialize Python-wrapper-to-pointer for '</xsl:text><xsl:value-of select="@name"/><xsl:text>'");
         status = -1;
    }

    return status;
}

</xsl:text>
     </xsl:if>
  </xsl:template>


  <xsl:template match="//ArrayType[number(@max)&gt;0]">
     <xsl:param name="nsname"/>

     <xsl:message terminate="no">
###
# Generate ArrayType with Extend... <xsl:value-of select="@name"/>
###
     </xsl:message>

//init ArrayType <xsl:value-of select="@name"/>
static int init_<xsl:value-of select="@id"/>( PyObject* module){
     int status = 0;
     <xsl:variable name="nsnamebare"><xsl:choose><xsl:when test="$nsname='::'"/><xsl:otherwise><xsl:value-of select="$nsname"/></xsl:otherwise></xsl:choose></xsl:variable>
     <xsl:if test="@name">
         <xsl:variable name="is_complete"><xsl:choose><xsl:when test="@incomplete='1'">false</xsl:when><xsl:otherwise>true</xsl:otherwise></xsl:choose></xsl:variable>
         <xsl:variable name="typeid" select="@id"/>
         <xsl:variable name="typealias" select="//*[@id=$typeid]/@name"/><xsl:text>
    </xsl:text>if (PythonClassWrapper&lt; <xsl:value-of select="$nsnamebare"/>::<xsl:value-of select="$typealias"/>*, <xsl:value-of select="@max"/>&gt;::initialize<xsl:text>
      </xsl:text>(&quot;<xsl:value-of select="$typealias"/>&quot;,
		&quot;<xsl:value-of select="translate($typealias,'&lt;&gt;:','___')"/>&quot;,
		 module,
                &quot;<xsl:apply-templates select="." mode="generate_scoped_name"/>[<xsl:value-of select="$scopedname"/>]&quot; ) &lt; 0)<xsl:text>{
         PyErr_SetString( PyExc_RuntimeError, "Failed to initialize Python-wrapper-to-reference for '</xsl:text><xsl:value-of select="@name"/><xsl:text>'&quot;);
         status = -1;
    }
    return status;
}
</xsl:text>
     </xsl:if>
  </xsl:template>
   <xsl:template match="//Enumeration">
     <xsl:param name="nsname"/>
     <xsl:variable name="nsnamebare"><xsl:choose><xsl:when test="$nsname='::'"/><xsl:otherwise><xsl:value-of select="$nsname"/></xsl:otherwise></xsl:choose></xsl:variable>
     <xsl:param name="parentclassname" select="''"/>
     <xsl:variable name="enumid" select="@id"/>
    <xsl:message terminate="no">
###
# Generate Class/Struct/Union... <xsl:value-of select="@name"/>
###
     </xsl:message>

// init Enum <xsl:value-of select="@name"/>
static int init_<xsl:value-of select="@id"/>( PyObject* module ){
     int status = 0;
     <xsl:if test="@name!=''">
         <xsl:variable name="enumname"><xsl:apply-templates select="." mode="generate_name"/></xsl:variable>
         <xsl:variable name="scopedenumname"><xsl:apply-templates select="." mode="generate_scoped_name"/></xsl:variable>
     if (PythonClassWrapper&lt; <xsl:value-of select="$nsnamebare"/><xsl:if test="$parentclassname!=''">::<xsl:value-of select="$parentclassname"/></xsl:if>::<xsl:value-of select="@name"/> &gt;::initialize<xsl:text>
          </xsl:text>(&quot;<xsl:value-of select="$enumname"/>&quot;,
          &quot;<xsl:value-of select="translate($enumname,'&lt;&gt;:','___')"/>&quot;,
          module,
          &quot;<xsl:value-of select="$scopedenumname"/>&quot; ) &lt; 0)<xsl:text>{
         PyErr_SetString( PyExc_RuntimeError, "Failed to initialize Python-wrapper-to-enum for '</xsl:text><xsl:value-of select="@name"/><xsl:text>'&quot;);
         status = -1;
    }
     </xsl:text></xsl:if> 
    if(module){
       PyObject* enumval;
    <xsl:for-each select="./EnumValue">
       enumval = PyInt_FromLong( <xsl:value-of select="@init"/>);
       PyModule_AddObject( module, &quot;<xsl:value-of select="@name"/>&quot;, enumval);
 
    </xsl:for-each>
    }
    return status;
}

   </xsl:template>
   <xsl:template match="//Struct[@name!='' and @name!='__va_list_tag']|//Class|//Union">
     <xsl:param name="nsname"/>
     <xsl:param name="parentclassname" select="''"/>
     <xsl:variable name="classid" select="@id"/>
     <xsl:variable name="classname"><xsl:apply-templates select="." mode="generate_name"/></xsl:variable>
     <xsl:variable name="scopedclassname"><xsl:apply-templates select="." mode="generate_scoped_name"/></xsl:variable>
     <xsl:variable name="nsnamebare"><xsl:choose><xsl:when test="$nsname='::'"/><xsl:otherwise><xsl:value-of select="$nsname"/></xsl:otherwise></xsl:choose></xsl:variable>
     <xsl:message terminate="no">
###
# Generate Class/Struct/Union... <xsl:value-of select="@name"/>
###
     </xsl:message>

/////////BEGIN <xsl:value-of select="@name"/>

// define nested type initializers first
      <xsl:apply-templates select="//Typedef[@context=$classid and  (not(@access) or @access='public')]|
			   //Class[@context=$classid and  (not(@access) or @access='public')]|
                	              //Enumeration[@context=$classid and  (not(@access) or @access='public')]|
                                    //Struct[@context=$classid and  (not(@access) or @access='public')]|
                                    //ArrayType[@context=$classid and  (not(@access) or @access='public')]|
				  //Function[@context=$classid and  (not(@access) or @access='public') and count(./Ellipsis)=0]">
        <xsl:with-param name="nsname"><xsl:value-of select="$nsname"/><xsl:if test="$nsname!='::'">::</xsl:if><xsl:value-of select="$classname"/></xsl:with-param>
         <xsl:with-param name="parentclassname"><xsl:if test="$parentclassname!=''"><xsl:value-of select="$parentclassname"/><xsl:if test="$classname!=''">::</xsl:if></xsl:if><xsl:value-of select="$classname"/></xsl:with-param>
      </xsl:apply-templates>


      <xsl:apply-templates select="//Union[@context=$classid and  (not(@access) or @access='public')]">
         <xsl:with-param name="nsname" select="$nsname"/>
         <xsl:with-param name="parentclassname"><xsl:if test="$parentclassname!=''"><xsl:value-of select="$parentclassname"/><xsl:if test="$classname!=''">::</xsl:if></xsl:if><xsl:value-of select="$classname"/></xsl:with-param>
      </xsl:apply-templates>

// init Struct/Class/Union <xsl:value-of select="@name"/>
static int init_<xsl:value-of select="@id"/>( PyObject* module ){
     int status = 0;



     <xsl:variable name="is_complete"><xsl:choose><xsl:when test="@incomplete='1'">false</xsl:when><xsl:otherwise>true</xsl:otherwise></xsl:choose></xsl:variable>
<xsl:choose>
   <xsl:when test="//Field[@type=$classid]|@name!=''">
     <xsl:text>
     //Pyllars generation of class or struct or union  </xsl:text><xsl:value-of select="$classname"/> id: <xsl:value-of select="@id"/>
     <xsl:if test="not(//*[@id=$classid]/@abstract)">
      <xsl:apply-templates select="//Constructor[@context=$classid and  (not(@access) or @access='public')]"><xsl:with-param name="classname"><xsl:if test="$nsname!='::' and $nsname!=''"><xsl:value-of select="$nsname"/>::</xsl:if><xsl:value-of select="$classname"/></xsl:with-param><xsl:with-param name="bareclassname" select="$classname"/></xsl:apply-templates>
</xsl:if>
      <xsl:apply-templates select="//Method[@context=$classid and  (@access='' or @access='public')]"><xsl:with-param name="classname"><xsl:if test="$nsname!='::' and $nsname!=''"><xsl:value-of select="$nsname"/>::</xsl:if><xsl:value-of select="$classname"/></xsl:with-param><xsl:with-param name="bareclassname" select="$classname"/></xsl:apply-templates>

      <xsl:apply-templates select="//Field[@context=$classid and  (not(@access) or @access='public')]"><xsl:with-param name="classname"><xsl:if test="$nsname!='::' and $nsname!=''"><xsl:value-of select="$nsname"/>::</xsl:if><xsl:value-of select="$classname"/></xsl:with-param></xsl:apply-templates>
<xsl:text>

     </xsl:text><xsl:if test="$classname!=''"><xsl:if test="not(@incomplete) or @incomplete!='1'">if (PythonClassWrapper&lt; <xsl:value-of select="$nsnamebare"/><xsl:if test="$nsnamebare!=''">::</xsl:if><xsl:value-of select="$classname"/> &gt;::initialize<xsl:text>
        </xsl:text>(&quot;<xsl:value-of select="$classname"/>&quot;,
			&quot;<xsl:value-of select="translate($classname,'&lt;&gt;:','___')"/>&quot;,
			 module,
			&quot;<xsl:value-of select="$scopedclassname"/>&quot;) &lt; 0)<xsl:text>{
         PyErr_SetString( PyExc_RuntimeError, "Failed to initialize Python-wrapper-to-class/type for '</xsl:text><xsl:value-of select="$classname"/><xsl:text>'&quot;);
         status = -1;
     }
     </xsl:text></xsl:if>if (PythonClassWrapper&lt; <xsl:value-of select="$nsnamebare"/><xsl:if test="$nsnamebare!=''">::</xsl:if><xsl:value-of select="$classname"/>&amp; &gt;::initialize<xsl:text>
       </xsl:text>(&quot;<xsl:value-of select="$classname"/>&amp;&quot;,
			&quot;<xsl:value-of select="translate($classname,'&lt;&gt;:','___')"/>_ref&quot;,
			  module,
			&quot;<xsl:value-of select="$scopedclassname"/>&amp;&quot;) &lt; 0)<xsl:text>{
         PyErr_SetString( PyExc_RuntimeError, "Failed to initialize Python-wrapper-to-reference for '</xsl:text><xsl:value-of select="$classname"/><xsl:text>'&quot;);
         status = -1;
     }
     </xsl:text>if (PythonClassWrapper&lt; <xsl:value-of select="$nsnamebare"/><xsl:if test="$nsnamebare!=''">::</xsl:if><xsl:value-of select="$classname"/>* &gt;::initialize<xsl:text>
       </xsl:text>(&quot;<xsl:value-of select="$classname"/>_ptr&quot;, 
			&quot;<xsl:value-of select="translate($classname,'&lt;&gt;:','___')"/>_ptr&quot;, 
			module,
			&quot;<xsl:value-of select="$scopedclassname"/>*&quot; ) &lt; 0)<xsl:text>{
         PyErr_SetString( PyExc_RuntimeError, "Failed to initialize Python-wrapper-to-pointer for '</xsl:text><xsl:value-of select="$classname"/><xsl:text>'&quot;);
         status = -1;
     }
  </xsl:text>
</xsl:if>

      // BEGIN Nested Pyllars definitions initializations for <xsl:value-of select="@name"/>

      <!-- Nested defintions -->
      <xsl:for-each select="//Typedef[@context=$classid and  (not(@access) or @access='public') and @name!='__va_list_tag']|
	                 //Enumeration[@context=$classid and  (not(@access) or @access='public')]|
                            //Class[@context=$classid and  (not(@access) or @access='public')]|
                            //Struct[@context=$classid and  (not(@access) or @access='public') and @name!='__va_list_tag' and @name!='']|
                            //Union[@context=$classid and  (not(@access) or @access='public')]|
			   //Function[@context=$classid and  (not(@access) or @access='public') and count(./Ellipsis)=0]">
         <xsl:variable name="typeid" select="@id"/>
         <xsl:variable name="name"><xsl:choose><xsl:when test="not(@name)"><xsl:value-of select="//*[@id=$typeid]/@name"/></xsl:when><xsl:otherwise><xsl:value-of select="@name"/></xsl:otherwise></xsl:choose></xsl:variable>

      status |= init_<xsl:value-of select="@id"/>( module );

      // Add to this class/struct/union/enum definition  <xsl:text>
      </xsl:text><xsl:if test="$name!=''">PythonClassWrapper&lt; <xsl:value-of select="$nsnamebare"/>::<xsl:value-of select="$classname"/> &gt;::addClassMember<xsl:text>
           </xsl:text>(&quot;<xsl:value-of select="$name"/>&quot;, (PyObject*)&amp;PythonClassWrapper&lt; <xsl:value-of select="$nsnamebare"/>::<xsl:value-of select="$classname"/>::<xsl:value-of select="$name"/> &gt;::Type);</xsl:if>
      </xsl:for-each>
   </xsl:when>
   <xsl:otherwise>
      //This is an internal anonymous union within <xsl:value-of select="$parentclassname"/> not named as a field, so just add members as direct parent class members:
      <xsl:apply-templates select="//Field[@context=$classid and  (not(@access) or @access='public')]"><xsl:with-param name="classname"><xsl:if test="$nsname!='::' and $nsname!=''"><xsl:value-of select="$nsname"/>::</xsl:if><xsl:value-of select="$parentclassname"/></xsl:with-param></xsl:apply-templates>

   </xsl:otherwise>
</xsl:choose>

      //DONE

      return status;
}

//////////// END <xsl:value-of select="@name"/>

     <xsl:message terminate="no">
###
# DONE Class/Struct/Union... <xsl:value-of select="@name"/>
###
     </xsl:message>

   </xsl:template>

   <xsl:template match="//Namespace">
     <xsl:param name="parentname" select="''"/>

     <xsl:message terminate="no">
###
# Generating namespace <xsl:value-of select="@name"/>
###
     </xsl:message>

     <xsl:variable name="nsname"><xsl:choose><xsl:when test="not(@context)"></xsl:when><xsl:otherwise><xsl:if test="$parentname!='' and $parentname!='::'"><xsl:value-of select="$parentname"/>::</xsl:if><xsl:value-of select="@name"/></xsl:otherwise></xsl:choose></xsl:variable>
     <xsl:variable name="nsid"><xsl:value-of select="@id"/></xsl:variable>
<xsl:text>


    //Pyllars structs/classes for namespace </xsl:text><xsl:value-of select="@name"/>

     <xsl:apply-templates select="//Namespace[@context=$nsid]">
       <xsl:with-param name="parentname"><xsl:value-of select="$nsname"/></xsl:with-param>
     </xsl:apply-templates>
   </xsl:template>

   <xsl:template match="//Namespace" mode="generate_externs">
     <xsl:variable name="nsid" select="@id"/>


//externs for namespace '<xsl:value-of select="@name"/>'
     <xsl:apply-templates select="//Namespace[@context=$nsid]|//Struct[@context=$nsid and  (not(@access) or @access='public')]|//Class[@context=$nsid and  (not(@access) or @access='public')]|//Union[@context=$nsid and  (not(@access) or @access='public')]|//Function[@context=$nsid and  (not(@access) or @access='public') and count(./Ellipsis)=0]" mode="generate_externs"/>
   </xsl:template>

   <xsl:template match="//Namespace">
     <xsl:param name="parentname"></xsl:param>
     <xsl:param name="init_name"><xsl:choose><xsl:when test="not(@context)"></xsl:when><xsl:otherwise><xsl:if test="$parentname!='' and $parentname!='::'"><xsl:value-of select="$parentname"/>::</xsl:if><xsl:value-of select="@name"/></xsl:otherwise></xsl:choose></xsl:param>
     <xsl:variable name="nsname"><xsl:choose><xsl:when test="not(@context) or @name='::'"><xsl:value-of select="$target_namespace"/></xsl:when><xsl:otherwise><xsl:if test="$parentname!='' and $parentname!='::'"><xsl:value-of select="$parentname"/>::</xsl:if><xsl:value-of select="@name"/></xsl:otherwise></xsl:choose></xsl:variable>
     <xsl:variable name="nsid"><xsl:value-of select="@id"/></xsl:variable>
     <xsl:variable name="mod_name">mod_<xsl:value-of  select="translate($nsname,':','_')"/></xsl:variable><xsl:text>

///////////////// BEGIN sub-namespaces </xsl:text><xsl:value-of select="$nsname"/>
//namespace defns for sub-namespaces to <xsl:value-of select="$nsname"/><xsl:text>
     </xsl:text><xsl:apply-templates select="//Namespace[@context=$nsid]">
       <xsl:with-param name="parentname"><xsl:if test="$parentname!='' or $parentname!='::'"><xsl:value-of select="$parentname"/>::</xsl:if><xsl:if test="$nsname!='::'"><xsl:value-of select="$nsname"/></xsl:if></xsl:with-param>
     </xsl:apply-templates>
//
///////////// END sub-namespaces <xsl:value-of select="@name"/>


////////////////  BEGIN sub-elements <xsl:value-of select="@name"/>
//
     <xsl:apply-templates select="//Typedef[@context=$nsid and  (not(@access) or @access='public') and not(@name='_va_list_tag')]|
			    //Class[@context=$nsid and  (not(@access) or @access='public')]|
        		    //Enumeration[@context=$nsid and  (not(@access) or @access='public')]|
                            //Struct[@context=$nsid and  (not(@access) or @access='public')]|
                            //Union[@context=$nsid and  (not(@access) or @access='public')]|
			   //Function[@context=$nsid and  (not(@access) or @access='public') and count(./Ellipsis)=0]">
       <xsl:with-param name="nsname"><xsl:value-of select="$nsname"/></xsl:with-param>
     </xsl:apply-templates><xsl:text>
//
//////////////// END sub-elements </xsl:text><xsl:value-of select="@name"/><xsl:text>

</xsl:text>//initialize namespace <xsl:value-of select="@name"/><xsl:text>
static int init_</xsl:text><xsl:value-of select="@id"/>( PyObject* moduleparent){
     int status = 0;

     PyObject* module = Py_InitModule3(&quot;<xsl:value-of select="$module_name"/>&quot;,
                               nullptr,
			       &quot;pyllars for  namespace <xsl:value-of select="$module_name"/>&quot;);<xsl:text>
     </xsl:text><xsl:choose><xsl:when test="$parentname!='' and $parentname!='::'">
     if(moduleparent){
         PyModule_AddObject( moduleparent, &quot;<xsl:value-of select="@name"/>&quot;, module);
     }
     </xsl:when>
     <xsl:otherwise>
     </xsl:otherwise></xsl:choose><xsl:text>
     </xsl:text>//initialize sub-element initialization

     <xsl:for-each select="//Typedef[@context=$nsid and  (not(@access) or @access='public') and  @name!='__va_list_tag']|
	                 //Class[@context=$nsid and  (not(@access) or @access='public')]|
                             //Enumeration[@context=$nsid and  (not(@access) or @access='public')]|
                            //Struct[@context=$nsid and  (not(@access) or @access='public') and  @name!='__va_list_tag' and @name!='']|
                            //Union[@context=$nsid and  (not(@access) or @access='public')]|
		       	    //Function[@context=$nsid and (not(@access) or @access='public') and @name!='_IO_cookie_init' and @name!='gets' and count(./Ellipsis)=0]|
                            //Namespace[@context= $nsid]">
     status |= init_<xsl:value-of select="@id"/>( module );
      </xsl:for-each>
      <xsl:for-each select="//Variable[@context=$nsid]">
      <xsl:variable name="typeid" select="@type"/>
      <xsl:variable name="is_incomplete"><xsl:apply-templates select="." mode="is_incomplete"/></xsl:variable>
      <xsl:if test="$is_incomplete!='1'">
      <xsl:variable name="typename"><xsl:apply-templates select="//*[@id=$typeid]" mode="generate_scoped_name"/></xsl:variable>
      {  
<xsl:choose>
   <xsl:when test="@init">
         typedef typename std::remove_pointer&lt;typename extent_as_pointer&lt;<xsl:value-of select="$typename"/> &gt;::type&gt;::type basic_type;
         const size_t type_size = sizeof(basic_type);
         const size_t array_size =  sizeof(<xsl:value-of select="@name"/>)/type_size;
   </xsl:when>
   <xsl:otherwise>
         const size_t array_size = -1;/* unknown since extern and/or no init*/
   </xsl:otherwise>
</xsl:choose>
         GlobalVariable::createGlobalVariable&lt; <xsl:value-of select="$typename"/> &gt;
                 ( &quot;<xsl:value-of select="@name"/>&quot;, &quot;<xsl:value-of select="$nsname"/><xsl:if test="$nsname!='::'">::</xsl:if><xsl:value-of select="@name"/>&quot;,
                  (typename extent_as_pointer&lt;<xsl:value-of select="$typename"/>&gt;::type*) &amp;<xsl:value-of select="@name"/>,  module,
                   array_size );
      }

     </xsl:if>
     </xsl:for-each><xsl:text>
  
     return status;
}

</xsl:text><xsl:if test="$parentname='' or $parentname='::'"><xsl:text>

PyMODINIT_FUNC
</xsl:text>init<xsl:value-of select="$init_name"/>(){
    int status = 0;
    initpyllars();
    <xsl:for-each select="//Namespace[@id=$nsid]">
    status |= init_<xsl:value-of select="@id"/>(nullptr);//no module to add to, as this it the top
    </xsl:for-each>
   
    if (status != 0){
        PyErr_SetString(PyExc_RuntimeError, "Failed to initialize");
     }
}
</xsl:if>
   </xsl:template>


   <xsl:template match="//GCC_XML">

<xsl:text>
#include &lt;</xsl:text><xsl:value-of select="$filename"/><xsl:text>&gt;
#include &lt;pyllars/pyllars_pointer.hpp&gt;
#include &lt;pyllars/pyllars_function_wrapper.hpp&gt;
#include &lt;pyllars/pyllars_classwrapper.hpp&gt;
#include &lt;pyllars/pyllars_conversions.hpp&gt;
#include &lt;pyllars/pyllars_globalmembersemantics.hpp&gt;
#include &lt;Python.h&gt;

#include &lt;stdarg.h&gt;
#include &lt;stdio.h&gt;
namespace{
   static va_list _____dummy;
}
using __va_list_tag = typename std::remove_reference&lt;decltype(*_____dummy)&gt;::type;

using namespace __pyllars_internal;


PyMODINIT_FUNC
initpyllars(){

    PyObject* m = Py_InitModule3(&quot;pyllars&quot;, nullptr,
	    		         &quot;global pyllars definitions&quot;);
    PythonClassWrapper&lt; char&amp;&gt;::initialize(&quot;char_ref&quot;, &quot;char_ref&quot;, m);
    PythonClassWrapper&lt;char*&gt;::initialize(&quot;char_ptr&quot;, &quot;char_ptr&quot;, m );
    PythonClassWrapper&lt;short&amp;&gt;::initialize(&quot;short_ref&quot;, &quot;short_ref&quot;, m );
    PythonClassWrapper&lt;short*&gt;::initialize(&quot;short_ptr&quot;, &quot;short_ptr&quot;, m );
    PythonClassWrapper&lt; int&amp;&gt;::initialize(&quot;int_ref&quot;, &quot;int_ref&quot;, m );
    PythonClassWrapper&lt; int*&gt;::initialize(&quot;int_ptr&quot;, &quot;int_ptr&quot;, m );
    PythonClassWrapper&lt; const int&amp;&gt;::initialize(&quot;const_int_ref&quot;, &quot;const_int_ref&quot;, m );
    PythonClassWrapper&lt; const int*&gt;::initialize(&quot;const_int_ptr&quot;, &quot;const_int_ptr&quot;, m );
    PythonClassWrapper&lt;long int&amp;&gt;::initialize(&quot;long_int_ref&quot;, &quot;long_int_ref&quot;, m );
    PythonClassWrapper&lt;long int*&gt;::initialize(&quot;long_int_ptr&quot;,&quot;long_int_ptr&quot;, m );
    PythonClassWrapper&lt;long long int&amp;&gt;::initialize(&quot;long_long_int_ref&quot;,&quot;long_long_int_ref&quot;, m );
    PythonClassWrapper&lt;long long int*&gt;::initialize(&quot;long_long_int_ptr&quot;,&quot;long_long_int_ptr&quot;, m );
    PythonClassWrapper&lt;float&amp;&gt;::initialize(&quot;float_ref&quot;, &quot;float_ref&quot;, m );
    PythonClassWrapper&lt;float*&gt;::initialize(&quot;float_ptr&quot;, &quot;float_ptr&quot;, m );
    PythonClassWrapper&lt;double&amp;&gt;::initialize(&quot;double_ref&quot;, &quot;double_ref&quot;, m );
    PythonClassWrapper&lt;double*&gt;::initialize(&quot;double_ptr&quot;, &quot;double_ptr&quot;, m );


    PythonClassWrapper&lt;unsigned char&amp;&gt;::initialize(&quot;unsigned_char_ref&quot;, &quot;unsigned_char_ref&quot;, m );
    PythonClassWrapper&lt;unsigned char*&gt;::initialize(&quot;unsigned_char_ptr&quot;,&quot;unsigned_char_ptr&quot;, m );
    PythonClassWrapper&lt;unsigned short&amp;&gt;::initialize(&quot;unsigned_short_ref&quot;,&quot;unsigned_short_ref&quot;, m );
    PythonClassWrapper&lt;unsigned short*&gt;::initialize(&quot;unsigned_short_ptr&quot;, &quot;unsigned_short_ptr&quot;, m );
    PythonClassWrapper&lt;unsigned  int&amp;&gt;::initialize(&quot;unsigned_int_ref&quot;, &quot;unsigned_int_ref&quot;, m );
    PythonClassWrapper&lt;unsigned  int*&gt;::initialize(&quot;unsigned_int_ptr&quot;, &quot;unsigned_int_ptr&quot;, m );
    PythonClassWrapper&lt;unsigned long int&amp;&gt;::initialize(&quot;unsigned_long_int_ref&quot;, &quot;unsigned_long_int_ref&quot;, m );
    PythonClassWrapper&lt;unsigned long int*&gt;::initialize(&quot;unsigned_long_int_ptr&quot;, &quot;unsigned_long_int_ptr&quot;, m );
    PythonClassWrapper&lt;unsigned long long int&amp;&gt;::initialize(&quot;unsigned_long_long_int_ref&quot;, &quot;unsigned_long_long_int_ref&quot;, m );
    PythonClassWrapper&lt;unsigned long long int*&gt;::initialize(&quot;unsigned_long_long_int_ptr&quot;, &quot;unsigned_long_long_int_ptr&quot;, m );


    PythonClassWrapper&lt;const char&gt;::initialize(&quot;const_char&quot;, &quot;const_char&quot;, m );
    PythonClassWrapper&lt;const char*&gt;::initialize(&quot;const_char_ptr&quot;, &quot;const_char_ptr&quot;, m );
    PythonClassWrapper&lt;const char* const&gt;::initialize(&quot;const_char_cosnt_ptr&quot;, &quot;const_char_cosnt_ptr&quot;, m );
    PythonClassWrapper&lt;char* const&gt;::initialize(&quot;char_ptr_const&quot;, &quot;char_ptr_const&quot;, m );

    PythonClassWrapper&lt;const char&gt;::initialize(&quot;const_char&amp;&quot;, &quot;const_char_ref&quot;, m );
    PythonClassWrapper&lt;const char*&gt;::initialize(&quot;const_char_ptr&quot;, &quot;const_char_ptr_ref&quot;, m );
    PythonClassWrapper&lt;const char* const &amp;&gt;::initialize(&quot;const_char_const_ptr_ref&quot;, &quot;const_char_const_ptr_ref&quot;, m );
    PythonClassWrapper&lt;char* const &amp;&gt;::initialize(&quot;char_ptr_const_ref&quot;, &quot;char_ptr_const_ref&quot;, m );

}

namespace pyllars{
}
   //Pyllars structs/classes for namespace  </xsl:text><xsl:value-of select="$target_namespace"/>
   <xsl:apply-templates select="//Namespace[@name=$target_namespace]" mode="generate_externs"/>
   <xsl:variable name="targetns"><xsl:choose><xsl:when test="$target_namespace=''">::</xsl:when><xsl:otherwise><xsl:value-of select="$target_namespace"/></xsl:otherwise></xsl:choose></xsl:variable>
   <xsl:apply-templates select="//Namespace[@name=$targetns]">
      <xsl:with-param name="parentname" select="''"/>
      <xsl:with-param name="nsname" select="$target_namespace"/>
      <xsl:with-param name="init_name" select="$module_name"/>
   </xsl:apply-templates>

  </xsl:template>

   <xsl:template match="/">
     <xsl:apply-templates select="GCC_XML"/>
   </xsl:template>

</xsl:stylesheet>
