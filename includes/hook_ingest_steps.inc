<?php

/**
 * @file cmodel_ingest_steps.inc
 *
 *       This file provides this modules implementations of hook_islandora_ingest_steps()
 */

/**
 * Implements hook_islandora_ingest_steps()
 * @return array
 */
function islandora_cmodel_islandora_ingest_steps(){
  $ingest_steps = array();

  $ingest_steps['set_parent_relationship'] = array(
    'type' => 'callback',
    'weight' => 2,
    'do_function' => array(
      'function' => 'set_parent_relationships',
      'args' => array('object'),
    ),
    'undo_function' => array(
      'function' => 'unset_parent_relationships',
      'args' => array('object'),
    ),
  );

  return $ingest_steps;
}

/**
 * set_parent_relationships
 *
 * @param $args
 */
function set_parent_relationships($args){
  // New Object
  $object = $args['build_info']['args'][0]['object'];

  // New object has parents
  $has_parent = $object->relationships->get(ISLANDORA_RELS_EXT_URI, 'temp');

  if($has_parent){
    // Remove the temp relationship
    $object->relationships->remove(ISLANDORA_RELS_EXT_URI, 'temp');

    // Get the ID of the Parent object
    $parent_id = $has_parent[0]['object']['value'];

    // Get all of the Parent object's CModels.
    $parent_cmodels = islandora_object_models($parent_id);

    // Get the New Object's CModel.
    $cmodel_id = $args['build_info']['args'][0]['models'];

    if(sizeof($cmodel_id) > 1) {
      $cmodel_id = $args['islandora']['step_storage']['xml_form_builder_association_step']['association']['content_model'];
    }
    else{
      $cmodel_id = $cmodel_id[0];
    }

    // Get the NO CModel's CMRELS
    $cmodel_rels = islandora_cmodel_relationships_away($cmodel_id);

    // For each of those CMRELS
    foreach($cmodel_rels as $rel){
      // If {the object of the relationship} INTERSECT {PARENT CMODELS} is a
      // non empty set, then create the relationship.
      if (array_intersect($parent_cmodels, array($rel['object']['value']))){
        $ns = $rel['predicate']['namespace'];
        $predicate = $rel['predicate']['value'];
        $object->relationships->add($ns, $predicate, $parent_id);
      }
    }
  }
}

/**
 * unset_parent_relationships
 * @param $args
 */
function unset_parent_relationships($args){
  /** @todo */
}
