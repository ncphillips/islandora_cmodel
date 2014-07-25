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

  $ingest_steps['islandora_cmodel_set_parent_relationship'] = array(
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

  $ingest_steps['islandora_cmodel_select_cmodel'] = array(
    'type' => 'form',
    'weight' => -1,
    'form_id' => 'select_cmodel_form',
  );

  return $ingest_steps;
}



/**
 * set_parent_relationships
 *
 * @param $args
 */
function set_parent_relationships($args){
  $object = $args['islandora']['objects'][0];

  $parent = true ? $args['islandora']['shared_storage']['parent'] : null ;

  if($parent){
    $parent_cmodels = islandora_object_models($parent);
    $cmodel_id = $args['islandora']['shared_storage']['models'][0];

    // Get the NO CModel's CMRELS
    $cmodel_rels = islandora_cmodel_relationships_away($cmodel_id);

    // For each of those CMRELS
    foreach($cmodel_rels as $rel){
      // If {the object of the relationship} INTERSECT {PARENT CMODELS} is a
      // non empty set, then create the relationship.
      if (array_intersect($parent_cmodels, array($rel['object']['value']))){
        $ns = $rel['predicate']['namespace'];
        $predicate = $rel['predicate']['value'];
        $object->relationships->add($ns, $predicate, $parent);
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


/**
 * Implements hook_islandora_ingest_steps_alter
 *
 * This implementation does two things.
 *
 * 1. Adds all the subtypes of each model listed.
 * 2. If there is NOT more than one model the Select CModel step is removed.
 */
function islandora_cmodel_islandora_ingest_steps_alter(array &$steps, array &$form_state) {
  if($form_state['islandora']['step_id'] != 'islandora_cmodel_select_cmodel'){
    $models = $form_state['islandora']['shared_storage']['models'];

    foreach($models as $model){
      $models = array_merge($models, islandora_cmodel_subtypes($model));
    }

    if (sizeof($models) < 2){
      unset($steps['islandora_cmodel_select_cmodel']);
    }

    $form_state['islandora']['shared_storage']['models'] = $models;
  }
}