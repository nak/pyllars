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
        <xsl:param name="type"/>
        <xsl:choose>
            <xsl:when test="$type='__int128'">(c_longlong*2)</xsl:when>
            <xsl:when test="$type='__uint28'">(c_ulonglong*2)</xsl:when>
            <xsl:when test="$type='long_int'">c_long</xsl:when>
            <xsl:when test="$type='long_unsigned_int' or $type='long_unsigned'">c_ulong</xsl:when>
            <xsl:when test="$type='signed_char'">c_char</xsl:when>
            <xsl:when test="$type='char'">c_char</xsl:when>
            <xsl:when test="$type='short_int'">c_short</xsl:when>
            <xsl:when test="$type='unsigned_char'">c_byte</xsl:when>
            <xsl:when test="$type='short_unsigned_int' or $type='unsigned_short'">c_ushort</xsl:when>
            <xsl:when test="$type='int'">c_int</xsl:when>            
            <xsl:when test="$type='unsigned_int' or $type='unsigned'">c_uint</xsl:when>
            <xsl:when test="$type='bool'">c_char</xsl:when>
            <xsl:when test="$type='float'">c_float</xsl:when>
            <xsl:when test="$type='double'">c_double</xsl:when>
            <xsl:when test="$type='void'">void</xsl:when>
            <xsl:when test="$type='unsigned_int128'">(c_ulong*2)</xsl:when>
            <xsl:when test="$type='unsigned__int128'">(c_ulong*2)</xsl:when>
            <xsl:when test="$type='unsigned___int128'">(c_ulong*2)</xsl:when>
            <xsl:otherwise>Unknown_type_<xsl:value-of select="$type"/></xsl:otherwise>
        </xsl:choose>
    </xsl:template>

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
    <xsl:template match="CvQualifiedType|Typedef" mode="gen_typename"><xsl:variable name="type" select="@type"/><xsl:apply-templates select="//*[@id=$type]" mode="gen_typename"/></xsl:template>

    <xsl:template match="ReferenceType" mode="gen_typename"><xsl:variable name="type" select="@type"/><xsl:apply-templates select="//*[@id=$type]" mode="gen_typename"/>&amp;</xsl:template>
     
    <xsl:template match="PointerType" mode="gen_typename"><xsl:variable name="typeid" select="@type"/><xsl:apply-templates mode="gen_typename" select="//*[@id=$typeid]"></xsl:apply-templates> *</xsl:template>
    
    <xsl:template match="FundamentalType" mode="gen_typename"><xsl:call-template name="get_basic_type"><xsl:with-param name="type" select="@name"/></xsl:call-template></xsl:template>

    <xsl:template match="FunctionType" mode="gen_typename"><xsl:variable name="id" select="@id"/><xsl:variable name="name" select="translate(//Typedef[@type=$id]/@name,'$lt;$gt;,: ','_____')"/><xsl:choose>
        <xsl:when test="$name"><xsl:value-of select="$name"/></xsl:when><xsl:otherwise>functype_<xsl:value-of select="$id"/></xsl:otherwise></xsl:choose></xsl:template>
    
    <xsl:template match="Typedef" mode="gen_realtypename"><xsl:value-of select="@type"/></xsl:template>
    
    <xsl:template match="PointerType|ReferenceType" mode="gen_realtypename"><xsl:variable name="type" select="@type"/><xsl:apply-templates select="//*[@id=$type]" mode="gen_realtypename"/>*</xsl:template>
   
    <xsl:template match="CvQualifiedType" mode="gen_realtypename"><xsl:variable name="type" select="@type"/><xsl:apply-templates select="//*[@id=$type]" mode="gen_realtypename"/></xsl:template>
    
    <xsl:template match="FunctionType" mode="gen_realtypename"><xsl:apply-templates select="." mode="get_typename"/></xsl:template>
    
    <xsl:template match="FundamentalType" mode="gen_realtypename"><xsl:call-template name="get_basic_type"><xsl:with-param name="type" select="@name"/></xsl:call-template></xsl:template>
    
    
    <xsl:template match="Class|Struct" mode="gen_realtypename">        <xsl:variable name="classid" select="@id"/>
        <xsl:variable name="namespace"><xsl:apply-templates select="." mode="get_namespace"></xsl:apply-templates></xsl:variable>
        <xsl:variable name="namespace_prefix"><xsl:apply-templates select="." mode="get_namespace_prefix"/></xsl:variable><xsl:value-of select="$namespace_prefix"/><xsl:choose><xsl:when test="@name"><xsl:value-of select="translate(@name,'&lt;&gt;:, ','_____')"/></xsl:when><xsl:otherwise>Anon_<xsl:value-of select="@id"/></xsl:otherwise></xsl:choose></xsl:template>
    
    <xsl:template match="Class|Struct" mode="gen_typename"><xsl:choose><xsl:when test="@name"><xsl:value-of select="translate(@name,'&lt;&gt;:, ','_____')"/></xsl:when><xsl:otherwise>Anon_<xsl:value-of select="@id"/></xsl:otherwise></xsl:choose></xsl:template>
    
    <!-- Tell if type is a funcdamental type or composite type -->
    
    <xsl:template match="Typedef" mode="is_fundamental"><xsl:variable name="type" select="@type"/><xsl:apply-templates select="//*[@id=$type]" mode="is_fundamental"/></xsl:template>
    
    <xsl:template match="FundamentalType" mode="is_fundamental">true</xsl:template>
    
    <!-- =====================================================
                     GENERATE RETURN OBJECT GENERATION
         ===================================================== -->
    <!-- Generate function callback Python logic -->
    <xsl:template match="FunctionType" mode="gen_python"><xsl:variable name="returns" select="@returns"/><xsl:variable name="name"><xsl:apply-templates mode="gen_typename" select="."/></xsl:variable><xsl:text>
</xsl:text>class <xsl:value-of select="$name"/>_CL{<xsl:text>
public:

    </xsl:text><xsl:value-of select="$name"/>( PyObject* pycb):_pycb(pycb){<xsl:text>
    </xsl:text>    <xsl:value-of select="$name"/>_cbmap[cb] = pycb;<xsl:text>
       _C_callback =  
         std::bind( &amp;</xsl:text><xsl:value-of select="$name"/>_CL::callback<xsl:text>, this, </xsl:text><xsl:value-of select="count(./Argument)"/><xsl:text>
    }
    
    </xsl:text><xsl:value-of select="$name"/><xsl:text> get_C_callback{
             return _C_callback;
    }
    
private:
    </xsl:text><xsl:apply-templates select="//*[@id=$returns]" mode="gen_typename"/> callback(<xsl:apply-templates select="./Argument" mode="gen_arg"/>){<xsl:text>
        </xsl:text>PyObject pyargs[] = <xsl:apply-templates select="./Argument" mode="gen_argpyconversins"/><xsl:text>
        </xsl:text>PyObject* result = PyObject_CallObject( _pycb, pyargs);<xsl:text>
        return </xsl:text><xsl:call-template name="convert_return_to_C"><xsl:with-param name="retrunid" select="$returns"/>
        <xsl:with-param name="resultname">result</xsl:with-param></xsl:call-template><xsl:text>
    }
    std::function&lt;</xsl:text><xsl:value-of select="$name"/><xsl:text>&gt; _C_callback;
    PyObject* _pycb;
};
</xsl:text>
    </xsl:template>
    <!-- Generate return type logic -->
    <xsl:template match="CvQualifiedType|Typedef" mode="gen_returnlogic"><xsl:variable name="typeid" select="@type"/><xsl:apply-templates select="//*[@id=$typeid]" mode="gen_returnlogic"/></xsl:template>
 
    <xsl:template match="FundamentalType" mode="gen_returnlogic"><xsl:param name="paramname"/>
        <xsl:variable name="typeid"  select="@type"/>
        <xsl:variable name="type"><xsl:call-template name="get_basic_type"><xsl:with-param name="type" select="$typeid"/></xsl:call-template></xsl:variable>
        result_obj =  <xsl:choose>
            <xsl:when test="$type='__int128'">PyLong_FromLong(result);</xsl:when>
            <xsl:when test="$type='__uint28'">PyLong_FromLong(result);</xsl:when>
            <xsl:when test="$type='long_int'">PyInt_FromLong(result);</xsl:when>
            <xsl:when test="$type='long_unsigned_int'">PyLong_FromLong(result);</xsl:when>
            <xsl:when test="$type='signed_char'">PyInt_FromLong(result);</xsl:when>
            <xsl:when test="$type='char'">PyInt_FromLong(result);</xsl:when>
            <xsl:when test="$type='short_int'">PyInt_FromLong(result);</xsl:when>
            <xsl:when test="$type='unsigned_char'">PyInt_FromLong(result);</xsl:when>
            <xsl:when test="$type='short_unsigned_int'">PyInt_FromLong(result);</xsl:when>
            <xsl:when test="$type='int'">PyInt_FromLong(result);</xsl:when>            
            <xsl:when test="$type='unsigned_int'">PyInt_FromLong(result);</xsl:when>
            <xsl:when test="$type='bool'">result?Py_True:Py_False;</xsl:when>
            <xsl:when test="$type='float'">PyFloat_FromDouble(result);</xsl:when>
            <xsl:when test="$type='double'">PyFloat_FromDouble(result);</xsl:when>
            <xsl:when test="$type='void'">Py_None</xsl:when>
            <xsl:when test="$type='unsigned_int128'">PyLong_FromLong(result);</xsl:when>
            <xsl:when test="$type='unsigned__int128'">PyLong_FromLong(result);</xsl:when>
            <xsl:when test="$type='unsigned___int128'">PyLong_FromLong(result);</xsl:when>
            <xsl:otherwise>Unknown_type_<xsl:value-of select="$type"/></xsl:otherwise>
        </xsl:choose>
    </xsl:template>
 
    <xsl:template match="ReferenceType" mode="gen_returnlogic"><xsl:param name="paramname"/><xsl:variable name="typeid" select="@type"/>
        <xsl:variable name="is_funcdamental"><xsl:apply-templates select="//*[@id=$typeid]" mode="is_fundamental"/></xsl:variable>
        <xsl:choose>
            <xsl:when test="$is_funcdamental">TO BE FIXED result_obj = Py<xsl:apply-templates select="//*[@id=$typeid]" mode="gen_typename"/>Object_new(result);</xsl:when>
            <xsl:otherwise><xsl:variable name="py_classname"><xsl:apply-templates select="//*[@d=$typeid]" mode="get_namespace_prefix"/><xsl:apply-templates select="//*[@id=$typeid]" mode="gen_typename"/></xsl:variable>
                <xsl:variable name="class_name"><xsl:apply-templates select="//*[@d=$typeid]" mode="get_namespace"/><xsl:apply-templates select="//*[@id=$typeid]" mode="gen_typename"/></xsl:variable>
                result_obj = PyObject_CallObject((PyObject *) &amp; <xsl:value-of select="$py_classname"/>Type, NULL);
                if(result_obj){
                    delete ((<xsl:value-of select="$py_classname"/>*) result_obj)-&gt;_object) ;
                ((<xsl:value-of select="$py_classname"/>*) result_obj)-&gt;_object = &amp;result;
                ((<xsl:value-of select="$py_classname"/>*) result_obj)-&gt;_is_c_reference = true;
                }
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>
    
    <xsl:template match="PointerType" mode="gen_returnlogic"><xsl:param name="paramname"/>
        <xsl:variable name="typeid" select="@id"/>
        <xsl:variable name="py_classname"><xsl:apply-templates select="//*[@id=$typeid]" mode="get_namespace_prefix"/><xsl:apply-templates select="//*[@id=$typeid]" mode="gen_typename"/></xsl:variable>
                result_obj = PyObject_CallObject((PyObject *) &amp; <xsl:value-of select="$py_classname"/>Type, NULL);
                if(result_obj){
                delete ((<xsl:value-of select="$py_classname"/>*) result_obj)-&gt;_object ;
        ((<xsl:value-of select="$py_classname"/>*) result_obj)-&gt;_object = result;
                }
    </xsl:template>
    
    <xsl:template match="Class|Struct" mode="gen_returnlogic">  <xsl:param name="paramname"/>      
         <xsl:variable name="py_classname"><xsl:apply-templates select="." mode="get_namespace_prefix"/><xsl:apply-templates select="." mode="gen_typename"/></xsl:variable>
                result_obj = PyObject_CallObject((PyObject *) &amp; <xsl:value-of select="$py_classname"/>Type, NULL);
                if(result_obj){
                    delete ((<xsl:value-of select="$py_classname"/>*) result_obj)-&gt;_object ;
                    ((<xsl:value-of select="$py_classname"/>*) result_obj)-&gt;_object = new <xsl:apply-templates select="." mode="get_namespace"/>::<xsl:apply-templates select="." mode="gen_typename"/>(result);
                }
    </xsl:template>
    
    <!-- ===============================
         Generate an argument definition as part of an init call
         for a constructor argument
         =============================== -->
    <xsl:template match="Argument" mode="gen_pynative_decl2"><xsl:variable name="id" select="@id"/>
        <xsl:variable name="typeid" select="@type"/>
        <xsl:variable name="argname"><xsl:choose><xsl:when test="@name"><xsl:value-of select="@name"/></xsl:when><xsl:otherwise>Anon_<xsl:value-of select="position()"/></xsl:otherwise></xsl:choose></xsl:variable>
        <xsl:variable name="is_fundamental" ><xsl:apply-templates select="." mode="is_fundamental"/></xsl:variable>
        <xsl:choose><xsl:when test="$is_fundamental='true'"><xsl:apply-templates select="." mode="gen_typename"/> <xsl:value-of select="$argname"/></xsl:when>
        <xsl:otherwise><xsl:variable name="py_classname"><xsl:apply-templates select="//*[@id=$typeid]" mode="get_namespace_prefix"/><xsl:apply-templates select="//*[@id=$typeid]" mode="gen_typename"/></xsl:variable>
        <xsl:value-of select="translate($py_classname,'&amp;',' ')"/><xsl:text>* </xsl:text><xsl:value-of select="$argname"/>= NULL;</xsl:otherwise></xsl:choose></xsl:template>
 
    <xsl:template match="Argument" mode="is_fundamental"><xsl:variable name="type" select="@type"/><xsl:apply-templates select="//*[@id=$type]" mode="is_fundamental"/>
    </xsl:template>

    <!-- =============================
        Parse all possible contructor signatures
        ============================== -->
    <xsl:template match="Constructor" mode="gen_pynative">
        <xsl:variable name="context" select="@context"/>
        <xsl:variable name="owner" select="//*[@id=$context]/@id"/>
        <xsl:variable name="namespace"><xsl:apply-templates select="//*[@id=$owner]" mode="get_namespace"/></xsl:variable>
        <xsl:variable name="format">TO_BE_IMPLEMENTED</xsl:variable>
    {
        PyObject * tmp = NULL;
        (void)tmp;
        <xsl:apply-templates select="./Argument" mode="gen_pynative_decl2"/>
        <xsl:variable name="is_fundamental"><xsl:apply-templates mode="is_fundamental" select="."/></xsl:variable>
        <xsl:variable name="prefix"><xsl:if test="$is_fundamental='true'">self-></xsl:if></xsl:variable>
        static const char *kwlist[] = { <xsl:for-each select="./Argument">&quot;<xsl:choose><xsl:when test="@name"><xsl:value-of select="@name"/>/></xsl:when><xsl:otherwise>Anon_<xsl:value-of select="position()"/></xsl:otherwise></xsl:choose>&quot;</xsl:for-each><xsl:if test="count(./Argument)>0">,</xsl:if> NULL};<xsl:text>
            
        if ( status !=0 ){
            status = PyArg_ParseTupleAndKeywords(args, kwds, </xsl:text><xsl:value-of select="$format"/><xsl:text>, kwlist, </xsl:text><xsl:for-each select="./Argument">&amp;<xsl:value-of select="$prefix"/><xsl:choose><xsl:when test="@name"><xsl:value-of select="@name"/></xsl:when><xsl:otherwise>Anon_<xsl:value-of select="position()"/></xsl:otherwise></xsl:choose><xsl:if test="position()!=last()">,</xsl:if></xsl:for-each><xsl:text>);
            if ( 0 == status </xsl:text><xsl:for-each select="./Argument">   <xsl:variable name="is_fundamental"><xsl:apply-templates select="." mode="is_fundamental"/></xsl:variable>
                <xsl:variable name="argtype" select="@type"/>
                <xsl:variable name="deref"><xsl:if test="//ReferenceType[@id=$argtype]">*</xsl:if></xsl:variable>
                <xsl:variable name="name"><xsl:choose>
                    <xsl:when test="@name"><xsl:value-of select="@name"/></xsl:when>
                    <xsl:otherwise>Anon_<xsl:value-of select="position()"/></xsl:otherwise>
                </xsl:choose></xsl:variable><xsl:if test="$is_fundamental!='true'"> &amp;&amp; <xsl:value-of select="$name"/></xsl:if></xsl:for-each><xsl:text>){
                </xsl:text><xsl:for-each select="./Argument">
                    <xsl:variable name="is_fundamental"><xsl:apply-templates select="." mode="is_fundamental"/></xsl:variable>
                    <xsl:variable name="argtype" select="@type"/>
                    <xsl:variable name="deref"><xsl:if test="//ReferenceType[@id=$argtype]">*</xsl:if></xsl:variable>
                    <xsl:variable name="name"><xsl:choose>
                        <xsl:when test="@name"><xsl:value-of select="@name"/></xsl:when>
                        <xsl:otherwise>Anon_<xsl:value-of select="position()"/></xsl:otherwise>
                    </xsl:choose></xsl:variable>
                    <xsl:if test="$is_fundamental!='true'">
                         <xsl:apply-templates select="//*[@id=$argtype]" mode="get_namespace"/>::<xsl:apply-templates select="//*[@id=$argtype]" mode="gen_typename"/><xsl:text> </xsl:text><xsl:value-of select="$name"/>_obj = <xsl:value-of select="$deref"/><xsl:value-of select="$name"/>-&gt;_object;
                    </xsl:if>
                </xsl:for-each><xsl:text>
                self-&gt;_object = new </xsl:text><xsl:value-of select="$namespace"/>::<xsl:value-of select="@name"/>(<xsl:for-each select="./Argument"><xsl:variable name="name"><xsl:choose><xsl:when test="@name"><xsl:value-of select="@name"/></xsl:when><xsl:otherwise>Anon_<xsl:value-of select="position()"/></xsl:otherwise>
                </xsl:choose></xsl:variable>
                <xsl:variable name="is_fundamental"><xsl:apply-templates mode="is_fundamental" select="."/></xsl:variable><xsl:choose>
                    <xsl:when test="$is_fundamental='true'"><xsl:value-of select="$name"/></xsl:when>
                    <xsl:otherwise><xsl:value-of select="$name"/>_obj</xsl:otherwise>
                        </xsl:choose><xsl:if test="position()!=last()">,</xsl:if></xsl:for-each><xsl:text>);
               
            }
        }
    }</xsl:text>
    </xsl:template>
     
    <!-- =====================================
        Generate native Python method, accounting for overloading
        ==================================== -->
    <xsl:template match="Method[@static!='1']" mode="gen_pynative_decl">
        <xsl:variable name="id" select="@id"/>
        <xsl:variable name="resulttypeid" select="@returns"/>
        <xsl:variable name="context" select="@context"/>
        <xsl:variable name="name" select="@name"/>
        <xsl:variable name="realname" select="@name"/>
        <xsl:variable name="namespace"><xsl:apply-templates select="//*[@id=$context]" mode="get_namespace"/></xsl:variable>
        <xsl:variable name="namespace_prefix"><xsl:apply-templates select="//*[@id=$context]" mode="get_namespace_prefix"/></xsl:variable>
        <xsl:variable name="format">TO_BE_IMPLEMENTED</xsl:variable>
        <xsl:variable name="py_classname"><xsl:value-of select="$namespace_prefix"/><xsl:apply-templates select="//*[@id=$context]" mode="gen_typename"/></xsl:variable>
static PyObject*<xsl:text>
</xsl:text><xsl:value-of select="$py_classname"/>__<xsl:value-of select="@name"/>(PyObject *args, PyObject *kwds){<xsl:text>
   int status = -1;
</xsl:text><xsl:if test="//Method[@context=$context and @name=$name][1]/@id=$id"><xsl:for-each select="//Method[@context=$context and @name=$name]"><xsl:text>
    {
        PyObject * result_obj = NULL;
        if (!self || !self-&gt;_object){
            return result_obj;
        }
        </xsl:text><xsl:apply-templates select="./Argument" mode="gen_pynative_decl"/>
        <xsl:variable name="is_fundamental"><xsl:apply-templates mode="is_fundamental" select="."/></xsl:variable>
        <xsl:variable name="prefix"><xsl:if test="$is_fundamental='true'">self-></xsl:if></xsl:variable>
        static const char *kwlist[] = { <xsl:for-each select="./Argument">&quot;<xsl:choose><xsl:when test="@name"><xsl:value-of select="@name"/></xsl:when><xsl:otherwise>Anon<xsl:value-of select="@id"/></xsl:otherwise></xsl:choose>&quot;</xsl:for-each><xsl:if test="count(./Argument)>0">,</xsl:if> NULL};<xsl:text>
            
        if ( status !=0 ){
            status = PyArg_ParseTupleAndKeywords(args, kwds, </xsl:text><xsl:value-of select="$format"/><xsl:text>, kwlist</xsl:text><xsl:if test="count(./Argument)>0">, </xsl:if><xsl:for-each select="./Argument">&amp;<xsl:value-of select="$prefix"/><xsl:choose><xsl:when test="@name"><xsl:value-of select="@name"/></xsl:when><xsl:otherwise>Anon_<xsl:value-of select="../@id"/></xsl:otherwise></xsl:choose><xsl:if test="position()!=last()">,</xsl:if></xsl:for-each><xsl:text>);
            if ( 0 == status </xsl:text><xsl:for-each select="./Argument">   <xsl:variable name="is_fundamental"><xsl:apply-templates select="." mode="is_fundamental"/></xsl:variable>
                <xsl:variable name="argtype" select="@type"/>
                <xsl:variable name="deref"><xsl:if test="//ReferenceType[@id=$argtype]">*</xsl:if></xsl:variable>
                <xsl:variable name="name"><xsl:choose>
                    <xsl:when test="@name"><xsl:value-of select="@name"/></xsl:when>
                    <xsl:otherwise>Anon_<xsl:value-of select="position()"/></xsl:otherwise>
                </xsl:choose></xsl:variable><xsl:if test="$is_fundamental!='true'"> &amp;&amp; <xsl:value-of select="$name"/></xsl:if></xsl:for-each>)<xsl:text>{
                </xsl:text><xsl:for-each select="./Argument">
                    <xsl:variable name="is_fundamental"><xsl:apply-templates select="." mode="is_fundamental"/></xsl:variable>
                    <xsl:variable name="argtype" select="@type"/>
                    <xsl:variable name="name"><xsl:choose>
                        <xsl:when test="@name"><xsl:value-of select="@name"/></xsl:when>
                        <xsl:otherwise>Anon_<xsl:value-of select="../@id"/></xsl:otherwise>
                    </xsl:choose></xsl:variable>
                    <xsl:variable name="deref"><xsl:if test="//ReferenceType[@id=$argtype]">*</xsl:if></xsl:variable>
                    <xsl:if test="$is_fundamental!='true'">
                    <xsl:apply-templates select="//*[@id=$argtype]" mode="get_namespace"/>::<xsl:apply-templates select="//*[@id=$argtype]" mode="gen_typename"/><xsl:text> </xsl:text><xsl:value-of select="$name"/>_obj  = <xsl:value-of select="$deref"/><xsl:value-of select="$name"/>)-&gt;_object;
                    </xsl:if>
                </xsl:for-each><xsl:choose><xsl:when test="//*[@id=$resulttypeid]/@name!='void'"><xsl:text>
                auto result = </xsl:text>self-&gt;_object-&gt;<xsl:value-of select="$realname"/>(<xsl:for-each select="./Argument"><xsl:variable name="name"><xsl:choose><xsl:when test="@name"><xsl:value-of select="@name"/></xsl:when><xsl:otherwise>Anon_<xsl:value-of select="../@id"/>;
               </xsl:otherwise>
                </xsl:choose></xsl:variable>
                <xsl:variable name="is_fundamental"><xsl:apply-templates mode="is_fundamental" select="."/></xsl:variable><xsl:choose>
                    <xsl:when test="$is_fundamental='true'"><xsl:value-of select="$name"/></xsl:when>
                    <xsl:otherwise><xsl:value-of select="$name"/>_obj</xsl:otherwise>
                        </xsl:choose><xsl:if test="position()!=last()">,</xsl:if></xsl:for-each><xsl:text>);
                </xsl:text><xsl:apply-templates select="//*[@id=$resulttypeid]" mode="gen_returnlogic"/><xsl:text>
            }
        }
    }
    </xsl:text></xsl:when><xsl:otherwise><xsl:text>
                </xsl:text>self-&gt;_object-&gt;<xsl:value-of select="$realname"/>(<xsl:for-each select="./Argument"><xsl:variable name="name"><xsl:choose><xsl:when test="@name"><xsl:value-of select="@name"/></xsl:when><xsl:otherwise>Anon_<xsl:value-of select="../@id"/>;
                </xsl:otherwise>
                </xsl:choose></xsl:variable>
                    <xsl:variable name="is_fundamental"><xsl:apply-templates mode="is_fundamental" select="."/></xsl:variable><xsl:choose>
                        <xsl:when test="$is_fundamental='true'"><xsl:value-of select="$name"/></xsl:when>
                        <xsl:otherwise><xsl:value-of select="$name"/>_obj</xsl:otherwise>
                    </xsl:choose><xsl:if test="position()!=last()">,</xsl:if></xsl:for-each><xsl:text>);
                    </xsl:text><xsl:apply-templates select="//*[@id=$resulttypeid]" mode="gen_returnlogic"/><xsl:text>
            }
        }
    }
    </xsl:text></xsl:otherwise></xsl:choose></xsl:for-each></xsl:if> <xsl:text>
    return result_obj;
}
</xsl:text>
    </xsl:template>
    
    <xsl:template match="Method[@static='1']" mode="gen_pynative_decl">
        <xsl:variable name="id" select="@id"/>
        <xsl:variable name="resulttypeid" select="@returns"/>
        <xsl:variable name="context" select="@context"/>
        <xsl:variable name="name" select="@name"/>
        <xsl:variable name="realname" select="@name"/>
        <xsl:variable name="namespace"><xsl:apply-templates select="//*[@id=$context]" mode="get_namespace"/></xsl:variable>
        <xsl:variable name="namespace_prefix"><xsl:apply-templates select="//*[@id=$context]" mode="get_namespace_prefix"/></xsl:variable>
        <xsl:variable name="format">TO_BE_IMPLEMENTED</xsl:variable>
        <xsl:variable name="py_classname"><xsl:value-of select="$namespace_prefix"/><xsl:apply-templates select="//*[@id=$context]" mode="gen_typename"/></xsl:variable>
        <xsl:if test="//Method[@context=$context and @name=$name][1]/@id=$id">
static PyObject*<xsl:text>
</xsl:text><xsl:value-of select="$py_classname"/>__<xsl:value-of select="@name"/>(PyObject *args, PyObject *kwds){<xsl:text>
    int status = -1;
</xsl:text><xsl:for-each select="//Method[@context=$context and @name=$name]"><xsl:text>
    {
        PyObject * result_obj = NULL;
       
        </xsl:text><xsl:apply-templates select="./Argument" mode="gen_pynative_decl2"/>
    <xsl:variable name="is_fundamental"><xsl:apply-templates mode="is_fundamental" select="."/></xsl:variable>
    <xsl:variable name="prefix"><xsl:if test="$is_fundamental='true'">self-></xsl:if></xsl:variable><xsl:text>
        static const char *kwlist[] = { </xsl:text><xsl:for-each select="./Argument">&quot;<xsl:choose><xsl:when test="@name"><xsl:value-of select="@name"/>/></xsl:when><xsl:otherwise>Anon<xsl:value-of select="@id"/></xsl:otherwise></xsl:choose>&quot;</xsl:for-each><xsl:if test="count(./Argument)>0">,</xsl:if> NULL};<xsl:text>
            
        if ( status !=0 ){
            status = PyArg_ParseTupleAndKeywords(args, kwds, </xsl:text><xsl:value-of select="$format"/><xsl:text>, kwlist</xsl:text><xsl:if test="count(./Argument)>0">, </xsl:if><xsl:for-each select="./Argument">&amp;<xsl:value-of select="$prefix"/><xsl:choose><xsl:when test="@name"><xsl:value-of select="@name"/></xsl:when><xsl:otherwise>Anon_<xsl:value-of select="../@id"/></xsl:otherwise></xsl:choose><xsl:if test="position()!=last()">,</xsl:if></xsl:for-each><xsl:text>);
            if ( 0 == status </xsl:text><xsl:for-each select="./Argument">   <xsl:variable name="is_fundamental"><xsl:apply-templates select="." mode="is_fundamental"/></xsl:variable>
                <xsl:variable name="argtype" select="@type"/>
                <xsl:variable name="deref"><xsl:if test="//ReferenceType[@id=$argtype]">*</xsl:if></xsl:variable>
                <xsl:variable name="name"><xsl:choose>
                    <xsl:when test="@name"><xsl:value-of select="@name"/></xsl:when>
                    <xsl:otherwise>Anon_<xsl:value-of select="position()"/></xsl:otherwise>
                </xsl:choose></xsl:variable><xsl:if test="$is_fundamental!='true'"> &amp;&amp; <xsl:value-of select="$name"/></xsl:if></xsl:for-each><xsl:text>){
                </xsl:text><xsl:for-each select="./Argument">
                    <xsl:variable name="is_fundamental"><xsl:apply-templates select="." mode="is_fundamental"/></xsl:variable>
                    <xsl:variable name="argtype" select="@type"/>
                    <xsl:variable name="name"><xsl:choose>
                        <xsl:when test="@name"><xsl:value-of select="@name"/></xsl:when>
                        <xsl:otherwise>Anon_<xsl:value-of select="../@id"/></xsl:otherwise>
                    </xsl:choose></xsl:variable>
                    <xsl:variable name="deref"><xsl:if test="//ReferenceType[@id=$argtype]">*</xsl:if></xsl:variable>
                    <xsl:if test="$is_fundamental!='true'">
                        <xsl:apply-templates select="//*[@id=$argtype]" mode="get_namespace"/>::<xsl:apply-templates select="//*[@id=$argtype]" mode="gen_typename"/><xsl:text> </xsl:text><xsl:value-of select="$name"/>_obj  = <xsl:value-of select="$deref"/><xsl:value-of select="$name"/>?((<xsl:apply-templates select="//*[@id=$argtype]" mode="gen_realtypename"/>)<xsl:value-of select="$name"/>)-&gt;_object:NULL;
                    </xsl:if>
                </xsl:for-each><xsl:choose><xsl:when test="//*[@id=$resulttypeid]/@name!='void'"><xsl:text>
                auto result = </xsl:text><xsl:value-of select="$namespace"/>::<xsl:apply-templates select="//*[@id=$context]" mode="gen_typename"/>::<xsl:value-of select="$realname"/>(<xsl:for-each select="./Argument"><xsl:variable name="name"><xsl:choose><xsl:when test="@name"><xsl:value-of select="@name"/></xsl:when><xsl:otherwise>Anon_<xsl:value-of select="../@id"/>;
                </xsl:otherwise>
                </xsl:choose></xsl:variable>
                    <xsl:variable name="is_fundamental"><xsl:apply-templates mode="is_fundamental" select="."/></xsl:variable><xsl:choose>
                        <xsl:when test="$is_fundamental='true'"><xsl:value-of select="$name"/></xsl:when>
                        <xsl:otherwise><xsl:value-of select="$name"/>_obj</xsl:otherwise>
                    </xsl:choose><xsl:if test="position()!=last()">,</xsl:if></xsl:for-each><xsl:text>);
                    </xsl:text><xsl:apply-templates select="//*[@id=$resulttypeid]" mode="gen_returnlogic"/><xsl:text>
            }
       }
    }
    </xsl:text></xsl:when><xsl:otherwise><xsl:text>
                </xsl:text><xsl:value-of select="$namespace"/>::<xsl:apply-templates select="//*[@id=$context]" mode="gen_typename"/>::<xsl:value-of select="$realname"/>(<xsl:for-each select="./Argument"><xsl:variable name="name"><xsl:choose><xsl:when test="@name"><xsl:value-of select="@name"/></xsl:when><xsl:otherwise>Anon_<xsl:value-of select="../@id"/>;
                </xsl:otherwise>
                </xsl:choose></xsl:variable>
                    <xsl:variable name="is_fundamental"><xsl:apply-templates mode="is_fundamental" select="."/></xsl:variable><xsl:choose>
                        <xsl:when test="$is_fundamental='true'"><xsl:value-of select="$name"/></xsl:when>
                        <xsl:otherwise><xsl:value-of select="$name"/>_obj</xsl:otherwise>
                    </xsl:choose><xsl:if test="position()!=last()">,</xsl:if></xsl:for-each><xsl:text>);
                    </xsl:text><xsl:apply-templates select="//*[@id=$resulttypeid]" mode="gen_returnlogic"/><xsl:text>
            }
        }
    }
    </xsl:text></xsl:otherwise></xsl:choose></xsl:for-each> <xsl:text>
    return result_obj;
}
</xsl:text></xsl:if>
    </xsl:template>
    
    
   <!-- ==================================
       Generate member defns
       ================================== -->
    <xsl:template match="Class|Struct" mode="gen_pymembers">
        <xsl:variable name="name"><xsl:choose><xsl:when test="@name"><xsl:value-of select="@name"/></xsl:when><xsl:otherwise>Anon_<xsl:value-of select="@id"/></xsl:otherwise></xsl:choose></xsl:variable>
        <xsl:variable name="classid" select="@id"/>
         <xsl:variable name="namespace"><xsl:apply-templates select="." mode="get_namespace"></xsl:apply-templates></xsl:variable>
         <xsl:variable name="namespace_prefix"><xsl:apply-templates select="." mode="get_namespace_prefix"/></xsl:variable>
        <xsl:variable name="py_classname"><xsl:value-of select="$namespace_prefix"/><xsl:value-of select="$name"/></xsl:variable><xsl:text>
static PyMemberDef </xsl:text><xsl:value-of select="$py_classname"/><xsl:text>_members[] =   {
        </xsl:text><xsl:for-each select="//Field[@context=$classid and (@access='public' or @access='protected') ]">
            &quot;<xsl:value-of select="@name"/>&quot;, T_OBJECT_EX, offset_of(<xsl:value-of select="$py_classname"/>,<xsl:value-of select="@name"/>, 0, ""},
    </xsl:for-each><xsl:text>{NULL}
};
    </xsl:text>
    </xsl:template>
    <!-- ===================
        Generate real method name
       ===================== -->
    <xsl:template match="Method" mode="get_realname"><xsl:variable name="returntypeid" select="@type"/><xsl:variable name="returntype"><xsl:apply-templates select="//*[@id=$returntypeid]" mode="gen_typename"/></xsl:variable><xsl:choose><xsl:when test="@name='='">assign</xsl:when>
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
    <xsl:template match="Method" mode="gen_methflags"><xsl:if test="@static='1'">METH_CLASS |</xsl:if> METH_VARARGS| METH_KEYWORDS</xsl:template>
    
    <!-- ===============================
        Generate native python definition for a class or struct
        ================================ -->
     <xsl:template match="Struct|Class" mode="gen_pynative">
         <xsl:variable name="classid" select="@id"/>
         <xsl:variable name="name"><xsl:choose><xsl:when test="@name"><xsl:value-of select="@name"/></xsl:when><xsl:otherwise>Anon_<xsl:value-of select="@id"/></xsl:otherwise>
         </xsl:choose></xsl:variable>
         <xsl:variable name="namespace"><xsl:apply-templates select="." mode="get_namespace"></xsl:apply-templates></xsl:variable>
         <xsl:variable name="namespace_prefix"><xsl:apply-templates select="." mode="get_namespace_prefix"/></xsl:variable>
         <xsl:variable name="py_classname"><xsl:value-of select="$namespace_prefix"/><xsl:value-of select="$name"/></xsl:variable>
<xsl:text>
static void
</xsl:text><xsl:value-of select="$py_classname"/><xsl:text>_dealloc(</xsl:text><xsl:value-of select="$py_classname"/><xsl:text>* self)
{
    if(!self) return;
    if (!self-&gt;_is_c_reference){
        delete self->_object;
    }
    self->_object = NULL;
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
        self->_object = NULL;
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
    </xsl:text><xsl:if test="count(//Constructor[@context=$classid])=0">self-&gt;_object = new <xsl:value-of select="$py_classname"/>();
    status = self-&gt;get_object(self)?0:-1; </xsl:if><xsl:apply-templates select="//Constructor[@context=$classid and (@access='public' or @access='protected')]" mode="gen_pynative"/><xsl:text>
    if (status != 0){
       self-&gt;_object = NULL;
       status = 0;
    }
    return status;
}

</xsl:text><xsl:apply-templates select="." mode="gen_pymembers"/><xsl:text>
</xsl:text><xsl:apply-templates select="//Method[@context=$classid and (@access='public' or @access='private')]" mode="gen_pynative_decl"/><xsl:text>
static PyTypeObject = </xsl:text><xsl:value-of select="$py_classname"/><xsl:text>Type{
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
             "</xsl:text><xsl:value-of select="$namespace"/><xsl:text> objects",           /* tp_doc */"</xsl:text><xsl:value-of select="$py_classname"/><xsl:text> objects",           /* tp_doc */
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
             
static PyMethodDef </xsl:text><xsl:value-of select="$namespace_prefix"/><xsl:text>_methods[] = {
             </xsl:text><xsl:for-each select="//Method[@context=$classid and (@access='public' or @access='protected')]">
                 {&quot;<xsl:apply-templates select="." mode="get_realname"/>&quot;,  (PyCFunction)<xsl:value-of select="$py_classname"/>__<xsl:apply-templates select="." mode="get_realname"/>,<xsl:apply-templates select="." mode="gen_methflags"/>, &quot;&quot;},
             </xsl:for-each><xsl:text>NULL}  /* Sentinel */
};

</xsl:text>
     </xsl:template>     

     <!-- =====================
         ROOT processing node
         ====================== -->
     <xsl:template match="/GCC_XML"><xsl:text>
#include </xsl:text>&lt;<xsl:text>PyPy.h</xsl:text>&gt;<xsl:text>
#include </xsl:text>&lt;<xsl:text>structmember.h</xsl:text>&gt;<xsl:text>
</xsl:text><xsl:for-each select="//File[not(starts-with(@name,'/usr/share/castxml')) and @name!='&lt;builtin&gt;']"><xsl:text>
#include &lt;</xsl:text><xsl:value-of select="@name"/><xsl:text>&gt;
</xsl:text>
</xsl:for-each><xsl:text>
</xsl:text>
         <xsl:for-each select="//Namespace[@name!='::']">
             <xsl:variable name="scopeid" select="@id"/>
             <xsl:variable name="namespace"><xsl:apply-templates select="." mode="get_namespace"/></xsl:variable>
             <xsl:variable name="pynamespace"><xsl:call-template name="string-replace-all"><xsl:with-param name="text" select="$namespace"/><xsl:with-param name="replace">::</xsl:with-param><xsl:with-param name="by">.</xsl:with-param></xsl:call-template></xsl:variable>
             <xsl:variable name="namespace_prefix"><xsl:apply-templates select="." mode="get_namespace_prefix"/></xsl:variable>
<xsl:for-each select="//Struct[@context=$scopeid]|//Class[@context=$scopeid]">
    <xsl:variable name="namespace"><xsl:apply-templates select="." mode="get_namespace"/></xsl:variable>
    <xsl:variable name="namespace_prefix"><xsl:apply-templates select="." mode="get_namespace_prefix"/></xsl:variable>
    <xsl:variable name="py_classname"><xsl:apply-templates select="." mode="gen_typename"></xsl:apply-templates></xsl:variable>
    <xsl:text>
struct </xsl:text><xsl:value-of select="$namespace_prefix"/><xsl:value-of select="$py_classname"/><xsl:text>{
    PyObject_HEAD
    </xsl:text><xsl:value-of select="$namespace"/>::<xsl:value-of select="@name"/><xsl:text>* _object;
    /* Type-specific fields go here. */
    PyObject* _owner;
    bool _is_c_reference;
    
    </xsl:text><xsl:value-of select="$namespace"/>::<xsl:value-of select="@name"/>*<xsl:text>
    get_object(struct </xsl:text><xsl:value-of select="$py_classname"/><xsl:text>* newowner);
};

</xsl:text>
</xsl:for-each>
         </xsl:for-each>
         <xsl:for-each select="//Namespace[@name!='::']">
             <xsl:variable name="scopeid" select="@id"/>
             <xsl:variable name="namespace"><xsl:apply-templates select="." mode="get_namespace"/></xsl:variable>
             <xsl:variable name="pynamespace"><xsl:call-template name="string-replace-all"><xsl:with-param name="text" select="$namespace"/><xsl:with-param name="replace">::</xsl:with-param><xsl:with-param name="by">.</xsl:with-param></xsl:call-template></xsl:variable>
             <xsl:variable name="namespace_prefix"><xsl:apply-templates select="." mode="get_namespace_prefix"/></xsl:variable>
             
<xsl:apply-templates select="//Struct[@context=$scopeid]|//Class[@context=$scopeid]" mode="gen_pynative"></xsl:apply-templates><xsl:text>

#ifndef PyMODINIT_FUNC	/* declarations for DLL import/export */
#define PyMODINIT_FUNC void
#endif


PyMODINIT_FUNC
init</xsl:text><xsl:value-of select="$namespace_prefix"/><xsl:text>(void) 
{
    int status = 0;
    PyObject* m;
    m = Py_InitModule3("</xsl:text><xsl:value-of select="$pynamespace"/><xsl:text>", </xsl:text><xsl:value-of select="$namespace"/><xsl:text>_methods,
             "Module wrapping C/C++ library </xsl:text><xsl:value-of select="$namespace"/><xsl:text>"q);
     
    </xsl:text><xsl:for-each select="//Struct[@context=$scopeid]|//Class[@context=$scopeid]"><xsl:variable name="name"><xsl:apply-templates select="." mode="gen_typename"/></xsl:variable>
    <xsl:variable name="py_classname"><xsl:value-of select="$namespace_prefix"/><xsl:value-of select="$name"/></xsl:variable><xsl:text>
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
</xsl:text>
             <xsl:for-each select="//Struct[@context=$scopeid]|Class[@context=$scopeid]">
                 <xsl:variable name="namespace"><xsl:apply-templates select="." mode="get_namespace"/></xsl:variable>
                 <xsl:variable name="namespace_prefix"><xsl:apply-templates select="." mode="get_namespace_prefix"/></xsl:variable>
                 <xsl:variable name="py_classname"><xsl:apply-templates select="." mode="gen_typename"></xsl:apply-templates></xsl:variable><xsl:text>
</xsl:text><xsl:value-of select="$namespace"/>::<xsl:value-of select="@name"/>* <xsl:value-of select="$py_classname"/>::<xsl:text>
get_object(struct </xsl:text><xsl:value-of select="$namespace_prefix"/><xsl:value-of select="$py_classname"/><xsl:text>* newowner){
   if( _is_c_reference &amp;&amp; _owner != (PyObject*)newowner ){
       _object = new </xsl:text><xsl:value-of select="$namespace"/>::<xsl:value-of select="@name"/><xsl:text>(*_object);
       _is_c_reference = false;
   }
   return _object;
}
                 </xsl:text>
             </xsl:for-each>
         </xsl:for-each>
     </xsl:template>
</xsl:stylesheet>