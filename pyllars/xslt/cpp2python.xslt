<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
     version="1.0">
    <xsl:param name="libname" select="NONAME"/>
    <xsl:output media-type="text" method="text"></xsl:output>

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
    
    <!-- =====================================
      * Get a basic ctypes from a given fundamental type name
      * @type the typedef name to convert to basic type
      ======================================== -->
    <xsl:template name="get_basic_type">
        <xsl:param name="type"/>
        <xsl:choose>
            <xsl:when test="$type='__int128'">(c_longlong*2)</xsl:when>
            <xsl:when test="$type='__uint28'">(c_ulonglong*2)</xsl:when>
            <xsl:when test="$type='long_int'">c_long</xsl:when>
            <xsl:when test="$type='long_unsigned_int'">c_ulong</xsl:when>
            <xsl:when test="$type='signed_char'">c_char</xsl:when>
            <xsl:when test="$type='char'">c_char</xsl:when>
            <xsl:when test="$type='short_int'">c_short</xsl:when>
            <xsl:when test="$type='unsigned_char'">c_byte</xsl:when>
            <xsl:when test="$type='short_unsigned_int'">c_ushort</xsl:when>
            <xsl:when test="$type='int'">c_int</xsl:when>            
            <xsl:when test="$type='unsigned_int'">c_uint</xsl:when>
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
            <xsl:when test="//Namespace[@id=$id]"><xsl:if test="//Namespace[@id=$context]/@name!='::'"><xsl:apply-templates select="//Namespace[@id=$context]" mode="get_namespace"><xsl:with-param name="id" select="$context"/></xsl:apply-templates>.</xsl:if><xsl:value-of select="@name"/></xsl:when>
            <xsl:otherwise><xsl:apply-templates select="//*[@id=$context]" mode="get_namespace"/></xsl:otherwise>
        </xsl:choose>
    </xsl:template>
    
    <!-- ================================
        Get a C-style namespace prefix
        ============================== -->
    <xsl:template match="//*" mode="get_namespace_prefix">
        <xsl:variable name="namespace"><xsl:apply-templates select="." mode="get_namespace"/></xsl:variable><xsl:choose>
            <xsl:when test="$namespace"><xsl:call-template name="string-replace-all"><xsl:with-param name="text" select="$namespace"/>
            <xsl:with-param name="replace">.</xsl:with-param>
            <xsl:with-param name="by" select="___"/></xsl:call-template>___</xsl:when>
    <xsl:otherwise></xsl:otherwise></xsl:choose></xsl:template>

    <!-- =============================
      * Get the namespace node for the gvien node
      ================================ -->
    <xsl:template match="//*" mode="get_ns_context">
        <xsl:variable name="context" select="@context"/>
        <xsl:choose>
            <xsl:when test="count(//Namespace[@id=$context])>0"><xsl:value-of select="//Namespace[@id=$context]/@id"/></xsl:when>
            <xsl:otherwise><xsl:apply-templates select="//*[@id=$context]" mode="get_ns_context"></xsl:apply-templates></xsl:otherwise>
        </xsl:choose>
    </xsl:template>

    <xsl:template match="//*" mode="get_contextid">
        <xsl:variable name="context" select="@context"/>
        <xsl:choose>
            <xsl:when test="$context!=''"><xsl:choose>
                <xsl:when test="count(//Namespace[@id=$context])>0"><xsl:value-of select="$context"/></xsl:when>
                <xsl:otherwise><xsl:apply-templates select="//*[@id=$context]"/></xsl:otherwise>
            </xsl:choose></xsl:when>
            <xsl:otherwise><xsl:variable name="type" select="@type"/><xsl:apply-templates select="//*[@id=$type]" mode="get_contextid"/></xsl:otherwise>
        </xsl:choose>
    </xsl:template>
    
    <xsl:template  match="//*" mode="gen_type_imports">
        <xsl:param name="scopeid"/>
        <xsl:variable name="context" select="@context"/>
        <xsl:variable name="type" select="@type"/>
        <xsl:variable name="ptrtypeid" select="//PointerType[@id=$type]/@type"/>
        <xsl:variable name="namespace"><xsl:apply-templates select="." mode="get_namespace"/></xsl:variable>
        <xsl:choose>
            <xsl:when test="$context!='' and $context!=$scopeid">
                <xsl:if test="count(//Namespace[@id=$context])>0"><xsl:variable name="typename"><xsl:apply-templates select="." mode="typename"/></xsl:variable>
#@add_import(&quot;<xsl:value-of select="$namespace"/>&quot;,&quot;<xsl:value-of select="@name"/>&quot;)</xsl:if>
                <xsl:apply-templates select="//*[@id=$context]" mode="gen_type_imports"><xsl:with-param name="scopeid" select="$scopeid"/></xsl:apply-templates>
            </xsl:when>
            <xsl:when test="count(//FunctionType[@id=$type])>0 or count(//FunctionType[@id=$ptrtypeid])>0"><xsl:text>                
</xsl:text>#@add_func_defn(&quot;func<xsl:value-of select="@id"/>&quot;,&quot;<xsl:apply-templates select="." mode="typename"/>)</xsl:when>
            <xsl:otherwise><xsl:apply-templates select="//*[@id=$type]" mode="get_type_imports"/></xsl:otherwise>
        </xsl:choose>
    </xsl:template>
    
    <xsl:template match="//Field|//Argument" mode="gen_imports">
        <xsl:param name="scopeid"/>
        <xsl:variable name="typee" select="@type"/>
        <xsl:apply-templates select="." mode="gen_type_imports"><xsl:with-param name="scopeid" select="$scopeid"/></xsl:apply-templates>
    </xsl:template>
    
    <!-- how to transform elements into typenames !-->
    <xsl:template match="Typedef" mode="typename"><xsl:value-of select="@name"/></xsl:template>
       
    <xsl:template match="CvQualifiedType" mode="typename">
        <xsl:param name="scopeid"/>
        <xsl:variable name="type" select="@type"/>
        <xsl:variable name="typename"><xsl:apply-templates select="//*[@id=$type]" mode="typename"><xsl:with-param name="scopeid" select="$scopeid"/></xsl:apply-templates></xsl:variable>
        <xsl:if test="@const='1'"><xsl:value-of select="$typename"/></xsl:if>
        <xsl:if test="@const!='1'"><xsl:value-of select="$typename"/></xsl:if>
    </xsl:template>
    
    <xsl:template match="FundamentalType" mode="typename">
        <xsl:call-template name="get_basic_type"><xsl:with-param name="type"><xsl:value-of select="translate(@name,' ','_')"/></xsl:with-param></xsl:call-template>    
    </xsl:template>
    
    <xsl:template match="Struct[@name='']" mode="typename">
        <xsl:param name="scopeid"/>
        <xsl:variable name="contextid"><xsl:apply-templates select="." mode="get_ns_context"/></xsl:variable>
        <xsl:variable name="namespace"><xsl:apply-templates select="//*[@id=$contextid]" mode="get_namespace"/></xsl:variable>
        <xsl:variable name="prefix"><xsl:if test="$contextid!=$scopeid"><xsl:value-of select="$namespace"/>.</xsl:if></xsl:variable>
        <xsl:value-of select="$prefix"/>CStruct_Anon_<xsl:value-of select="@id"/>
    </xsl:template>
    
    <xsl:template match="Struct[@name!='']" mode="typename">
        <xsl:param name="scopeid"/>
        <xsl:variable name="contextid"><xsl:apply-templates select="." mode="get_ns_context"/></xsl:variable>
        <xsl:variable name="namespace"><xsl:apply-templates select="//*[@id=$contextid]" mode="get_namespace"/></xsl:variable>
        <xsl:variable name="prefix"><xsl:if test="$contextid!=$scopeid"><xsl:value-of select="$namespace"/>.</xsl:if></xsl:variable>
        <xsl:value-of select="$prefix"/><xsl:value-of select="translate(@name,'&lt;&gt;,: ','_____')"/>
    </xsl:template>
    
    <xsl:template match="Class[@name!='']" mode="typename">
        <xsl:param name="scopeid"/>
        <xsl:variable name="contextid"><xsl:apply-templates select="." mode="get_ns_context"/></xsl:variable>
        <xsl:variable name="namespace"><xsl:apply-templates select="//*[@id=$contextid]" mode="get_namespace"/></xsl:variable>
        <xsl:variable name="prefix"><xsl:if test="$contextid!=$scopeid"><xsl:value-of select="$namespace"/>.</xsl:if></xsl:variable>
<xsl:value-of select="$prefix"/><xsl:value-of select="translate(@name,'&lt;&gt;,: ','_____')"/>
    </xsl:template>   
    
    <xsl:template match="Union" mode="typename">
CUnion_Anon_<xsl:value-of select="@id"/>
    </xsl:template>
    
    <xsl:template match="PointerType|ReferenceType" mode="typename">
         <xsl:param name="scopeid"/>
        <xsl:param name="for_functype"/>
        <xsl:variable name="type" select="@type"/>
        <xsl:variable name="typename"><xsl:apply-templates select="//*[@id=$type]" mode="typename"><xsl:with-param name="scopeid" select="$scopeid"/></xsl:apply-templates></xsl:variable>
        <xsl:choose>
            <xsl:when test="count(//FunctionType[@id=$type])>0">func<xsl:value-of select="$type"/></xsl:when>
            <xsl:when test="$typename='void'">c_void_p</xsl:when>
            <xsl:when test="$typename='char'">c_char_p</xsl:when>
            <xsl:when test="$typename='c_char'">c_char_p</xsl:when>
            <xsl:when test="//Class[@id=$type]|//Struct[@id=$type] and $for_functype=''"><xsl:value-of select="$typename"/></xsl:when>
            <xsl:otherwise>POINTER(<xsl:choose><xsl:when test="$for_functype!=''">_FwdClassDecl_<xsl:value-of select="$typename"/></xsl:when><xsl:otherwise><xsl:value-of select="$typename"/></xsl:otherwise></xsl:choose>)</xsl:otherwise></xsl:choose>
    </xsl:template>
    
    <xsl:template match="ArrayType" mode="typename">
        <xsl:param name="scopeid"/>
        <xsl:variable name="type" select="@type" />
        <xsl:variable name="max" select="@max"/>
        <xsl:variable name="typename"><xsl:apply-templates select="//*[@id=$type]" mode="typename">
        <xsl:with-param name="scopeid" select="$scopeid"/>
        </xsl:apply-templates></xsl:variable>
<xsl:if test="$max=''">POINTER(<xsl:value-of select="$typename"/>)</xsl:if>
<xsl:if test="$max!=''">(<xsl:value-of select="$typename"/>*<xsl:value-of select="number($max)+1"/>)</xsl:if>
    </xsl:template>
    
    <xsl:template match="FunctionType[@id]" mode="typename">func<xsl:value-of select="@id"/></xsl:template>
    
    <xsl:template match="Enumeration" mode="typename"><xsl:param name="pos"/>c_uint</xsl:template>
    
    <!-- ================================================== -->
    <!-- Generate field name/type list for ctypes structures-->
    <!-- ================================================== -->
    
    <!-- 
      * Generate the callaable python argument for a call to native lib
      -->
    <xsl:template match="//Argument" mode="gen_callable">
        <xsl:param name="scopeid"/>
        <xsl:variable name="context" select="@type"/>
        <xsl:variable name="name"><xsl:choose><xsl:when test="@name!=''"><xsl:value-of select="@name"/></xsl:when><xsl:otherwise>arg<xsl:value-of select="position()"/></xsl:otherwise>
        </xsl:choose></xsl:variable>
        <xsl:choose>
            <xsl:when test="count(//ArrayType[@id=$context]|//PointerType[@id=$context]|//FundamentalType[@id=$context]|//Typedef[@id=$context])>0"><xsl:value-of select="$name"/></xsl:when>
            <xsl:otherwise><xsl:value-of select="$name"/>._cobject</xsl:otherwise>
        </xsl:choose>
        <xsl:apply-templates select="." mode="gen_imports"><xsl:with-param name="scopeid" select="$scopeid"/></xsl:apply-templates>
    </xsl:template>
    
    <!-- 
      * Generate meta data: import dependencnies and typedefinitions needed for a Field declaration
      * @param scopeid  The id of the Namespace to which the meta data is to be generated
      -->
    <xsl:template match="//Field|//Argument" mode="gen_meta_data">
        <xsl:param name="scopeid"/>
        <xsl:param name="typeid" select="@type"/>
        <xsl:variable name="contextid"><xsl:apply-templates select="//*[@id=$typeid]" mode="get_contextid"/></xsl:variable>
        <xsl:choose><xsl:when test="$contextid!=''">        
        <xsl:if test="$contextid!=$scopeid">
             <xsl:variable name="prefix"><xsl:apply-templates select="//*[@id=$contextid]" mode="get_namespace"/></xsl:variable><xsl:text>
</xsl:text>
<xsl:apply-templates select="//*[@id=$typeid]" mode="gen_type_imports"><xsl:with-param name="scopeid" select="$scopeid"/></xsl:apply-templates>
</xsl:if>    
        </xsl:when><xsl:otherwise>
            <xsl:apply-templates select="."><xsl:with-param name="scopeid" select="$scopeid"></xsl:with-param><xsl:with-param name="typeid" select="@type"/></xsl:apply-templates>
        </xsl:otherwise></xsl:choose>
    </xsl:template>

    <!-- 
       * Generate a field definition for a Struct as it would appear in a ctypes field declaration
       -->
    <xsl:template match="//Field" mode="gen_field_entry">
        <xsl:param name="scopeid"/>
        <xsl:variable name="id" select="@id"/>
        <xsl:variable name="prefix"><xsl:if test="@access!='public'">__</xsl:if></xsl:variable>
        <xsl:variable name="name"><xsl:choose>
            <xsl:when test="@name!=''"><xsl:value-of select="$prefix"/><xsl:value-of select="@name"/></xsl:when>
            <xsl:otherwise><xsl:value-of select="$prefix"/>member<xsl:value-of select="@id"/></xsl:otherwise>
        </xsl:choose></xsl:variable>
        <xsl:variable name="type" select="@type"/>(&quot;<xsl:value-of select="$name"/>&quot;,<xsl:apply-templates select="//*[@id=$type]" mode="typename">
        <xsl:with-param name="scopeid" select="$scopeid"/>
        </xsl:apply-templates>),
    </xsl:template>
   
    <!-- how to transform elements into typedefinitions !-->
    <xsl:template match="FundamentalType" mode="gen_meta_data">
        <xsl:param name="name"/>
    </xsl:template>
    
    <xsl:template match="Struct" mode="typedefinition">
        <xsl:param name="scopeid"/><xsl:variable name="id" select="@id"/>
        <xsl:variable name="prefix"><xsl:if test="@name=''">CStruct_Anon<xsl:value-of select="@id"/></xsl:if></xsl:variable><xsl:text>
</xsl:text><xsl:apply-templates select="//Field[@context=$id]" mode="gen_meta_data"><xsl:with-param name="scopeid" select="$scopeid"/></xsl:apply-templates><xsl:text>
class </xsl:text><xsl:value-of select="$prefix"/><xsl:value-of select="@name"/>( _FwdDecl_<xsl:value-of select="$prefix"/><xsl:value-of select="@name"/>):<xsl:text>
    _fields_ = [ </xsl:text><xsl:if test="./Method[@virtual='1']|./OperatorMethod[@virtual='1']">("__vtable",c_void_p),</xsl:if><xsl:apply-templates mode="gen_field_entry" select="//Field[@context=$id]"><xsl:with-param name="scopied" select="$scopeid"/></xsl:apply-templates>]
<xsl:apply-templates select="//Field[@context=$id]" mode="gen_imports"><xsl:with-param name="scopeid" select="$scopeid"/></xsl:apply-templates><xsl:text>
</xsl:text><xsl:apply-templates select="//Field[@context=$id]" mode="gen_meta_data"><xsl:with-param name="scopeid" select="$scopeid"/></xsl:apply-templates><xsl:text>

</xsl:text><xsl:apply-templates select="//Method[@context=$id]" mode="typedefinition"><xsl:with-param name="typename"><xsl:value-of select="$prefix"/><xsl:value-of select="@name"/></xsl:with-param><xsl:with-param name="scopeid" select="$scopeid"/></xsl:apply-templates>
    </xsl:template>
   
    <xsl:template match="Union" mode="typedefinition">
        <xsl:param name="pos"/>
        <xsl:param name="scopeid"/>
        <xsl:param name="name"/><xsl:variable name="id" select="@id"/>
        <xsl:apply-templates select="." mode="generate_definitions"><xsl:with-param name="scopeid" select="$scopeid"/></xsl:apply-templates><xsl:text>        

class CUnion_Anon_</xsl:text><xsl:value-of select="@id"/><xsl:text>( Union ):
    _fields_ = [ </xsl:text><xsl:apply-templates mode="gen_field_entry" select="//Field[@context=$id]"><xsl:with-param name="scopeid" select="$scopeid"/></xsl:apply-templates>]
        <xsl:text>
    
</xsl:text>
    </xsl:template>
    
    <xsl:template match="Constructor" mode="generate_def">
        <xsl:param name="scopeid"/>
        <xsl:variable name="id" select="@id"/>
        <xsl:variable name="context" select="@context"/>
        <xsl:variable name="typename"><xsl:apply-templates select="//*[@id=$context]" mode="typename"><xsl:with-param name="scopeid" select="$scopeid"></xsl:with-param></xsl:apply-templates></xsl:variable><xsl:text>

    @staticmethod
    def new</xsl:text><xsl:value-of select="position()"/>(self, <xsl:for-each select="./Argument">
        <xsl:choose><xsl:when test="@name!=''"><xsl:value-of select="@name"/>,</xsl:when><xsl:otherwise>arg<xsl:value-of select="position()"/>,</xsl:otherwise></xsl:choose></xsl:for-each>):<xsl:variable name="typeid" select="@type"/><xsl:variable name="name"> <xsl:choose><xsl:when test="@name!=''"><xsl:value-of select="@name"/>,</xsl:when><xsl:otherwise>arg<xsl:value-of select="position()"/>,</xsl:otherwise></xsl:choose></xsl:variable><xsl:text>
        </xsl:text>&quot;&quot;&quot;<xsl:text>
        Signature: </xsl:text><xsl:for-each select="./Argument"><xsl:variable name="argname"><xsl:choose><xsl:when test="@name"><xsl:value-of select="@name"/></xsl:when><xsl:otherwise>arg<xsl:value-of select="position()"/></xsl:otherwise></xsl:choose></xsl:variable><xsl:variable name="argtype" select="@type"/><xsl:text>
           </xsl:text><xsl:value-of select="$argname"/>:<xsl:apply-templates select="//*[@id=$argtype]" mode="typename"/><xsl:text>            
        </xsl:text></xsl:for-each>&quot;&quot;&quot;<xsl:for-each select="./Argument"><xsl:variable name="argname"><xsl:choose><xsl:when test="@name"><xsl:value-of select="@name"/></xsl:when><xsl:otherwise>arg<xsl:value-of select="position()"/></xsl:otherwise></xsl:choose></xsl:variable><xsl:text>
        assert(isinstance(</xsl:text><xsl:value-of select="$argname"/>,<xsl:value-of select="$typename"/>))</xsl:for-each><xsl:text>
        obj =  </xsl:text><xsl:value-of select="$typename"/>\<xsl:text>
            ( native.</xsl:text><xsl:value-of select="$typename"/>_new(<xsl:for-each select="./Argument"><xsl:apply-templates select="." mode="gen_callable"><xsl:with-param name="scopeid" select="$scopeid"/></xsl:apply-templates>, </xsl:for-each>))<xsl:text>
        return obj
        </xsl:text>
   </xsl:template>
    
    <xsl:template match="Method[not(@static)]|OperatorMethod[not(@static)]" mode="typedefinition">
        <xsl:param name="scopeid"/>
        <xsl:param name="typename"/>
        <xsl:variable name="id" select="@id"/>
        <xsl:variable name="context" select="@context"/>
        <xsl:variable name="namespace_prefix"><xsl:apply-templates select="//*[@id=$context]" mode="get_namespace_prefix"/></xsl:variable>
        <xsl:variable name="mname" select="@name"/>
        <xsl:variable name="returns" select="@returns"/>
        <xsl:variable name="returntype">
            <xsl:apply-templates mode="typename" select="//*[@id=$returns]"><xsl:with-param name="scopeid" select="$scopeid"/></xsl:apply-templates>
        </xsl:variable>
        <xsl:variable name="realname"><xsl:choose><xsl:when test="@name='='">assign</xsl:when>
        <xsl:when test="@name='*='">__mul__</xsl:when>
        <xsl:when test="@name='+='">__iadd__</xsl:when>
        <xsl:when test="@name='/='">__div__</xsl:when>
        <xsl:when test="@name='-&gt;'">deref2</xsl:when>
            <xsl:when test="@name='=='">__cmp__</xsl:when>
            <xsl:when test="@name='!='">__notcmp__</xsl:when>
            <xsl:when test="@name='&lt;'">__lt__</xsl:when>
            <xsl:when test="@name='&gt;'">__gt__</xsl:when>
            <xsl:when test="@name='|='">__mod__</xsl:when>
        <xsl:when test="@name='*' and $returntype='c_char_p'">__str__</xsl:when>
        <xsl:when test="@name='*'">deref</xsl:when>
        <xsl:otherwise><xsl:value-of select="@name"/></xsl:otherwise></xsl:choose></xsl:variable>
        <xsl:variable name="count" select="count(//Method[@name=$mname and @context=$context and not(@static)])+count(//OperatorMethod[@name=$mname and @context=$context and not(@static)])"/>
        <xsl:variable name="ext"><xsl:if test="$count>1">_<xsl:value-of select="position()"/></xsl:if></xsl:variable><xsl:text>
</xsl:text><xsl:if test="$ext!='' and (//Method[@context=$context and @name=$mname and not(@static)][1]/@id=$id or //OperatorMethod[@context=$context and @name=$mname][1]/@id=$id)"><xsl:text>
    </xsl:text>def <xsl:value-of select="$realname"/>(self, *args):<xsl:text>
        </xsl:text>count = 0<xsl:for-each select="//Method[@context=$context and not(@static)]|//OperatorMethod[@context=$context]"><xsl:if test="@name=$mname"><xsl:text>
        </xsl:text><xsl:variable name="ext2">_<xsl:value-of select="position()"/></xsl:variable><xsl:text></xsl:text>if count==0 and len(*args) == <xsl:value-of select="count(./Argument)"/>:<xsl:text>
            try:
                </xsl:text>return self.<xsl:value-of select="$realname"/><xsl:value-of select="$ext2"/>(*args)<xsl:text>
                count += 1
            except:
                pass</xsl:text></xsl:if></xsl:for-each><xsl:text>
        if not count:
            raise Exception("No signature matching arguments")

</xsl:text></xsl:if><xsl:text>
    </xsl:text>def <xsl:value-of select="$realname"/><xsl:value-of select="$ext"/>(self, <xsl:for-each select="./Argument"><xsl:variable name="argname"><xsl:choose><xsl:when test="@name"><xsl:value-of select="@name"/></xsl:when><xsl:otherwise>arg<xsl:value-of select="position()"/></xsl:otherwise></xsl:choose></xsl:variable><xsl:value-of select="$argname"/>,</xsl:for-each>):<xsl:text>
        '''
        @returns: </xsl:text><xsl:if test="@returns=''">(void)</xsl:if><xsl:apply-templates select="//*[@id=$returns]" mode="typename"/>
        '''<xsl:for-each select="./Argument"><xsl:variable name="argname"><xsl:choose><xsl:when test="@name"><xsl:value-of select="@name"/></xsl:when><xsl:otherwise>arg<xsl:value-of select="position()"/></xsl:otherwise></xsl:choose></xsl:variable><xsl:variable name="typeid" select="@type"/><xsl:text>
        assert(isinstance(</xsl:text><xsl:value-of select="$argname"/>,<xsl:apply-templates select="//*[@id=$typeid]" mode="typename"><xsl:with-param name="scopeid" select="$scopeid"/></xsl:apply-templates>))</xsl:for-each><xsl:text>
        </xsl:text><xsl:if test="$returns!=''">return<xsl:text> </xsl:text></xsl:if><xsl:if test="//ReferenceType[@id=$returns]|//Class[@id=$returns]|//Struct[@id=$returns]"><xsl:apply-templates select="//Class[@id=$returns]|//Struct[@id=$returns]" mode="typename"><xsl:with-param name="scopeid" select="$scopeid"/></xsl:apply-templates>(</xsl:if>native.<xsl:value-of select="$namespace_prefix"/><xsl:value-of select="$typename"/>__<xsl:value-of select="$realname"/>(self._cobject, <xsl:for-each select="./Argument"><xsl:apply-templates select="." mode="gen_callable"/>,</xsl:for-each>)<xsl:if test="//ReferenceType[@id=$returns]|//Class[@id=$returns]|//Struct[@id=$returns]">)</xsl:if>
        <xsl:for-each select="./Argument"><xsl:variable name="typeid" select="@type"/><xsl:apply-templates select="//*[@id=$typeid]" mode="gen_imports"><xsl:with-param name="scopeid" select="$scopeid"></xsl:with-param></xsl:apply-templates>
        </xsl:for-each> 
        <xsl:apply-templates select=".//Argument" mode="gen_meta_data"><xsl:with-param name="scopeid" select="$scopeid"/></xsl:apply-templates>
    </xsl:template>

    <xsl:template match="Method[@static='1']|OperatorMethod[@static!='1']" mode="typedefinition">
        <xsl:param name="scopeid"/>
        <xsl:param name="typename"/>
        <xsl:variable name="id" select="@id"/>
        <xsl:variable name="context" select="@context"/>
        <xsl:variable name="namespace_prefix"><xsl:apply-templates select="//*[@id=$context]" mode="get_namespace_prefix"/></xsl:variable>
        <xsl:variable name="mname" select="@name"/>
        <xsl:variable name="returns" select="@returns"/>
        <xsl:variable name="returntype">
            <xsl:apply-templates mode="typename" select="//*[@id=$returns]"><xsl:with-param name="scopeid" select="$scopeid"/></xsl:apply-templates>
        </xsl:variable>
        <xsl:variable name="realname"><xsl:choose><xsl:when test="@name='='">assign</xsl:when>
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
            <xsl:otherwise><xsl:value-of select="@name"/></xsl:otherwise></xsl:choose></xsl:variable>
        <xsl:variable name="count" select="count(//Method[@name=$mname and @context=$context and @static='1'])+count(//OperatorMethod[@name=$mname and @context=$context and @static='1'])"/>
        <xsl:variable name="ext"><xsl:if test="$count>1">_<xsl:value-of select="position()"/></xsl:if></xsl:variable><xsl:text>
</xsl:text><xsl:if test="$ext!=''  and (//Method[@context=$context and @name=$mname and @static='1'][1]/@id=$id or //OperatorMethod[@context=$context and @name=$mname and @static='1'][1]/@id=$id)"><xsl:text>
    </xsl:text>@staticmethod
    def <xsl:value-of select="$realname"/>(*args):<xsl:text>
        </xsl:text>count = 0<xsl:for-each select="//Method[@context=$context and @static='1']|//OperatorMethod[@context=$context and @static='1']"><xsl:if test="@name=$mname"><xsl:text>
        </xsl:text><xsl:variable name="ext2">_<xsl:value-of select="position()"/></xsl:variable><xsl:text></xsl:text>if count==0 and len(*args) == <xsl:value-of select="count(./Argument)"/>:<xsl:text>
            try:
                </xsl:text>return <xsl:value-of select="$typename"/>.<xsl:value-of select="$realname"/><xsl:value-of select="$ext2"/>(*args)<xsl:text>
                count += 1
            except:
                pass</xsl:text></xsl:if></xsl:for-each><xsl:text>
        if not count:
            raise Exception("No signature matching arguments")

</xsl:text></xsl:if><xsl:text>
    </xsl:text>@staticmethod
    def <xsl:value-of select="$realname"/><xsl:value-of select="$ext"/>(<xsl:for-each select="./Argument"><xsl:variable name="argname"><xsl:choose><xsl:when test="@name"><xsl:value-of select="@name"/></xsl:when><xsl:otherwise>arg<xsl:value-of select="position()"/></xsl:otherwise></xsl:choose></xsl:variable><xsl:value-of select="$argname"/>,</xsl:for-each>):<xsl:text>
        '''
        @returns: </xsl:text><xsl:if test="@returns=''">(void)</xsl:if><xsl:apply-templates select="//*[@id=$returns]" mode="typename"/>
        '''<xsl:for-each select="./Argument"><xsl:variable name="argname"><xsl:choose><xsl:when test="@name"><xsl:value-of select="@name"/></xsl:when><xsl:otherwise>arg<xsl:value-of select="position()"/></xsl:otherwise></xsl:choose></xsl:variable><xsl:variable name="typeid" select="@type"/><xsl:text>
        assert(isinstance(</xsl:text><xsl:value-of select="$argname"/>,<xsl:apply-templates select="//*[@id=$typeid]" mode="typename"><xsl:with-param name="scopeid" select="$scopeid"/></xsl:apply-templates>))</xsl:for-each><xsl:text>
        </xsl:text><xsl:if test="$returns!=''">return<xsl:text> </xsl:text></xsl:if><xsl:if test="//ReferenceType[@id=$returns]|//Class[@id=$returns]|//Struct[@id=$returns]"><xsl:apply-templates select="//Class[@id=$returns]|//Struct[@id=$returns]" mode="typename"><xsl:with-param name="scopeid" select="$scopeid"/></xsl:apply-templates>(</xsl:if>native.<xsl:value-of select="$namespace_prefix"/><xsl:value-of select="$typename"/>__<xsl:value-of select="$realname"/>(<xsl:for-each select="./Argument"><xsl:apply-templates select="." mode="gen_callable"/>,</xsl:for-each>)<xsl:if test="//ReferenceType[@id=$returns]|//Class[@id=$returns]|//Struct[@id=$returns]">)</xsl:if>
        <xsl:for-each select="./Argument"><xsl:variable name="typeid" select="@type"/><xsl:apply-templates select="//*[@id=$typeid]" mode="gen_imports"><xsl:with-param name="scopeid" select="$scopeid"></xsl:with-param></xsl:apply-templates>
        </xsl:for-each> 
        <xsl:apply-templates select=".//Argument" mode="gen_meta_data"><xsl:with-param name="scopeid" select="$scopeid"/></xsl:apply-templates>
    </xsl:template>
    
    <xsl:template match="Class" mode="typedefinition">
        <xsl:param name="scopeid"/>
         <xsl:variable name="id" select="@id"/>
        <xsl:variable name="typename"><xsl:apply-templates select="." mode="typename"><xsl:with-param name="scopeid" select="$scopeid"/></xsl:apply-templates></xsl:variable>
        <xsl:apply-templates select="." mode="generate_definitions"><xsl:with-param name="scopeid" select="$scopeid"/></xsl:apply-templates><xsl:text>        

class </xsl:text><xsl:value-of select="$typename"/>( _FwdDecl_<xsl:value-of select="$typename"/> ):<xsl:apply-templates select="//Constructor[@context=$id and @access='public']" mode="generate_def"><xsl:with-param name="scopeid" select="$scopeid"></xsl:with-param></xsl:apply-templates>    <xsl:text>
    def __init__(self,*args):
        &quot;&quot;&quot;</xsl:text><xsl:for-each select="//Constructor[@context=$id and @access='public']"><xsl:text>
        Signature[</xsl:text><xsl:value-of select="position()"/>]: <xsl:if test="count(./Argument)=0"><xsl:text>
           No arguments</xsl:text></xsl:if><xsl:for-each select="./Argument"><xsl:variable name="argname">args[<xsl:value-of select="position()-1"/>]</xsl:variable><xsl:variable name="argtype" select="@type"/><xsl:text>
           </xsl:text><xsl:value-of select="$argname"/>:<xsl:apply-templates select="//*[@id=$argtype]" mode="typename"/><xsl:text>            
        </xsl:text></xsl:for-each></xsl:for-each><xsl:text>&quot;&quot;&quot;
        self._cobject = None
        if len(*args) == 1 and isinstance(args[0], POINTER(</xsl:text><xsl:value-of select="$typename"/>)<xsl:text>):
            self._cobject = native.</xsl:text><xsl:value-of select="$typename"/><xsl:text>_copy(args[0])
        else:
            </xsl:text><xsl:if test="count(//Constructor[@context=$id and @access='public'])=0">self._cobject = None#No public constructor</xsl:if><xsl:for-each select="//Constructor[@context=$id and @access='public']"><xsl:text>
            try:
                self._cobject = native.</xsl:text><xsl:value-of select="@name"/>_new<xsl:value-of select="position()"/>(*args)<xsl:text>
            except:
                pass
        </xsl:text></xsl:for-each><xsl:text>
        if self._cobject is None:
            raise Exception("No signature found to create object")

    def copy(self):
        return </xsl:text><xsl:value-of select="$typename"/>(native.<xsl:value-of select="$typename"/>_copy( self._cobject ))<xsl:text>

    def __del__(self):
        native.</xsl:text><xsl:value-of select="$typename"/>_delete(self._cobject)
        self._cobjet = None<xsl:text>
            
    </xsl:text><xsl:apply-templates select="//Method[@context=$id]|//OperatorMethod[@context=$id]" mode="typedefinition"><xsl:with-param name="typename" select="$typename"/><xsl:with-param name="scopeid" select="$scopeid"/></xsl:apply-templates>
    </xsl:template>
 
    <xsl:template match="FunctionType" mode="gen_meta_data">
        <xsl:param name="pos"/>
        <xsl:param name="incontext"/>
        <xsl:param name="scopeid"/>
        <xsl:variable name="id" select="@id"/>
        <xsl:variable name="context"><xsl:value-of select="//Typedef[@type=$id]/@context"/></xsl:variable>
          <xsl:variable name="return" select="@returns"/>
        <xsl:variable name="returntype"><xsl:apply-templates select="//*[@id=$return]" mode="typename">
            <xsl:with-param name="pos" select="$pos"/>
        </xsl:apply-templates></xsl:variable><xsl:text>
#@-add_functiontype(&quot;&quot;&quot;func</xsl:text><xsl:value-of select="$id"/> = CFUNCTYPE( <xsl:value-of select="$returntype"/>, <xsl:for-each select="./Argument"><xsl:variable name="argtype" select="@type"/>
#-    <xsl:apply-templates select="//*[@id=$argtype]" mode="typename"><xsl:with-param name="scopeid" select="$scopeid"/><xsl:with-param name="for_functype">1</xsl:with-param><xsl:with-param name="pos" select="$pos"/></xsl:apply-templates>, </xsl:for-each>)&quot;&quot;&quot;)    
    </xsl:template>   
    
    <xsl:template match="Enumeration" mode="typedefinition"><xsl:text>>

class</xsl:text><xsl:value-of select="translate(@name,'&lt;&gt;,: ','_____')"/>(object):
<xsl:for-each select="./EnumValue"><xsl:text>
    </xsl:text><xsl:value-of select="@name"/> = <xsl:value-of select="@init"/>
</xsl:for-each>
    </xsl:template>
    
    <xsl:template match="//Typedef" mode="typedefinition">
        <xsl:param name="scopeid"/>
        <xsl:param name="pos"/>
        <xsl:variable name="typeid" select="@type"/>
        <xsl:variable name="ptrtypeid" select="//PointerType[@id=$typeid]/@type"/>
        <xsl:variable name="typename"><xsl:apply-templates select="//*[@id=$typeid]" mode="typename"><xsl:with-param name="scopeid" select="$scopeid"/></xsl:apply-templates></xsl:variable>          
        <xsl:choose><xsl:when test="count(//FunctionType[@id=$typeid])>0 or count(//FunctionType[@id=$ptrtypeid])>0"><xsl:text>
</xsl:text>#@add_functypedef(&quot;<xsl:value-of select="@name"/>&quot;, &quot;<xsl:value-of select="$typename"/>&quot;)<xsl:text>
</xsl:text><xsl:variable name="returntype"><xsl:choose><xsl:when test="@returns"><xsl:value-of select="@returns"/></xsl:when>
<xsl:otherwise>void</xsl:otherwise></xsl:choose></xsl:variable>#@-add_functiontype(&quot;&quot;&quot;func<xsl:value-of select="//FunctionType[@id=$typeid or @id=$ptrtypeid]/@id"/> = CFUNCTYPE( <xsl:value-of select="$returntype"/>, <xsl:for-each select="//FunctionType[@id=$scopeid or @id=$ptrtypeid]/Argument"><xsl:variable name="argtype" select="@type"/><xsl:text>
</xsl:text>#-    <xsl:apply-templates select="//*[@id=$argtype]" mode="typename"><xsl:with-param name="for_functype">1</xsl:with-param><xsl:with-param name="scopeid" select="$scopeid"/><xsl:with-param name="pos" select="$pos"/></xsl:apply-templates>, </xsl:for-each>)&quot;&quot;&quot;)    
</xsl:when><xsl:when test="@context!=$scopeid or count(//FundamentalType[@id=$typeid])>0"><xsl:text>
</xsl:text><xsl:if test="@context=$scopeid">#@add_typedef(&quot;<xsl:value-of select="@name"/>&quot;, &quot;<xsl:value-of select="$typename"/>&quot;)</xsl:if></xsl:when> <xsl:otherwise><xsl:text>
#typedef
</xsl:text><xsl:value-of select="@name"/> = <xsl:value-of select="$typename"/></xsl:otherwise></xsl:choose>
<xsl:apply-templates select="//*[@id=$typeid]" mode="gen_meta_data"><xsl:with-param name="scopeid" select="$scopeid"/></xsl:apply-templates>
    </xsl:template>
  
    <!-- The workhorse to generate the definitions -->
    <xsl:template match="//*" mode="generate_definitions">  
        <xsl:param name="scopeid"/>
        <xsl:variable name="myid" select="@id"/>
        <xsl:for-each select="//Union[@context=$myid]|//Typedef[@context=$myid]|//Class[@context=$myid]|//Struct[@context=$myid]|//Union[@context=$myid]"><xsl:variable name='type' select='@type'/>
            <xsl:variable name="fileid" select="@file"/>
            <xsl:variable name="location" select="//File[@id=$fileid]/@name"/><xsl:if test="not(starts-with($location,'//'))"><xsl:apply-templates mode="typedefinition" select=".">
            <xsl:with-param name="scopeid" select="$scopeid"/>
</xsl:apply-templates></xsl:if></xsl:for-each>
    </xsl:template>
    
    <!-- root processing node -->
    <xsl:template match="/GCC_XML">
        <xsl:for-each select="Namespace"><xsl:variable name="scopeid" select="@id"/>
            <xsl:text>
====</xsl:text><xsl:apply-templates select="." mode="get_namespace"></xsl:apply-templates><xsl:text>
from ctypes import *
void = None

uint16_t = c_uint16
uint32_t = c_uint32
int32_t = c_int32
size_t = c_size_t
int64_t = c_int64
uint64_t = c_uint64
uintptr_t = POINTER(c_uint)
intptr_t = POINTER(c_int)

try:
    global native
    native = CDLL(&quot;</xsl:text><xsl:value-of select="$libname"/><xsl:text>&quot;)
except:
    native = None
    
def init( libname ):
    global native
    native = CDLL(libname)
</xsl:text><xsl:for-each select="//Class[@context=$scopeid]|//Struct[@context=$scopeid]"><xsl:variable name="typename"><xsl:apply-templates select="." mode="typename"><xsl:with-param name="scopeid" select="$scopeid"/></xsl:apply-templates></xsl:variable><xsl:text>
class _FwdDecl_</xsl:text><xsl:value-of select="$typename"/>(Structure):<xsl:text>
    _fields_=[]</xsl:text></xsl:for-each><xsl:text>
#!!
</xsl:text><xsl:apply-templates select="." mode="generate_definitions"><xsl:with-param name="scopeid" select="$scopeid"></xsl:with-param></xsl:apply-templates>
        </xsl:for-each>
    </xsl:template>
</xsl:stylesheet>