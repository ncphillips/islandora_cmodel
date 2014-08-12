# Islandora CModel


## Solr Indexing

Solr transforms are located in the the `fedoragsearch` folder. Depending on what system is being
used, it may either be located at:
    /usr/local/fedora/tomcat6/webapps/

or:

    /var/lib/tomcat6/webapps/fedoragsearch

From here,

    $ cd WEB-INF/classes/fgsconfigFinal/index/FgsIndex

### CModel Indexing
This module requires CModel indexing in Solr to be turned on.

    $ vim foxmlToSolr.xslt


Aroud line 96 you will see the following line:

    <xsl:if test="not(foxml:digitalObject/foxml:datastream[@ID='METHODMAP' or @ID='DS-COMPOSITE-MODEL'])">

remove the second half of the 'or' so it looks like this:

    <xsl:if test="not(foxml:digitalObject/foxml:datastream[@ID='METHODMAP'])">

### Modify RELS-EXT_to_solr.xslt

    $ vim islandora_transforms/RELS-EXT_to_solr.xlst

Around line 14 you will see:

    <xsl:for-each select="$content//rdf:Description/*[@rdf:resource] | $content//rdf:description/*[@rdf:resource]">
      <field>
        <xsl:attribute name="name">
          <xsl:value-of select="concat($prefix, local-name(), '_uri', $suffix)"/>
        </xsl:attribute>
        <xsl:value-of select="@rdf:resource"/>
      </field>
    </xsl:for-each>

Add an extra field to this tag so it looks like:

    <xsl:for-each select="$content//rdf:Description/*[@rdf:resource] | $content//rdf:description/*[@rdf:resource]">
      <field>
        <xsl:attribute name="name">
          <xsl:value-of select="concat($prefix, local-name(), '_uri', $suffix)"/>
        </xsl:attribute>
        <xsl:value-of select="@rdf:resource"/>
      </field>
      <field>
        <xsl:attribute name="name">
          <xsl:value-of select="concat($prefix, substring-before(name(), ':'), '_', local-name(), '_uri', $suffix)"/>
        </xsl:attribute>
        <xsl:value-of select="@rdf:resource"/>
      </field>
    </xsl:for-each>




