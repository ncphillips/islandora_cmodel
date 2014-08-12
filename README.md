1. Solr Indexing
2. Islandora CModel Concepts

# Solr Indexing

In order for the Islandora CModel module to function properly, a couple changes must be made to the existing Solr transforms.

These Solr transforms are located in a subdirectory of the `fedoragsearch` folder. Depending on what system is being
used, it may either be located at:

    /usr/local/fedora/tomcat6/webapps/

or:

    /var/lib/tomcat6/webapps/fedoragsearch

From here, move into the FgsIndex directory:

    $ cd WEB-INF/classes/fgsconfigFinal/index/FgsIndex

Commands in the next two sections assume you are starting from this directory.

### CModel Indexing
This module requires CModel indexing in Solr to be turned on.

    $ vim foxmlToSolr.xslt


Aroud line 96 you will see the following line:

    <xsl:if test="not(foxml:digitalObject/foxml:datastream[@ID='METHODMAP'
    or @ID='DS-COMPOSITE-MODEL'])">

remove the second half of the 'or' so it looks like this:

    <xsl:if test="not(foxml:digitalObject/foxml:datastream[@ID='METHODMAP'])">

### Modify RELS-EXT_to_solr.xslt

    $ vim islandora_transforms/RELS-EXT_to_solr.xlst

Around line 14 you will see:

    <xsl:for-each select="$content//rdf:Description/*[@rdf:resource]
        | $content//rdf:description/*[@rdf:resource]">
      <field>
        <xsl:attribute name="name">
          <xsl:value-of select="concat($prefix, local-name(), '_uri', $suffix)"/>
        </xsl:attribute>
        <xsl:value-of select="@rdf:resource"/>
      </field>
    </xsl:for-each>

Add an extra field to this tag so it looks like:

    <xsl:for-each select="$content//rdf:Description/*[@rdf:resource]
        | $content//rdf:description/*[@rdf:resource]">
      <field>
        <xsl:attribute name="name">
          <xsl:value-of select="concat($prefix, local-name(), '_uri', $suffix)"/>
        </xsl:attribute>
        <xsl:value-of select="@rdf:resource"/>
      </field>
      <field>
        <xsl:attribute name="name">
          <xsl:value-of select="concat($prefix, substring-before(name(), ':'),
            '_', local-name(), '_uri', $suffix)"/>
        </xsl:attribute>
        <xsl:value-of select="@rdf:resource"/>
      </field>
    </xsl:for-each>



# Islandora CModel Concepts

This module introduces two concepts two Islandora CModel's:

  1. Subtypes
  2. CModle RELS-EXT Relationships

## Subtypes

A **subtype** is a CModel that is related to another CModel (the supertype) by the notion of substitutability.

Subtyping in Islandora is **width based**, meaning subtypes only add to their supertypes, they do not remove or overload existing structure and functionality. This subtype relationship must be explicitly defined. Two CModels which are structured or function in similar ways are not necessarily related types.

Throughout this text we will use the symbol `<:` to denote the phrase "is a subtype of", and `</:` to denote the phrase "is NOT a subtype of".

##### Properties of Subtypes

1. **Transitive:**

 	if ( a <: b ) and ( b <: c ) then ( a <: c )

2. **Not symmetric:**

	if ( a <: b ) then ( b </: a )

3. **Non-Circular:**

	if ( a <: b ) and ( a <: c ) then either ( b <: c ) or ( c <: b), but not both.


#### RDF Relationships
Let A, B, C, and D be CModels such that:

	B <: A
	C <: A
	D <: C

We will use two RDF descriptors for subtypes in CModels: `islandora-model:hasParentModel`, and `islandora-model:isSubtypeOf`. With the CModels described at the start of this section

A's relationships:

	A fedora-model:hasModel ContentModel-3.0

B's relationships:

	B fedora-model:hasModel ContentModel-3.0
	B islandora-model:hasParentModel A
	B isSubtypeOf A

C's relationships:

	C fedora-model:hasModel ContentModel-3.0
	C islandora-model:hasParentModel A
	C islandora-model:isSubtypeOf A

D's relationships:

	D fedora-model:hasModel ContentModel-3.0
	D islandora-model:hasParentModel C
	D islandora-model:isSubtypeOf A
	D islandora-model:isSubtypeOf C

##### Sets

For each CModel there are two sets, one for each of the RDF relationships. These sets are `parentModel` and `supertypes`.

	sizeof( D.parent) <= 1

	D.supertypes == D.parent UNION D.parent.supertypes


### Subtype Instances
When creating an instance of a CModel, the resulting object will have a `fedora-model:hasModel` relationship to the CModel, as well as all of that CModel's supertypes.

For example, let D_Inst be an instance of the D CModel, it will have the following relationships:

	D_Inst	fedora-model:hasModel	D
	D_Inst	fedora-model:hasModel	C
	D_Inst	fedora-model:hasModel	A

### Subtype Inheritence

Let ( A <: B )

Data objects of A also have the `fedora-model:hasModel` relationship towards B. As a result any instance of A will implicitly inherit the following features form B:

 1. Datastreams
 2. Workflows
 3. CModel Relationships

Future plans include making the following inheritable:

 1. Form Builder Forms
 2. Ingest steps

Instances of a subtype will show up in the results when searching for it's supertype, but not vice versa.

## CModel RELS-EXT
Relationships defined in the RELS-EXT of a CModel describe how data of different content types relate to one another.

For example, let A and B be CModels such that

	A fedora:isPartOf B

We can assume that an instance of A will have a `fedora:isPartOf` relationship to some instance(s) of B. The reverse relationship is not expected, unless explicitly defined.

#### fedora-model & islandora-model

Relationships in the fedora-model and islandora-model namespaces of CModels do not indicate relationships between data objects.

#### Future Plans

It would be useful if, in the future, we had some method of defining the cardinality for these relationships.



