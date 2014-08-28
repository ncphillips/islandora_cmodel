# Islandora CModel

Islandora CModel is a project which extends the concept of Fedora Content Models
in the context of an Islandora site, without the addition of new Datastreams.

The two core concepts that have been added, are that of Relationships, and Subtyping.

The concept of Content Model Relationships works simply by a assigning new semantic
meaning to the RELS-EXT of Content Model. The relationships defined in the RELS-EXT
are now understood to be descriptions of how instances of that Content Model
should be related to other objects in the repository.

The Subtyping concept allows Content Models to inherit properties from other
Models. Inheritable properties currently include views, relationships, datastreams,
and workflows, with plans to expand this scope to include ingest steps, and forms.

For more information on these concepts, and the way they are implemented, follow
the links below.

## Features
#### Current
 * [The CModel View](https://github.com/ncphillips/islandora_cmodel/wiki/CModel-View)
 * [CModel's RELS-EXT](https://github.com/ncphillips/islandora_cmodel/wiki/The-CModel-RELS-EXT)
 * [Cmodel Subtypes](https://github.com/ncphillips/islandora_cmodel/wiki/CModel-Subtypes)
 * [Cmodel Functions](https://github.com/ncphillips/islandora_cmodel/wiki/CModel-Functions)
 * [Object Functions](https://github.com/ncphillips/islandora_cmodel/wiki/Object-Functions)
 * [Select CModel Subtype Ingest Step](https://github.com/ncphillips/islandora_cmodel/wiki/Select-CModel-Ingest-Step)

#### Future
 * Creating CModel Subtypes

## Getting things running
 * [Solr Indexing](https://github.com/ncphillips/islandora_cmodel/wiki/Solr-Indexing)

