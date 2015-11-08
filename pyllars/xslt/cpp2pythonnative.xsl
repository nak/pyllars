<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:xs="http://www.w3.org/2001/XMLSchema"
    exclude-result-prefixes="xs"
    version="2.0">
    <xsl:output method="text"/>
    
    <!-- ===================================================== 
                  UTILITY "FUNCTIONS"
         ===================================================== -->
    
    <!-- Utility string replace since not availble in XSLT2.0-->
    <xsl:template name="string-replace-all">
        <xsl:param name="text" />
        <xsl:param name="replace" />
        <xsl:param name="by" />
        <xsl:choose>
            <xsl:when test="contains($text, $replace)">
                <xsl:value-of select="substring-before($text,$replace)" />
                <xsl:value-of select="$by" />
                <xsl:call-template name="string-replace-all">
                    <xsl:with-param name="text"
                        select="substring-after($text,$replace)" />
                    <xsl:with-param name="replace" select="$replace" />
                    <xsl:with-param name="by" select="$by" />
                </xsl:call-template>
            </xsl:when>
            <xsl:otherwise>
                <xsl:value-of select="$text" />
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>
    
    <!-- Get basic type from funcdamental type name -->
    <xsl:template name="get_basic_type">
        <xsl:param name="type"/><xsl:variable name="type2" select="translate($type,' ','_')"/>
        <xsl:value-of select="$type"/>
    </xsl:template>

     <xsl:template name="get_format"><xsl:param name="count"/><xsl:if test="$count &gt;0">O<xsl:call-template name="get_format"><xsl:with-param name="count" select="$count - 1"/></xsl:call-template></xsl:if></xsl:template>
   
      
    <!-- =============================
      * Get the namespace name for the gvien node
      ================================ -->
    <xsl:template match="//*" mode="get_namespace">
        <xsl:variable name="id" select="@id"/>
        <xsl:variable name="context" select="@context"/><xsl:choose>
            <xsl:when test="//Namespace[@id=$id]"><xsl:if test="//Namespace[@id=$context]/@name!='::'"><xsl:apply-templates select="//Namespace[@id=$context]" mode="get_namespace"><xsl:with-param name="id" select="$context"/></xsl:apply-templates>::</xsl:if><xsl:value-of select="@name"/></xsl:when>
            <xsl:when test="@type"><xsl:variable name="type" select="@type"/><xsl:apply-templates select="//*[@id=$type]" mode="get_namespace"/></xsl:when>
            <xsl:otherwise><xsl:apply-templates select="//*[@id=$context]" mode="get_namespace"/></xsl:otherwise>
        </xsl:choose>
    </xsl:template>
    
    <!-- ================================
        Get a C-style namespace prefix
        ============================== -->
    <xsl:template match="//*" mode="get_namespace_prefix">
        <xsl:variable name="namespace"><xsl:apply-templates select="." mode="get_namespace"/></xsl:variable><xsl:choose>
            <xsl:when test="$namespace"><xsl:call-template name="string-replace-all"><xsl:with-param name="text" select="$namespace"/>
                <xsl:with-param name="replace">::</xsl:with-param>
                <xsl:with-param name="by" select="___"/></xsl:call-template>___</xsl:when>
            <xsl:otherwise></xsl:otherwise></xsl:choose></xsl:template>
    
    <!-- ========================================================
                  TYPE NAME GENERATION
         ======================================================== -->
    
    <!-- ===============================
        Generate the python C API type name associated with a type
        ================================ -->
    <xsl:template match="Typedef" mode="gen_C_typename"><xsl:variable name="type" select="@type"/><xsl:variable name="context" select="@context"/><xsl:if test="//Class[@id=$context]|//Struct[@id=$context]"><xsl:apply-templates select="//Class[@id=$context]|//Struct[@id=$context]" mode="gen_C_typename"/></xsl:if><xsl:apply-templates select="//*[@id=$type]" mode="gen_C_typename"/></xsl:template>

    <xsl:template match="CvQualifiedType" mode="gen_C_typename"><xsl:variable name="type" select="@type"/><xsl:apply-templates select="//*[@id=$type]" mode="gen_C_typename"/> const</xsl:template>
    
    <xsl:template match="ReferenceType" mode="gen_C_typename"><xsl:variable name="type" select="@type"/><xsl:apply-templates select="//*[@id=$type]" mode="gen_C_typename"/>&amp;</xsl:template>
     
    <xsl:template match="PointerType" mode="gen_C_typename"><xsl:variable name="typeid" select="@type"/><xsl:apply-templates mode="gen_C_typename" select="//*[@id=$typeid]"></xsl:apply-templates> *</xsl:template>
    
    <xsl:template match="FundamentalType" mode="gen_C_typename"><xsl:call-template name="get_basic_type"><xsl:with-param name="type" select="@name"/></xsl:call-template></xsl:template>

    <xsl:template match="Enumeration" mode="gen_C_typename"><xsl:variable name="context" select="@context"/><xsl:apply-templates mode="get_namespace" select="//*[@id=$context]"/>::<xsl:variable name="context" select="@context"/><xsl:if test="//Class[@id=$context]|//Struct[@id=$context]"><xsl:apply-templates select="//Class[@id=$context]|//Struct[@id=$context]" mode="gen_C_typename"><xsl:with-param name="nons">1</xsl:with-param></xsl:apply-templates>::</xsl:if><xsl:value-of select="@name"/></xsl:template>

    <xsl:template match="FunctionType" mode="gen_C_typename"><xsl:variable name="id" select="@id"/>
        <xsl:variable name="name" select="//Typedef[@type=$id][1]/@name"/>
        <xsl:variable name="id2" select="//PointerType[@type=$id]/@id"/>
        <xsl:variable name="name2" select="//Typedef[@type=$id2][1]/@name"/>
        <xsl:variable name="context" select="//Typedef[@type=$id2]/@context"/>
        <xsl:variable name="ns"><xsl:apply-templates mode="get_namespace" select="//*[@id=$context]"/></xsl:variable>
        <xsl:variable name="classname"><xsl:if test="//Class[@id=$context]|//Struct[@id=$context]"><xsl:variable name="clname"><xsl:apply-templates select="//Class[@id=$context]|//Struct[@id=$context]" mode="gen_C_typename"><xsl:with-param name="nons">1</xsl:with-param></xsl:apply-templates></xsl:variable><xsl:if test="$clname!='' and $clname!='::'"><xsl:value-of select="$clname"/>::</xsl:if></xsl:if></xsl:variable>
        <xsl:value-of select="$ns"/>::<xsl:value-of select="$classname"/>
        <xsl:value-of select="$name2"/></xsl:template>
    <xsl:template match="FunctionType" mode="gen_name"><xsl:variable name="id" select="@id"/><xsl:variable name="name" select="translate(//Typedef[@type=$id][1]/@name,'&lt;&gt;,: ','_____')"/><xsl:variable name="id2" select="//PointerType[@type=$id]/@id"/><xsl:variable name="name2" select="//Typedef[@type=$id2][1]/@name"/><xsl:variable name="context" select="//Typedef[@type=$id2]/@context"/><xsl:apply-templates mode="get_namespace_prefix" select="//*[@id=$context]"></xsl:apply-templates><xsl:if test="//Class[@id=$context]|//Struct[@id=$context]"><xsl:apply-templates select="//Class[@id=$context]|//Struct[@id=$context]" mode="gen_C_typename"><xsl:with-param name="nons">1</xsl:with-param></xsl:apply-templates>___</xsl:if><xsl:value-of select="translate($name2,'&lt;&gt;:, ','_____')"/></xsl:template>
    <xsl:template match="FunctionType" mode="gen_signature"><xsl:variable name="returns" select="@returns"/><xsl:apply-templates select="//*[@id=$returns]" mode="gen_C_typename"/>(<xsl:for-each select="./Argument"><xsl:variable name="type" select="@type"/><xsl:apply-templates select="//*[@id=$type]" mode="gen_C_typename"/><xsl:if test="position()!=last()">, </xsl:if></xsl:for-each>)</xsl:template>

    <xsl:template match="Class|Struct" mode="gen_C_typename"><xsl:param name="nons"></xsl:param><xsl:if test="$nons!='1'"><xsl:apply-templates select="." mode="get_namespace"/>::</xsl:if><xsl:choose><xsl:when test="@name"><xsl:value-of select="@name"/></xsl:when><xsl:otherwise>Anon_<xsl:value-of select="@id"/></xsl:otherwise></xsl:choose></xsl:template>
    
    <!-- Generate C names of Python wrapper objects.  How these are defined must match how they are called in gen_callable templates.
         For example, PointerType python wrappre type name is same as the underlying type:  but the callable generated for pointer
         types is generated without a dereference of the underlying object in the python wrapper-->
    
    <xsl:template match="CvQualifiedType|PointerType|ReferenceType|Typedef" mode="gen_python_wrapper_C_typename"><xsl:variable name="type" select="@type"/><xsl:apply-templates select="//*[@id=$type]"/></xsl:template>
    
    <xsl:template match="FunctionType" mode="gen_python_wrapper_C_typename">TBD</xsl:template>
    
    <xsl:template match="FundamentalType" mode="gen_python_wrapper_C_typename"><xsl:call-template name="get_basic_type"><xsl:with-param name="type" select="@name"/></xsl:call-template></xsl:template>
    
    
    <xsl:template match="Class|Struct" mode="gen_python_wrapper_C_typename">        <xsl:variable name="classid" select="@id"/>
        <xsl:variable name="namespace"><xsl:apply-templates select="." mode="get_namespace"></xsl:apply-templates></xsl:variable>
        <xsl:variable name="namespace_prefix"><xsl:apply-templates select="." mode="get_namespace_prefix"/></xsl:variable><xsl:value-of select="$namespace_prefix"/><xsl:choose><xsl:when test="@name"><xsl:value-of select="translate(@name,'&lt;&gt;:, ','_____')"/></xsl:when><xsl:otherwise>Anon_<xsl:value-of select="@id"/></xsl:otherwise></xsl:choose></xsl:template>
    
    <!-- Tell if type is a funcdamental type or composite type -->
    
    <xsl:template match="Typedef" mode="is_fundamental"><xsl:variable name="type" select="@type"/><xsl:apply-templates select="//*[@id=$type]" mode="is_fundamental"/></xsl:template>
    
    <xsl:template match="FundamentalType" mode="is_fundamental">true</xsl:template>
    
    <!-- =================================================
         Convert python C wrapper/object to plain C 
         =================================================-->
    
    <!-- Code generation for type checks -->
    
    <xsl:template match="Typedef" mode="check_type"><xsl:param name="paramname"/><xsl:param name="statusvar">status</xsl:param><xsl:variable name="typeid" select="@type"/><xsl:apply-templates select="//*[@id=$typeid]" mode="check_type"><xsl:with-param name="paramname" select="$paramname"/><xsl:with-param name="statusvar" select="$statusvar"/></xsl:apply-templates></xsl:template>
    
    <xsl:template match="FundamentalType" mode="check_type">
        <xsl:param name="statusvar">status</xsl:param>
        <xsl:param name="paramname"/><xsl:variable name="type0"><xsl:call-template name="get_basic_type"><xsl:with-param name="type" select="@name"/></xsl:call-template></xsl:variable>
        <xsl:variable name="type" select="translate($type0,' ','_')"/><xsl:choose><xsl:when test="$type='long_int' or $type='long_unsigned_int' or $type='unsigned___int128' or $type='unsigned_int128' or $type='__int128'"><xsl:text>
            </xsl:text>if (!PyLong_Check(<xsl:value-of select="$paramname"/> )){ <xsl:value-of select="$statusvar"/> = -1; }</xsl:when>
            <xsl:when test="$type='unsigned_int' or $type='short_int' or $type='signed_char' or $type='char' or $type='unsigned_char' or $type='unsigned_int'or $type='short_unsigned_int' or $type='int'"><xsl:text>
                </xsl:text>if(!PyInt_Check(<xsl:value-of select="$paramname"/>)){ <xsl:value-of select="$statusvar"/> = -1;}
            </xsl:when>
            <xsl:when test="$type='bool'"><xsl:text>
                </xsl:text>if(!PyBool_Check(<xsl:value-of select="$paramname"/>)){ <xsl:value-of select="$statusvar"/> = -1;}
            </xsl:when>
            <xsl:when test="$type='float' or $type='double'"><xsl:text>
                </xsl:text>if(!PyFloat_Check(<xsl:value-of select="$paramname"/>)){ <xsl:value-of select="$statusvar"/> = -1;}
            </xsl:when>
            <xsl:when test="$type='void'"></xsl:when>
           <xsl:otherwise>if(true){
                Unknown_type_<xsl:value-of select="$type"/></xsl:otherwise></xsl:choose>
    </xsl:template>
    
    <xsl:template match="FunctionType" mode="check_type"><xsl:param name="paramname"/><xsl:param name="statusvar">status</xsl:param>
        if(!(<xsl:value-of select="$paramname"/> &amp;&amp; (PyFunction_Check( <xsl:value-of select="$paramname"/>) || <xsl:value-of select="$paramname"/>->tp_call) ) ){ <xsl:value-of select="$statusvar"/> = -1;}
    </xsl:template>
    
    <xsl:template match="CvQualifiedType|PointerType|ReferenceType" mode="check_type"><xsl:param name="paramname"/><xsl:param name="statusvar">status</xsl:param><xsl:variable name="typeid" select="@type"/><xsl:apply-templates select="//*[@id=$typeid]" mode="check_type"><xsl:with-param name="paramname" select="$paramname"></xsl:with-param><xsl:with-param name="statusvar" select="$statusvar"/></xsl:apply-templates></xsl:template>
    
    <xsl:template match="Class|Struct" mode="check_type"><xsl:param name="paramname"/><xsl:param name="statusvar">status</xsl:param><xsl:variable name="pytype"><xsl:apply-templates select="." mode="gen_python_wrapper_C_typename"/></xsl:variable><xsl:text>
                </xsl:text>if (!checkType(&amp;<xsl:value-of select="$pytype"/>Type, <xsl:value-of select="$paramname"/> )){ <xsl:value-of select="$statusvar"/> = -1;}
    </xsl:template>

    <xsl:template match="Argument" mode="check_type"><xsl:variable name="type" select="@type"/><xsl:variable name="name"><xsl:choose>
        <xsl:when test="@name"><xsl:value-of select="@name"/></xsl:when><xsl:otherwise>Anon_<xsl:value-of select="position()"/></xsl:otherwise>
    </xsl:choose></xsl:variable><xsl:apply-templates mode="check_type" select="//*[@id=$type]"><xsl:with-param name="paramname" select="$name"></xsl:with-param><xsl:with-param name="statusvar">status</xsl:with-param></xsl:apply-templates></xsl:template>
    
    <!-- Code generation to convert python object to C object -->
    
    <xsl:template match="Typedef" mode="convert_python_to_C"><xsl:param name="paramname"/><xsl:variable name="type" select="@type"/><xsl:apply-templates select="//*[@id=$type]" mode="convert_python_to_C"><xsl:with-param name="paramname" select="$paramname"/></xsl:apply-templates></xsl:template>

    <xsl:template match="CvQualifiedType" mode="convert_python_to_C"><xsl:param name="paramname"/><xsl:param name="ctypename"><xsl:apply-templates select="." mode="gen_C_typename"/></xsl:param><xsl:variable name="typeid" select="@type"/>
        <xsl:apply-templates select="//*[@id=$typeid]" mode="convert_python_to_C">
            <xsl:with-param name="paramname" select="$paramname"/>
            <xsl:with-param name="ctypename">
                <xsl:value-of select="$ctypename"/></xsl:with-param></xsl:apply-templates></xsl:template>

    <xsl:template match="PointerType" mode="convert_python_to_C"><xsl:param name="paramname"/>
        <xsl:param name="ctypename"><xsl:apply-templates select="." mode="gen_C_typename"/></xsl:param><xsl:variable name="typeid" select="@type"/>      
        <xsl:apply-templates select="//*[@id=$typeid]" mode="convert_python_to_C"><xsl:with-param name="deref"></xsl:with-param><xsl:with-param name="paramname" select="$paramname"/><xsl:with-param name="ctypename"><xsl:value-of select="$ctypename"/></xsl:with-param></xsl:apply-templates></xsl:template>
     
    <xsl:template match="ReferenceType" mode="convert_python_to_C"><xsl:param name="paramname"/><xsl:param name="ctypename"><xsl:apply-templates mode="gen_C_typename" select="."/></xsl:param><xsl:variable name="typeid" select="@type"/><xsl:apply-templates select="//*[@id=$typeid]" mode="convert_python_to_C"><xsl:with-param name="deref">*</xsl:with-param><xsl:with-param name="paramname" select="$paramname"/><xsl:with-param name="ctypename"><xsl:value-of select="$ctypename"/></xsl:with-param></xsl:apply-templates></xsl:template>
    
    <xsl:template match="FundamentalType" mode="convert_python_to_C"><xsl:param name="paramname"/><xsl:variable name="type"><xsl:call-template name="get_basic_type"><xsl:with-param name="type" select="@name"/></xsl:call-template></xsl:variable><xsl:text>
                    </xsl:text><xsl:choose><xsl:when test="$type='long unsigned int' or $type='unsigned __int128' or $type='unsigned int128' or $type='__int128'">
               <xsl:value-of select="$type"/> <xsl:value-of select="$paramname"/>_C = PyInt_Check(<xsl:value-of select="$paramname"/>)? PyInt_AsLong(<xsl:value-of select="$paramname"/>):PyLong_AsLong(<xsl:value-of select="$paramname"/>);
            </xsl:when>
        <xsl:when test="$type='unsigned int' or $type='long int' or $type='short int' or $type='signed char' or $type='char' or $type='unsigned char' or $type='unsigned int'or $type='short unsigned int' or $type='int'">
               <xsl:value-of select="$type"/> <xsl:value-of select="$paramname"/>_C = PyInt_Check(<xsl:value-of select="$paramname"/>)? PyInt_AsLong(<xsl:value-of select="$paramname"/>):PyLong_AsLong(<xsl:value-of select="$paramname"/>);</xsl:when>
        <xsl:when test="$type='bool'">
                bool  <xsl:value-of select="$paramname"/>_C = (PyInt_Check(<xsl:value-of select="$paramname"/>)? PyInt_AsLong(<xsl:value-of select="$paramname"/>):PyLongAsLong(<xsl:value-of select="$paramname"/>))!=0</xsl:when>
            <xsl:when test="$type='double'">
                double  <xsl:value-of select="$paramname"/>_C = PyFloat_AsDouble(<xsl:value-of select="$paramname"/>);</xsl:when>
            <xsl:when test="$type='float'">
                float  <xsl:value-of select="$paramname"/>_C = PyFloat_AsFloat(<xsl:value-of select="$paramname"/>);</xsl:when>
            <xsl:when test="$type='void'"></xsl:when>
        <xsl:otherwise>if(true){
                Unknown_type_<xsl:value-of select="$type"/></xsl:otherwise></xsl:choose><xsl:text>     
                </xsl:text>
    </xsl:template>
    
    <xsl:template match="FundamentalType" mode="convert_C_to_python"><xsl:param name="paramname"/><xsl:variable name="type"><xsl:call-template name="get_basic_type"><xsl:with-param name="type" select="@name"/></xsl:call-template></xsl:variable><xsl:text>
                    </xsl:text><xsl:choose><xsl:when test="$type='long unsigned int' or $type='unsigned __int128' or $type='unsigned int128' or $type='__int128'">
                        <xsl:value-of select="$type"/> <xsl:value-of select="$paramname"/>_py = PyLong_FromLong(<xsl:value-of select="$paramname"/>);
                    </xsl:when>
                        <xsl:when test="$type='unsigned int' or $type='long int' or $type='short int' or $type='signed char' or $type='char' or $type='unsigned char' or $type='unsigned int'or $type='short unsigned int' or $type='int'">
                            <xsl:value-of select="$type"/> <xsl:value-of select="$paramname"/>_py = PyLong_FromInt(<xsl:value-of select="$paramname"/>);</xsl:when>
                            <xsl:when test="$type='bool'">
                                bool  <xsl:value-of select="$paramname"/>_py = PyInt_FromInt(<xsl:value-of select="$paramname"/>);</xsl:when>
                            <xsl:when test="$type='double'">
                                double  <xsl:value-of select="$paramname"/>_py = PyFloat_FromDouble(<xsl:value-of select="$paramname"/>);</xsl:when>
                            <xsl:when test="$type='float'">
                                float  <xsl:value-of select="$paramname"/>_py = PyFloat_FromFloat(<xsl:value-of select="$paramname"/>);</xsl:when>
                            <xsl:when test="$type='void'"></xsl:when>
                            <xsl:otherwise>if(true){
                                Unknown_type_<xsl:value-of select="$type"/></xsl:otherwise></xsl:choose><xsl:text>     
                </xsl:text>        
    </xsl:template>
    
    <xsl:template match="Struct|Class" mode="convert_python_to_C"><xsl:param name="deref">*</xsl:param><xsl:param name="ctypename"><xsl:apply-templates select="." mode="gen_C_typename"/>&amp;</xsl:param><xsl:param name="paramname"/><xsl:variable name="pytype"><xsl:apply-templates select="." mode="gen_python_wrapper_C_typename"/></xsl:variable><xsl:text>
                   </xsl:text><xsl:value-of select="$ctypename"/><xsl:text> </xsl:text><xsl:value-of select="$paramname"/>_C = <xsl:value-of select="$deref"/>((<xsl:value-of select="$pytype"/>*)<xsl:value-of select="$paramname"/>)->_Cobject;
    </xsl:template>
    
    <xsl:template match="FunctionType" mode="convert_python_to_C">
        <xsl:param name="paramname"/><xsl:variable name="returns" select="@returns"/><xsl:variable name="name"><xsl:apply-templates mode="gen_name" select="."/></xsl:variable>
            <xsl:value-of select="$name"/>_CL * wrapperCB = new <xsl:value-of select="$name"/>_CL( <xsl:value-of select="$paramname"/>) ;
            <xsl:value-of select="$paramname"/>_C = wrapperCB-&gt;get_C_callback();       
            //TODO: FIX MEMORY LEAK!
    </xsl:template>
    
    <xsl:template match="Argument" mode="convert_python_to_C"><xsl:variable name="type" select="@type"/><xsl:variable name="name"><xsl:choose>
        <xsl:when test="@name"><xsl:value-of select="@name"/></xsl:when><xsl:otherwise>Anon_<xsl:value-of select="position()"/></xsl:otherwise>
    </xsl:choose></xsl:variable><xsl:apply-templates mode="convert_python_to_C" select="//*[@id=$type]"><xsl:with-param name="paramname" select="$name"></xsl:with-param></xsl:apply-templates></xsl:template>
    
    <xsl:template match="Argument" mode="convert_C_to_python"><xsl:variable name="type" select="@type"/><xsl:variable name="is_fundamental"><xsl:apply-templates select="//*[@id=$type]" mode="is_fundamental"/></xsl:variable><xsl:variable name="name"><xsl:choose>
        <xsl:when test="@name"><xsl:value-of select="@name"/></xsl:when><xsl:otherwise>Anon_<xsl:value-of select="position()"/></xsl:otherwise>
    </xsl:choose></xsl:variable><xsl:if test="$is_fundamental='true'"><xsl:apply-templates mode="convert_C_to_python" select="//*[@id=$type]"><xsl:with-param name="paramname" select="$name"></xsl:with-param></xsl:apply-templates></xsl:if></xsl:template>
    
    <!-- ==========================================
         C to Python conversion
         ========================================== -->
    <xsl:template match="FundamentalType" mode="convert_C_to_python">
        <xsl:param name="paramname"/>
        <xsl:variable name="typeid"  select="@type"/>
        <xsl:variable name="type"><xsl:call-template name="get_basic_type"><xsl:with-param name="type" select="$typeid"/></xsl:call-template></xsl:variable>
        <xsl:choose>
            <xsl:when test="$type='__int128'">PyLong_FromLong(<xsl:value-of select="$paramname"/>);</xsl:when>
            <xsl:when test="$type='__uint28'">PyLong_FromLong(<xsl:value-of select="$paramname"/>);</xsl:when>
            <xsl:when test="$type='long_int'">PyInt_FromLong(<xsl:value-of select="$paramname"/>);</xsl:when>
            <xsl:when test="$type='long_unsigned_int'">PyLong_FromLong(<xsl:value-of select="$paramname"/>);</xsl:when>
            <xsl:when test="$type='signed_char'">PyInt_FromLong(<xsl:value-of select="$paramname"/>);</xsl:when>
            <xsl:when test="$type='char'">PyInt_FromLong(<xsl:value-of select="$paramname"/>);</xsl:when>
            <xsl:when test="$type='short_int'">PyInt_FromLong(<xsl:value-of select="$paramname"/>);</xsl:when>
            <xsl:when test="$type='unsigned_char'">PyInt_FromLong(<xsl:value-of select="$paramname"/>);</xsl:when>
            <xsl:when test="$type='short_unsigned_int'">PyInt_FromLong(<xsl:value-of select="$paramname"/>);</xsl:when>
            <xsl:when test="$type='int'">PyInt_FromLong(<xsl:value-of select="$paramname"/>);</xsl:when>            
            <xsl:when test="$type='unsigned_int'">PyInt_FromLong(<xsl:value-of select="$paramname"/>);</xsl:when>
            <xsl:when test="$type='bool'"><xsl:value-of select="$paramname"/>?Py_True:Py_False;</xsl:when>
            <xsl:when test="$type='float'">PyFloat_FromDouble(<xsl:value-of select="$paramname"/>);</xsl:when>
            <xsl:when test="$type='double'">PyFloat_FromDouble(<xsl:value-of select="$paramname"/>);</xsl:when>
            <xsl:when test="$type='void'">Py_None</xsl:when>
            <xsl:when test="$type='unsigned_int128'">PyLong_FromLong(<xsl:value-of select="$paramname"/>);</xsl:when>
            <xsl:when test="$type='unsigned__int128'">PyLong_FromLong(<xsl:value-of select="$paramname"/>);</xsl:when>
            <xsl:when test="$type='unsigned___int128'">PyLong_FromLong(<xsl:value-of select="$paramname"/>);</xsl:when>
            <xsl:otherwise>Unknown_type_<xsl:value-of select="$type"/></xsl:otherwise>
        </xsl:choose>        
    </xsl:template>
    
    <xsl:template match="CvQualifiedType" mode="convert_C_to_python"><xsl:param name="paramname"/><xsl:variable name="typeid" select="@type"/>
        <xsl:apply-templates select="//*[@id=$typeid]" mode="convert_C_to_python"><xsl:with-param name="paramname" select="$paramname"/></xsl:apply-templates>
    </xsl:template>
    
    <xsl:template match="ReferenceType" mode="convert_C_to_python"><xsl:param name="paramname"/><xsl:variable name="typeid" select="@type"/><xsl:variable name="is_fundamental"><xsl:apply-templates select="//*[@id=$typeid]" mode="is_fundamental"/></xsl:variable>
    <xsl:choose>
        <xsl:when test="$is_fundamental"><xsl:apply-templates select="//*[@id=$typeid]" mode="convert_C_to_python"><xsl:with-param name="paramname" select="$paramname"/></xsl:apply-templates></xsl:when>
        <xsl:otherwise><xsl:variable name="py_classname"><xsl:apply-templates select="." mode="gen_python_wrapper_C_typename"/></xsl:variable>
            PyObject_CallObject((PyObject *) &amp; <xsl:value-of select="$py_classname"/>Type, NULL);
            if(<xsl:value-of select="$paramname"/>_py){
                delete ((<xsl:value-of select="$py_classname"/>*) <xsl:value-of select="$paramname"/>_py)-&gt;_Cobject) ;
                ((<xsl:value-of select="$py_classname"/>*) <xsl:value-of select="$paramname"/>_py)-&gt;_Cobject = &amp;<xsl:value-of select="$paramname"/>;
                ((<xsl:value-of select="$py_classname"/>*) <xsl:value-of select="$paramname"/>_py)-&gt;_is_c_reference = true;
            }
        </xsl:otherwise>
    </xsl:choose></xsl:template>
    
    <xsl:template match="Class|Struct" mode="convert_C_to_python"><xsl:param name="paramname"/><xsl:variable name="typeid" select="@type"/>
        <xsl:variable name="cname"><xsl:apply-templates select="." mode="gen_C_typename"/></xsl:variable>
        <xsl:variable name="py_classname"><xsl:apply-templates select="." mode="gen_python_wrapper_C_typename"/></xsl:variable>
                        PyObject_CallObject((PyObject *) &amp; <xsl:value-of select="$py_classname"/>Type, NULL);
                        if(<xsl:value-of select="$paramname"/>_py){
                            delete ((<xsl:value-of select="$py_classname"/>*) <xsl:value-of select="$paramname"/>_py)-&gt;_Cobject ;
                            ((<xsl:value-of select="$py_classname"/>*) <xsl:value-of select="$paramname"/>_py)-&gt;_Cobject = new <xsl:value-of select="$cname"/>(<xsl:value-of select="$paramname"/>);
                        }</xsl:template>
    
    <xsl:template match="PointerType" mode="convert_C_to_python"><xsl:param name="paramname"/><xsl:variable name="typeid" select="@type"/><xsl:variable name="is_fundamental"><xsl:apply-templates select="//*[@id=$typeid]" mode="is_fundamental"/></xsl:variable>
        <xsl:choose>
            <xsl:when test="$is_fundamental='true'"><xsl:apply-templates select="//*[@id=$typeid]" mode="convert_C_to_python"><xsl:with-param name="paramname" select="$paramname"/></xsl:apply-templates></xsl:when>
            <xsl:otherwise><xsl:variable name="py_classname"><xsl:apply-templates select="//*[@id=$typeid]" mode="gen_python_wrapper_C_typename"/></xsl:variable>
                       PyObject_CallObject((PyObject *) &amp; <xsl:value-of select="$py_classname"/>Type, NULL);
                       if(<xsl:value-of select="$paramname"/>_py){
                           delete ((<xsl:value-of select="$py_classname"/>*) <xsl:value-of select="$paramname"/>_py)-&gt;_Cobject ;
                           ((<xsl:value-of select="$py_classname"/>*) <xsl:value-of select="$paramname"/>_py)-&gt;_Cobject = <xsl:value-of select="$paramname"/>;
                       }</xsl:otherwise></xsl:choose></xsl:template>
    
    <!-- =====================================================
                     GENERATE RETURN OBJECT GENERATION
         ===================================================== -->
    <!-- Generate function callback Python logic -->
    <xsl:template match="FunctionType" mode="gen_python_C_decl"><xsl:variable name="returns" select="@returns"/>
        <xsl:variable name="name1"><xsl:apply-templates mode="gen_name" select="."/></xsl:variable>
        <xsl:variable name="name" select="translate($name1,'&lt;&gt;:, ','_____')" />
        <xsl:variable name="cname"><xsl:apply-templates mode="gen_C_typename" select="."/></xsl:variable>
        <xsl:variable name="resulttypename"><xsl:apply-templates select="//*[@id=$returns]" mode="gen_C_typename"/></xsl:variable><xsl:text>
</xsl:text><xsl:if test="$name!=''">class <xsl:value-of select="$name"/>_CL{<xsl:text>
public:

    </xsl:text><xsl:value-of select="$name"/>_CL( PyObject* pycb);<xsl:text>
    
    </xsl:text><xsl:value-of select="$cname"/><xsl:text> get_C_callback();
    
private:
    </xsl:text><xsl:apply-templates select="//*[@id=$returns]" mode="gen_C_typename"/> callback(<xsl:apply-templates select="./Argument" mode="gen_arg"/>);<xsl:text>

    std::function&lt;</xsl:text><xsl:apply-templates mode="gen_signature" select="."/><xsl:text>&gt; _C_callback;
    PyObject* _pycb;
};
</xsl:text></xsl:if>
    </xsl:template>
    
    <xsl:template match="FunctionType" mode="gen_python_C_defn"><xsl:variable name="returns" select="@returns"/>
        <xsl:variable name="name1"><xsl:apply-templates mode="gen_C_typename" select="."/></xsl:variable>
        <xsl:variable name="name" select="translate($name1,'&lt;&gt;:, ','_____')" />
        <xsl:variable name="cname"><xsl:apply-templates mode="gen_C_typename" select="."/></xsl:variable>
        <xsl:variable name="resulttypename"><xsl:apply-templates select="//*[@id=$returns]" mode="gen_C_typename"/></xsl:variable><xsl:text>
</xsl:text><xsl:value-of select="$name"/>_CL::<xsl:value-of select="$name"/>_CL( PyObject* pycb):_pycb(pycb){<xsl:text>
    </xsl:text><xsl:value-of select="$name"/>_cbmap[cb] = pycb;<xsl:text>
    _C_callback =  
    std::bind( &amp;</xsl:text><xsl:value-of select="$name"/>_CL::callback<xsl:text>, this, </xsl:text><xsl:value-of select="count(./Argument)"/><xsl:text>);
}
    
</xsl:text><xsl:value-of select="$cname"/><xsl:text> </xsl:text><xsl:value-of select="$name"/>_CL::<xsl:text>get_C_callback(){
    return _C_callback;
}

</xsl:text><xsl:value-of select="$name"/>
<xsl:value-of select="$name"/>_CL::callback(<xsl:apply-templates select="./Argument" mode="gen_arg"/>){<xsl:text>
    </xsl:text>PyObject pyargs[] = {<xsl:apply-templates select="./Argument" mode="to_Py_arg"/>};<xsl:text>
    </xsl:text>PyObject* result = PyObject_CallObject( _pycb, pyargs);<xsl:text>
    </xsl:text>
        if  (result){
        <xsl:apply-templates select="//*[@id=$returns]" mode="convert_python_to_C"><xsl:with-param name="paramname">result</xsl:with-param></xsl:apply-templates><xsl:text>
        }
        return </xsl:text><xsl:if test="not(//PointerType[@id=$returns])">*</xsl:if><xsl:text>result_C;
    }
    std::function&lt;</xsl:text><xsl:value-of select="$name"/><xsl:text>&gt; _C_callback;
    PyObject* _pycb;
};
</xsl:text>
    </xsl:template>
    
    <xsl:template match="Argument" mode="is_fundamental"><xsl:variable name="type" select="@type"/><xsl:apply-templates select="//*[@id=$type]" mode="is_fundamental"/>
    </xsl:template>

    <!-- =============================
        Parse all possible contructor signatures
        ============================== -->
    <xsl:template match="Constructor" mode="gen_pynative">
        <xsl:variable name="context" select="@context"/>
        <xsl:variable name="owner" select="//*[@id=$context]/@id"/>
        <xsl:variable name="namespace"><xsl:apply-templates select="//*[@id=$owner]" mode="get_namespace"/></xsl:variable>
        <xsl:variable name="format">&quot;<xsl:call-template name="get_format"><xsl:with-param name="count" select="count(./Argument)"/></xsl:call-template>&quot;</xsl:variable>
    {
        PyObject * tmp = NULL;
        (void)tmp;
         <xsl:variable name="is_fundamental"><xsl:apply-templates mode="is_fundamental" select="."/></xsl:variable>
        <xsl:variable name="prefix"><xsl:if test="$is_fundamental='true'">self-></xsl:if></xsl:variable>
        static char *kwlist[] = { <xsl:for-each select="./Argument">&quot;<xsl:choose><xsl:when test="@name"><xsl:value-of select="@name"/></xsl:when><xsl:otherwise>Anon_<xsl:value-of select="position()"/></xsl:otherwise></xsl:choose>&quot;</xsl:for-each><xsl:if test="count(./Argument)>0">,</xsl:if> NULL};<xsl:text>
        </xsl:text><xsl:for-each select="./Argument">
            <xsl:variable name="name"><xsl:choose><xsl:when test="@name"><xsl:value-of select="@name"/></xsl:when><xsl:otherwise>Anon_<xsl:value-of select="position()"/></xsl:otherwise></xsl:choose></xsl:variable>
            <xsl:variable name="is_fundamental"><xsl:apply-templates select="." mode="is_fundamental"/></xsl:variable>
        PyObject* <xsl:value-of select="$name"/> = NULL;
        </xsl:for-each><xsl:text>
            
        if ( 0 != status ){
            status = PyArg_ParseTupleAndKeywords(args, kwds, </xsl:text><xsl:value-of select="$format"/><xsl:text>, kwlist, </xsl:text><xsl:for-each select="./Argument">&amp;<xsl:value-of select="$prefix"/><xsl:choose><xsl:when test="@name"><xsl:value-of select="@name"/></xsl:when><xsl:otherwise>Anon_<xsl:value-of select="position()"/></xsl:otherwise></xsl:choose><xsl:if test="position()!=last()">,</xsl:if></xsl:for-each><xsl:text>);
            if ( 0 == status </xsl:text>
                <xsl:for-each select="./Argument">   <xsl:variable name="is_fundamental"><xsl:apply-templates select="." mode="is_fundamental"/></xsl:variable>
                <xsl:variable name="name"><xsl:choose>
                    <xsl:when test="@name"><xsl:value-of select="@name"/></xsl:when>
                    <xsl:otherwise>Anon_<xsl:value-of select="position()"/></xsl:otherwise>
                </xsl:choose></xsl:variable><xsl:if test="$is_fundamental!='true'"> &amp;&amp; <xsl:value-of select="$name"/></xsl:if></xsl:for-each><xsl:text>){
                </xsl:text><xsl:apply-templates select="./Argument[position()>1]" mode="check_type"/><xsl:text>
                </xsl:text>
                if(status == 0){
                    <xsl:apply-templates select="./Argument" mode="convert_python_to_C"/><xsl:text>
                    self-&gt;_Cobject = new </xsl:text><xsl:value-of select="$namespace"/>::<xsl:value-of select="@name"/>(<xsl:for-each select="./Argument"><xsl:variable name="name"><xsl:choose><xsl:when test="@name"><xsl:value-of select="@name"/></xsl:when><xsl:otherwise>Anon_<xsl:value-of select="position()"/></xsl:otherwise></xsl:choose>_C</xsl:variable><xsl:value-of select="$name"/><xsl:if test="position()!=last()">,</xsl:if></xsl:for-each><xsl:text>);
                    </xsl:text><xsl:apply-templates select="./Argument" mode="convert_C_to_python"/><xsl:text>
                        
                }
            }
        }
    }</xsl:text>
    </xsl:template>
    
    <xsl:template match="Argument" mode="gen_C_arglist"><xsl:variable name="name"><xsl:choose>
        <xsl:when test="@name"><xsl:value-of select="@name"/></xsl:when>
        <xsl:otherwise>Anon_<xsl:value-of select="position()"/></xsl:otherwise>
    </xsl:choose></xsl:variable><xsl:value-of select="$name"/>_C<xsl:if test="position()!=last()">, </xsl:if></xsl:template>
     
    <!-- =====================================
        Generate native Python method, accounting for overloading
        ==================================== -->    
    <xsl:template match="Method" mode="gen_pynative_decl">
        <xsl:variable name="id" select="@id"/>
        <xsl:variable name="resulttypeid" select="@returns"/>
        <xsl:variable name="resulttypename"><xsl:apply-templates select="//*[@id=$resulttypeid]" mode="gen_C_typename"/></xsl:variable>
        <xsl:variable name="context" select="@context"/>
        <xsl:variable name="name" select="@name"/>
        <xsl:variable name="realname" select="@name"/>
        <xsl:variable name="namespace"><xsl:apply-templates select="//*[@id=$context]" mode="get_namespace"/></xsl:variable>
        <xsl:variable name="namespace_prefix"><xsl:apply-templates select="//*[@id=$context]" mode="get_namespace_prefix"/></xsl:variable>
        <xsl:variable name="format">&quot;<xsl:call-template name="get_format"><xsl:with-param name="count" select="count(./Argument)"/></xsl:call-template>&quot;</xsl:variable>
        <xsl:variable name="py_classname"><xsl:apply-templates select="//*[@id=$context]" mode="gen_python_wrapper_C_typename"/></xsl:variable>
        <xsl:variable name="cname"><xsl:apply-templates select="//*[@id=$context]" mode="gen_C_typename"/></xsl:variable>
        <xsl:if test="//Method[@context=$context and @name=$name][1]/@id=$id"><xsl:variable name="static" select="@static"/>
static PyObject*<xsl:text>
</xsl:text><xsl:value-of select="$py_classname"/>__<xsl:value-of select="@name"/>(<xsl:if test="not($static='1')"><xsl:value-of select="$py_classname"/>* self, </xsl:if>PyObject *args, PyObject *kwds){<xsl:text>
    int status = -1;
    PyObject * result_py = NULL;

</xsl:text><xsl:for-each select="//Method[@context=$context and @name=$name]"><xsl:text>
    {
        </xsl:text><xsl:for-each select="./Argument">
            <xsl:variable name="name"><xsl:choose><xsl:when test="@name"><xsl:value-of select="@name"/></xsl:when><xsl:otherwise>Anon_<xsl:value-of select="position()"/></xsl:otherwise></xsl:choose></xsl:variable>
            <xsl:variable name="is_fundamental"><xsl:apply-templates select="." mode="is_fundamental"/></xsl:variable>
            PyObject* <xsl:value-of select="$name"/> = NULL;</xsl:for-each><xsl:text>
        </xsl:text>
    <xsl:variable name="is_fundamental"><xsl:apply-templates mode="is_fundamental" select="."/></xsl:variable>
    <xsl:variable name="prefix"><xsl:if test="$is_fundamental='true'">self-></xsl:if></xsl:variable><xsl:text>
        static char *kwlist[] = { </xsl:text><xsl:for-each select="./Argument">&quot;<xsl:choose><xsl:when test="@name"><xsl:value-of select="@name"/></xsl:when><xsl:otherwise>Anon<xsl:value-of select="@id"/></xsl:otherwise></xsl:choose>&quot;</xsl:for-each><xsl:if test="count(./Argument)>0">,</xsl:if> NULL};<xsl:text>
            
        if ( status !=0 ){
            status = PyArg_ParseTupleAndKeywords(args, kwds, </xsl:text><xsl:value-of select="$format"/><xsl:text>, kwlist</xsl:text><xsl:if test="count(./Argument)>0">, </xsl:if><xsl:for-each select="./Argument">&amp;<xsl:value-of select="$prefix"/><xsl:choose><xsl:when test="@name"><xsl:value-of select="@name"/></xsl:when><xsl:otherwise>Anon_<xsl:value-of select="../@id"/></xsl:otherwise></xsl:choose><xsl:if test="position()!=last()">,</xsl:if></xsl:for-each><xsl:text>);
            if ( 0 == status </xsl:text><xsl:for-each select="./Argument">   <xsl:variable name="is_fundamental"><xsl:apply-templates select="." mode="is_fundamental"/></xsl:variable>
                <xsl:variable name="name"><xsl:choose>
                    <xsl:when test="@name"><xsl:value-of select="@name"/></xsl:when>
                    <xsl:otherwise>Anon_<xsl:value-of select="position()"/></xsl:otherwise>
                </xsl:choose></xsl:variable><xsl:if test="$is_fundamental!='true'"> &amp;&amp; <xsl:value-of select="$name"/></xsl:if></xsl:for-each><xsl:text>){
                    
                    </xsl:text><xsl:apply-templates select="./Argument" mode="check_type"/><xsl:text>
                        </xsl:text> <xsl:apply-templates select="./Argument" mode="convert_python_to_C"/><xsl:text>
                        </xsl:text><xsl:if test="count(./Argument)>0">if( 0 == status ){</xsl:if><xsl:choose>
                            <xsl:when test="$resulttypeid!='' and $resulttypename!='void' and @static!='1'"><xsl:text>
                       </xsl:text><xsl:value-of select="$resulttypename"/><xsl:text> result = self-&gt;_Cobject-&gt;</xsl:text><xsl:value-of select="$realname"/>(<xsl:apply-templates select="./Argument" mode="gen_C_arglist"/>);<xsl:text>
                       </xsl:text> <xsl:apply-templates select="./Argument" mode="convert_python_to_C"/><xsl:text>
                       result_py = </xsl:text><xsl:apply-templates select="//*[@id=$resulttypeid]" mode="convert_C_to_python"><xsl:with-param name="paramname">result</xsl:with-param></xsl:apply-templates><xsl:text>;
                    </xsl:text></xsl:when>
                            <xsl:when test="$resulttypeid!='' and $resulttypename!='void' and @static='1'"><xsl:text>
                       </xsl:text><xsl:value-of select="$resulttypename"/> result = <xsl:value-of select="$cname"/>::<xsl:value-of select="$realname"/>(<xsl:apply-templates select="./Argument" mode="gen_C_arglist"/>);<xsl:text>
                       result_py = </xsl:text><xsl:apply-templates select="//*[@id=$resulttypeid]" mode="convert_C_to_python"><xsl:with-param name="paramname">result</xsl:with-param></xsl:apply-templates><xsl:text>;
                    </xsl:text></xsl:when>
                            <xsl:when test="@static='1'"><xsl:text>
                        </xsl:text><xsl:value-of select="$cname"/>::<xsl:value-of select="$realname"/>(<xsl:apply-templates select="./Argument" mode="gen_C_arglist"/>);<xsl:text>
                        </xsl:text>result_py = Py_None;<xsl:text>
                        </xsl:text></xsl:when>
                            <xsl:otherwise><xsl:text>
                        self-&gt;_Cobject-&gt;</xsl:text><xsl:value-of select="$realname"/>(<xsl:apply-templates select="./Argument" mode="gen_C_arglist"/>);<xsl:text>
                        </xsl:text>result_py = Py_None;<xsl:text>
                        </xsl:text></xsl:otherwise></xsl:choose><xsl:text>
                     </xsl:text><xsl:if test="count(./Argument)>0">} else {
                         throw "Invalid parameter type in method call";
                    }</xsl:if><xsl:text>
             }
        }
    }
    
</xsl:text></xsl:for-each>
    return result_py;
}        </xsl:if>
    </xsl:template>
    
    <xsl:template match="Method" mode="gen_pynative_defn">
        <xsl:variable name="id" select="@id"/>
        <xsl:variable name="resulttypeid" select="@returns"/>
        <xsl:variable name="context" select="@context"/>
        <xsl:variable name="name" select="@name"/>
        <xsl:variable name="realname" select="@name"/>
        <xsl:variable name="namespace"><xsl:apply-templates select="//*[@id=$context]" mode="get_namespace"/></xsl:variable>
        <xsl:variable name="namespace_prefix"><xsl:apply-templates select="//*[@id=$context]" mode="get_namespace_prefix"/></xsl:variable>
        <xsl:variable name="format">&quot;<xsl:call-template name="get_format"><xsl:with-param name="count" select="count(./Argument)"/></xsl:call-template>&quot;</xsl:variable>
        <xsl:variable name="py_classname"><xsl:apply-templates select="//*[@id=$context]" mode="gen_python_wrapper_C_typename"/></xsl:variable>
        <xsl:if test="//Method[@context=$context and @name=$name][1]/@id=$id">
static PyObject*<xsl:text>
</xsl:text><xsl:value-of select="$py_classname"/>__<xsl:value-of select="@name"/>(PyObject *args, PyObject *kwds);
    </xsl:if></xsl:template>
    
    <xsl:template match="Function" mode="gen_pynative_defn">
        <xsl:variable name="context" select="@context"/>
        <xsl:variable name="namespace"><xsl:apply-templates select="//*[@id=$context]" mode="get_namespace"/></xsl:variable>
        <xsl:variable name="namespace_prefix"><xsl:apply-templates select="//*[@id=$context]" mode="get_namespace_prefix"/></xsl:variable>
static PyObject*<xsl:text>
</xsl:text><xsl:value-of select="$namespace_prefix"/><xsl:value-of select="@name"/>(PyObject *args, PyObject *kwds);
        
    </xsl:template>
    
   <!-- ==================================
       Generate member defns
       ================================== -->
    <xsl:template match="Class|Struct" mode="gen_pymembers">
        <xsl:variable name="name"><xsl:choose><xsl:when test="@name"><xsl:value-of select="@name"/></xsl:when><xsl:otherwise>Anon_<xsl:value-of select="@id"/></xsl:otherwise></xsl:choose></xsl:variable>
        <xsl:variable name="classid" select="@id"/>
        <xsl:variable name="ctypename"><xsl:apply-templates select="." mode="gen_C_typename"/></xsl:variable>
         <xsl:variable name="namespace"><xsl:apply-templates select="." mode="get_namespace"></xsl:apply-templates></xsl:variable>
         <xsl:variable name="namespace_prefix"><xsl:apply-templates select="." mode="get_namespace_prefix"/></xsl:variable>
        <xsl:variable name="py_classname"><xsl:apply-templates select="." mode="gen_python_wrapper_C_typename"/></xsl:variable><xsl:text>
static PyMemberDef </xsl:text><xsl:value-of select="$py_classname"/><xsl:text>_members[] =   {
        </xsl:text><xsl:for-each select="//Field[@name!='' and @context=$classid and (@access='public') ]"><xsl:variable name="type" select="@type"/>
            <xsl:variable name="is_fundamental"><xsl:apply-templates select="//*[@id=$type]" mode="is_fundamental"/></xsl:variable>
            <xsl:if test="$is_fundamental">{&quot;<xsl:value-of select="@name"/>&quot;, T_OBJECT_EX, offsetof(<xsl:value-of select="$ctypename"/>,<xsl:value-of select="@name"/>), 0, ""},<xsl:text>
            </xsl:text></xsl:if>
    </xsl:for-each><xsl:text>{NULL}
};
    </xsl:text>
    </xsl:template>
    <!-- ===================
        Generate real method name
       ===================== -->
    <xsl:template match="Method" mode="get_realname"><xsl:variable name="returntypeid" select="@type"/><xsl:variable name="returntype"><xsl:apply-templates select="//*[@id=$returntypeid]" mode="gen_C_typename"/></xsl:variable><xsl:choose><xsl:when test="@name='='">assign</xsl:when>
        <xsl:when test="@name='*='">__mul__</xsl:when>
        <xsl:when test="@name='+='">__iadd__</xsl:when>
        <xsl:when test="@name='/='">__div__</xsl:when>
        <xsl:when test="@name='|='">__mod__</xsl:when>
        <xsl:when test="@name='-&gt;'">deref2</xsl:when>
        <xsl:when test="@name='=='">__cmp__</xsl:when>
        <xsl:when test="@name='!='">__notcmp__</xsl:when>
        <xsl:when test="@name='&lt;'">__lt__</xsl:when>
        <xsl:when test="@name='&gt;'">__gt__</xsl:when>
        <xsl:when test="@name='*' and $returntype='c_char_p'">__str__</xsl:when>
        <xsl:when test="@name='*'">deref</xsl:when>
        <xsl:otherwise><xsl:value-of select="@name"/></xsl:otherwise></xsl:choose>
    </xsl:template>
    
    <!-- Generate method flags -->
    <xsl:template match="Method|Function" mode="gen_methflags"><xsl:if test="@static='1'">METH_CLASS |</xsl:if> METH_VARARGS| METH_KEYWORDS</xsl:template>
    
    <!-- ===============================
        Generate native python definition for a class or struct
        ================================ -->   
    
    <xsl:template match="Class|Struct" mode="gen_method_list">
        <xsl:param name="basetypeid"/>
        <xsl:param name="py_classname"><xsl:apply-templates mode="gen_python_wrapper_C_typename" select="."/></xsl:param>
        <xsl:param name="parentclassid"/>
        <xsl:param name="name"><xsl:choose><xsl:when test="@name"><xsl:value-of select="@name"/></xsl:when><xsl:otherwise>Anon_<xsl:value-of select="@id"/></xsl:otherwise>
        </xsl:choose></xsl:param>
        <xsl:param name="namespace"><xsl:apply-templates select="." mode="get_namespace"></xsl:apply-templates></xsl:param>
        <xsl:param name="namespace_prefix"><xsl:apply-templates select="." mode="get_namespace_prefix"/></xsl:param>
        <xsl:variable name="classid" select="@id"/>
        <xsl:for-each select="//Method[@context=$classid and (@access='public')]">
            <xsl:variable name="methname" select="@name"/>
            <xsl:if test="count(//Method[@context=$parentclassid and @name=$methname])=0" >
            {&quot;<xsl:apply-templates select="." mode="get_realname"/>&quot;,  (PyCFunction)<xsl:value-of select="$py_classname"/>__<xsl:apply-templates select="." mode="get_realname"/>,<xsl:apply-templates select="." mode="gen_methflags"/>, &quot;&quot;},</xsl:if></xsl:for-each> 
        <xsl:for-each select="./Base">
            <xsl:variable select="@type" name="basetypeid"/>
           <xsl:apply-templates select="." mode="gen_method_list">
               <xsl:with-param name="basetypeid" select="$basetypeid"/>
               <xsl:with-param name="py_classname" select="$py_classname"/>
               <xsl:with-param name="parentclassid"><xsl:choose>
                   <xsl:when test="$parentclassid!=''"><xsl:value-of select="$parentclassid"/></xsl:when>
                   <xsl:otherwise><xsl:value-of select="$classid"/></xsl:otherwise>
               </xsl:choose></xsl:with-param>
               <xsl:with-param name="name" select="$name"/>
               <xsl:with-param name="namespace" select="$namespace"/>
               <xsl:with-param name="namepsace_prefix" select="$namespace_prefix"/>
           </xsl:apply-templates>
        </xsl:for-each>
    </xsl:template>
    
    <xsl:template match="Class|Struct" mode="gen_py_defn">
        <xsl:param name="is_base">1</xsl:param>
        <xsl:param name="py_classname"><xsl:apply-templates mode="gen_python_wrapper_C_typename" select="."/></xsl:param>
        <xsl:param name="classid" select="@id"/>
        <xsl:param name="name"><xsl:choose><xsl:when test="@name"><xsl:value-of select="@name"/></xsl:when><xsl:otherwise>Anon_<xsl:value-of select="@id"/></xsl:otherwise>
        </xsl:choose></xsl:param>
        <xsl:param name="namespace"><xsl:apply-templates select="." mode="get_namespace"></xsl:apply-templates></xsl:param>
        <xsl:param name="namespace_prefix"><xsl:apply-templates select="." mode="get_namespace_prefix"/></xsl:param>
        <xsl:if test="$is_base='1'"><xsl:text>
static void
</xsl:text><xsl:value-of select="$py_classname"/><xsl:text>_dealloc(</xsl:text><xsl:value-of select="$py_classname"/><xsl:text>* self);

static PyObject *
</xsl:text><xsl:value-of select="$py_classname"/><xsl:text>_new(PyTypeObject *type, PyObject *args, PyObject *kwds);
static int
</xsl:text><xsl:value-of select="$py_classname"/><xsl:text>_init(</xsl:text><xsl:value-of select="$py_classname"/><xsl:text> *self, PyObject *args, PyObject *kwds);
           
</xsl:text></xsl:if><xsl:apply-templates select="//Method[@context=$classid and (@access='public')]" mode="gen_pynative_defn"/>
        <xsl:text>
static PyMethodDef </xsl:text><xsl:value-of select="$py_classname"/><xsl:text>_methods[] = {
             </xsl:text><xsl:apply-templates select="." mode="gen_method_list"/>
             <xsl:text>{NULL}  /* Sentinel */
};
</xsl:text><xsl:apply-templates select="." mode="gen_pymembers"/><xsl:text>

static PyTypeObject </xsl:text><xsl:value-of select="$py_classname"/><xsl:text>Type = {
             PyObject_HEAD_INIT(NULL)
             0,                         /*ob_size*/
             "</xsl:text><xsl:value-of select="$namespace"/>.<xsl:value-of select="@name"/><xsl:text>",             /*tp_name*/
             sizeof(</xsl:text><xsl:value-of select="$py_classname"/><xsl:text>), /*tp_basicsize*/
             0,                         /*tp_itemsize*/
             </xsl:text>(destructor)<xsl:value-of select="$py_classname"/>_dealloc<xsl:text>,                         /*tp_dealloc*/
             0,                         /*tp_print*/
             0,                         /*tp_getattr*/
             0,                         /*tp_setattr*/
             0,                         /*tp_compare*/
             0,                         /*tp_repr*/
             0,                         /*tp_as_number*/
             0,                         /*tp_as_sequence*/
             0,                         /*tp_as_mapping*/
             0,                         /*tp_hash */
             0,                         /*tp_call*/
             0,                         /*tp_str*/
             0,                         /*tp_getattro*/
             0,                         /*tp_setattro*/
             0,                         /*tp_as_buffer*/
             Py_TPFLAGS_DEFAULT| Py_TPFLAGS_BASETYPE,        /*tp_flags*/
             "</xsl:text><xsl:value-of select="$py_classname"/><xsl:text> objects",           /* tp_doc */
             0,		               /* tp_traverse */
             0,		               /* tp_clear */
             0,		               /* tp_richcompare */
             0,		               /* tp_weaklistoffset */
             0,		               /* tp_iter */
             0,		               /* tp_iternext */
             </xsl:text><xsl:value-of select="$py_classname"/><xsl:text>_methods,             /* tp_methods */
             </xsl:text><xsl:value-of select="$py_classname"/><xsl:text>_members,             /* tp_members */
             0,                         /* tp_getset */
             0,                         /* tp_base */
             0,                         /* tp_dict */
             0,                         /* tp_descr_get */
             0,                         /* tp_descr_set */
             0,                         /* tp_dictoffset */
             (initproc)</xsl:text><xsl:value-of select="$py_classname"/><xsl:text>_init,      /* tp_init */
             0,                         /* tp_alloc */
             </xsl:text><xsl:value-of select="$py_classname"/><xsl:text>_new,                 /* tp_new */
         };
</xsl:text>
   </xsl:template>
    
     <xsl:template match="Struct|Class" mode="gen_pynative">
         <xsl:variable name="classid" select="@id"/>
         <xsl:variable name="name"><xsl:choose><xsl:when test="@name"><xsl:value-of select="@name"/></xsl:when><xsl:otherwise>Anon_<xsl:value-of select="@id"/></xsl:otherwise>
         </xsl:choose></xsl:variable>
         <xsl:variable name="namespace"><xsl:apply-templates select="." mode="get_namespace"></xsl:apply-templates></xsl:variable>
         <xsl:variable name="namespace_prefix"><xsl:apply-templates select="." mode="get_namespace_prefix"/></xsl:variable>
         <xsl:variable name="py_classname"><xsl:apply-templates mode="gen_python_wrapper_C_typename" select="."/></xsl:variable>
<xsl:text>
static void
</xsl:text><xsl:value-of select="$py_classname"/><xsl:text>_dealloc(</xsl:text><xsl:value-of select="$py_classname"/><xsl:text>* self)
{
    if(!self) return;
    if (!self-&gt;_is_c_reference){
        delete self->_Cobject;
    }
    self->_Cobject = NULL;
    self->_owner = NULL;
    self->_is_c_reference = false;
    self->ob_type->tp_free((PyObject*)self);
}

static PyObject *
</xsl:text><xsl:value-of select="$py_classname"/><xsl:text>_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    </xsl:text><xsl:value-of select="$py_classname"/><xsl:text> *self;

    self = (</xsl:text><xsl:value-of select="$py_classname"/><xsl:text> *)type->tp_alloc(type, 0);
    if (self != NULL) {
        self->_Cobject = NULL;
        self->_owner = NULL;
    }

    return (PyObject *)self;
}       

static int
</xsl:text><xsl:value-of select="$py_classname"/><xsl:text>_init(</xsl:text><xsl:value-of select="$py_classname"/><xsl:text> *self, PyObject *args, PyObject *kwds)
{
    int status = -1;
    if(!self) return status;
    self->_owner = (PyObject*)self;
    </xsl:text><xsl:if test="count(//Constructor[@context=$classid])=0">self-&gt;_Cobject = new <xsl:value-of select="$py_classname"/>();
    status = self-&gt;get_Cobject(self)?0:-1; </xsl:if><xsl:apply-templates select="//Constructor[@context=$classid and (@access='public')]" mode="gen_pynative"/><xsl:text>
    if (status != 0){
       self-&gt;_Cobject = NULL;
       status = 0;
    }
    return status;
}

</xsl:text><xsl:apply-templates select="//Method[@context=$classid and (@access='public' )]" mode="gen_pynative_decl"/><xsl:text>
</xsl:text>
     </xsl:template>     

     <!-- =====================
         ROOT processing node
         ====================== -->
     <xsl:template match="/GCC_XML"><xsl:text>
#include &lt;functional&gt;
#include </xsl:text>&lt;<xsl:text>Python.h</xsl:text>&gt;<xsl:text>
#include </xsl:text>&lt;<xsl:text>structmember.h</xsl:text>&gt;<xsl:text>
</xsl:text><xsl:for-each select="//File[not(starts-with(@name,'/usr/share/castxml')) and @name!='&lt;builtin&gt;']"><xsl:text>
#include &lt;</xsl:text><xsl:value-of select="@name"/><xsl:text>&gt;
</xsl:text>
</xsl:for-each><xsl:text>

#define PyFloatObject PyObject

static bool
checkType( PyTypeObject* const  type, PyObject * const obj){
    return PyObject_IsInstance( obj, (PyObject*)type );
}
    
</xsl:text><xsl:apply-templates select="//FunctionType" mode="gen_python_C_decl"/>
         
         <xsl:for-each select="//Namespace[@name!='::']">
             <xsl:variable name="scopeid" select="@id"/>
             <xsl:variable name="namespace"><xsl:apply-templates select="." mode="get_namespace"/></xsl:variable>
             <xsl:variable name="pynamespace"><xsl:call-template name="string-replace-all"><xsl:with-param name="text" select="$namespace"/><xsl:with-param name="replace">::</xsl:with-param><xsl:with-param name="by">.</xsl:with-param></xsl:call-template></xsl:variable>
             <xsl:variable name="namespace_prefix"><xsl:apply-templates select="." mode="get_namespace_prefix"/></xsl:variable>
<xsl:for-each select="//Struct[@context=$scopeid]|//Class[@context=$scopeid]">
    <xsl:variable name="namespace"><xsl:apply-templates select="." mode="get_namespace"/></xsl:variable>
    <xsl:variable name="namespace_prefix"><xsl:apply-templates select="." mode="get_namespace_prefix"/></xsl:variable>
    <xsl:variable name="classname"><xsl:choose><xsl:when test="@name"><xsl:value-of select="translate(@name,'&lt;&gt;, :','_____')"/></xsl:when><xsl:otherwise>Anon_<xsl:value-of select="@id"/></xsl:otherwise></xsl:choose></xsl:variable>
    <xsl:text>
struct </xsl:text><xsl:value-of select="$namespace_prefix"/><xsl:value-of select="$classname"/><xsl:text>{
    </xsl:text>PyObject_HEAD<xsl:text>
    </xsl:text><xsl:value-of select="$namespace"/>::<xsl:value-of select="@name"/><xsl:text>* _Cobject;
    PyObject* _owner;
    bool _is_c_reference;
    
    </xsl:text><xsl:value-of select="$namespace"/>::<xsl:value-of select="@name"/>*<xsl:text>
    get_Cobject(struct </xsl:text><xsl:value-of select="$classname"/><xsl:text>* newowner);
    </xsl:text><xsl:text>
};

</xsl:text>
</xsl:for-each>
         </xsl:for-each>
         <xsl:for-each select="//Namespace[@name!='::']">
             <xsl:variable name="scopeid" select="@id"/>
             <xsl:variable name="namespace"><xsl:apply-templates select="." mode="get_namespace"/></xsl:variable>
             <xsl:variable name="pynamespace"><xsl:call-template name="string-replace-all"><xsl:with-param name="text" select="$namespace"/><xsl:with-param name="replace">::</xsl:with-param><xsl:with-param name="by">.</xsl:with-param></xsl:call-template></xsl:variable>
             <xsl:variable name="namespace_prefix"><xsl:apply-templates select="." mode="get_namespace_prefix"/></xsl:variable>
<xsl:apply-templates select="//Function[@context=$scopeid]" mode="gen_pynative_defn"/>
<xsl:apply-templates select="//Struct[@context=$scopeid]|//Class[@context=$scopeid]" mode="gen_py_defn"></xsl:apply-templates><xsl:text>

#ifndef PyMODINIT_FUNC	/* declarations for DLL import/export */
#define PyMODINIT_FUNC void
#endif


static PyMethodDef </xsl:text><xsl:value-of select="$namespace_prefix"/><xsl:text>methods[] = {
             </xsl:text><xsl:for-each select="//Function[@context=$scopeid]">
                 {&quot;<xsl:value-of select="@name"/>&quot;,  (PyCFunction)<xsl:value-of select="$namespace_prefix"/><xsl:value-of select="@name"/>,<xsl:apply-templates select="." mode="gen_methflags"/>, &quot;&quot;},
             </xsl:for-each><xsl:text>{NULL}  /* Sentinel */
};

</xsl:text><xsl:apply-templates select="//Struct[@context=$scopeid]|//Class[@context=$scopeid]" mode="gen_pynative"></xsl:apply-templates><xsl:text>
           
PyMODINIT_FUNC
init</xsl:text><xsl:value-of select="$namespace_prefix"/><xsl:text>(void) 
{
    int status = 0;
    PyObject* m;
    m = Py_InitModule3("</xsl:text><xsl:value-of select="$pynamespace"/><xsl:text>", </xsl:text><xsl:value-of select="$namespace"/><xsl:text>_methods,
             "Module wrapping C/C++ library </xsl:text><xsl:value-of select="$namespace"/><xsl:text>");
     
    </xsl:text><xsl:for-each select="//Struct[@context=$scopeid]|//Class[@context=$scopeid]"><xsl:variable name="name"><xsl:apply-templates select="." mode="gen_C_typename"/></xsl:variable>
    <xsl:variable name="py_classname"><xsl:apply-templates select="." mode="gen_python_wrapper_C_typename"/></xsl:variable><xsl:text>
    </xsl:text><xsl:value-of select="$py_classname"/><xsl:text>Type.tp_new = PyType_GenericNew;
    if (PyType_Ready(&amp;</xsl:text><xsl:value-of select="$py_classname"/><xsl:text>) == 0){             
        Py_INCREF(&amp;</xsl:text><xsl:value-of select="$py_classname"/><xsl:text>Type);
        PyModule_AddObject(m, "</xsl:text><xsl:value-of select="translate(@name,'&lt;&gt;:','___')"/><xsl:text>", (PyObject *)&amp;</xsl:text><xsl:value-of select="$py_classname"/><xsl:text>Type);
    } else {
       status = =1;
    }
</xsl:text></xsl:for-each><xsl:text>
    //how to handle status???
}
</xsl:text><xsl:apply-templates select="//Function[@context=$scopeid]" mode="gen_pynative_decl"/>
             
             <xsl:for-each select="//Struct[@context=$scopeid]|Class[@context=$scopeid]">
                 <xsl:variable name="namespace"><xsl:apply-templates select="." mode="get_namespace"/></xsl:variable>
                 <xsl:variable name="namespace_prefix"><xsl:apply-templates select="." mode="get_namespace_prefix"/></xsl:variable>
                 <xsl:variable name="py_classname"><xsl:apply-templates select="." mode="gen_C_typename"></xsl:apply-templates></xsl:variable><xsl:text>
</xsl:text><xsl:value-of select="$namespace"/>::<xsl:value-of select="@name"/>* <xsl:value-of select="$py_classname"/>::<xsl:text>
get_Cobject(struct </xsl:text><xsl:value-of select="$namespace_prefix"/><xsl:value-of select="$py_classname"/><xsl:text>* newowner){
   if( _is_c_reference &amp;&amp; _owner != (PyObject*)newowner ){
       _Cobject = new </xsl:text><xsl:value-of select="$namespace"/>::<xsl:value-of select="@name"/><xsl:text>(*_Cobject);
       _is_c_reference = false;
   }
   return _Cobject;
}
                 </xsl:text>
             </xsl:for-each>
         </xsl:for-each>
     </xsl:template>
</xsl:stylesheet>