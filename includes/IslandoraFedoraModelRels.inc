<?php

class IslandoraFedoraModelRels extends FedoraRelationships {

  public function __construct(AbstractFedoraObject $object){
    $this->object = $object;

    $namespaces = array(
      'fedora' => FEDORA_RELS_EXT_URI,
      'fedora-model' => FEDORA_MODEL_URI,
      'islandora' => ISLANDORA_RELS_EXT_URI,
      'islandora-model' => ISLANDORA_MODEL_URI
    );

    parent::__construct($namespaces);
  }


  protected function initializeDatastream() {
    if ($this->datastream === NULL) {
      if (isset($this->object['RELS-EXT'])) {
        $ds = $this->object['RELS-EXT'];
      }
      else {
        $ds = $this->object->constructDatastream('RELS-EXT', INIT_DS_CONTROL_GROUP);
        $ds->label = INIT_DS_LABEL;
        $ds->format = INIT_DS_FORMAT;
        $ds->mimetype = INIT_DS_MIME;
        $this->new = TRUE;
      }

      $this->datastream = $ds;
    }
  }

  public function add($predicate_uri, $predicate, $object, $type = RELS_TYPE_URI) {
    $sub_is_cmodel    = islandora_cmodel_load($this->object->id);
    $object_is_cmodel = islandora_cmodel_load($object);
    $rel_exists       = islandora_object_has_relationship($this->object, $predicate_uri, $predicate, $object, $type);

    if ($sub_is_cmodel and $object_is_cmodel and !$rel_exists) {
      $this->initializeDatastream();
      parent::internalAdd($this->object->id, $predicate_uri, $predicate, $object, $type);
    }
  }


  public function remove($predicate_uri = NULL, $predicate = NULL, $object = NULL, $type = RELS_TYPE_URI) {
    $sub_is_cmodel    = islandora_cmodel_load($this->object->id);
    $object_is_cmodel = islandora_cmodel_load($object);
    $rel_exists       = islandora_object_has_relationship($this->object, $predicate_uri, $predicate, $object, $type);

    if ($sub_is_cmodel and $object_is_cmodel and !$rel_exists) {
      $this->initializeDatastream();
      $return = parent::internalRemove($this->object->id, $predicate_uri, $predicate, $object, $type);

      return $return;
    }
  }

  public function get($direction=NULL, $predicate_uri = NULL, $predicate = NULL, $object = NULL, $type = RELS_TYPE_URI) {
    // This function is going to be wayyy different

    $this->initializeDatastream();

    // This method once accepted only booleans.
    if ($type === TRUE) {
      $type = RELS_TYPE_PLAIN_LITERAL;
    }
    elseif ($type == FALSE) {
      $type = RELS_TYPE_URI;
    }

    return parent::internalGet($this->object->id, $predicate_uri, $predicate, $object, $type);
  }
}
