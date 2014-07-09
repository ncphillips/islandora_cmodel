<?php

module_load_include('inc', 'islandora', 'includes/tuque_wrapper');

define('IS_SUBJECT', 'sub');
define('IS_OBJECT', 'obj');


class IslandoraNewFedoraModel extends  IslandoraNewFedoraObject{

}


class IslandoraFedoraModel extends IslandoraFedoraObject {

  protected $cmrels = NULL;

  protected $dscm = NULL;

  public function __construct($id, FedoraRepository $repository){
    $this->cmrels = new IslandoraFedoraModelRels($this);
    parent::__construct($id, $repository);
  }

  protected function parent_modelMagicProperty($function, $parent_id) {
    switch($function){
      case 'get':
        $parents = array();

        $rels = $this->relationships->get(ISLANDORA_MODEL_URI, 'hasParentMode');

        foreach($rels as $rel) {
          $parents[] = $rel['object']['value'];
        }
        return $parents;
        break;

      case 'isset':
        return true;
        break;

      case 'set':
        $p = islandora_cmodel_load($parent_id);
        if($p){
          $this->relationships->remove(ISLANDORA_MODEL_URI, 'isSubtypeOf');
          $this->relationships->remove(ISLANDORA_MODEL_URI, 'hasParentModel');
          $this->relationships->add(ISLANDORA_MODEL_URI, 'hasParentModel', $parent_id);
          $supertypes = array_merge(array($parent_id), $p->getSupertypes());
          foreach($supertypes as $s){
            $this->relationships->add(ISLANDORA_MODEL_URI, 'isSubtypeOf', $s);
          }
        }
        break;

      case 'unset':
        break;


    }
  }

  protected function getSupertypes() {
    $supertypes = array();

    $rels = $this->relationships->get(ISLANDORA_MODEL_URI, 'isSubtypeOf');

    foreach($rels as $rel) {
      $supertypes[] = $rel['object']['value'];
    }
    return $supertypes;
  }

  protected function getSubtypes(){

  }

}
