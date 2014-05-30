<?php

/**
 * @file cmodel_functions.php
 *
 * Provides a number of helper functions for when working with CModel objects.
 */


define('CMODEL_SUFFIX', '_cmodel');
define('REL_TO', 'to');
define('REL_FROM', 'from');
define('REL', 'any');

/**
 * Loads an Islandora Object if it is a CModel.
 *
 * @param String $cmodel_id
 * This is a partial ID of an Islandora Content Model.
 *
 * @return FedoraObject $object | null
 * If a FedoraObject is found, and it is a Content Model, return it;
 * else, return NULL.
 *
 */
function islandora_cmodel_load($cmodel_id) {
  /**
   * Load the object from the pid and then check to make sure it is
   * actually a CModel.
   */
  $cmodel = islandora_object_load($cmodel_id);
  if (!in_array('fedora-system:ContentModel-3.0', $cmodel->models)) {
    /** @todo Produce an error message explaining why this function failed. */
    return NULL;
  }
  return $cmodel;
}

/**
 * Loads an Islandora Object if it is a CModel and it is related to the
 * object passed in as the first parameter.
 */
function islandora_related_cmodel_load($cmodel_id) {
  /**
   * @todo islandora_related_cmodel_load
   */
}

// Relationships
/**
 * Add a relationship to the given CModel.
 *
 * Takes a subject (either a AbstractObject or a FedoraDatastream), as well as
 * the parameters for FedoraRelsExt::add() or FedoraRelsInt::add(), and adds
 * the represented relationship.
 *
 * @see FedoraRelsExt::get()
 */
function islandora_cmodel_add_relationship($sub, $pred_uri, $pred, $object, $type) {
  $sub_is_cmodel    = islandora_cmodel_load($sub);
  $object_is_cmodel = islandora_cmodel_load($object);
  $rel_exists = islandora_object_has_relationship($sub, $pred_uri, $pred, $object, $type);

  // If they're both CModels, and the relationship has not already been  defined.
  if ($sub_is_cmodel and $object_is_cmodel and !$rel_exists) {
    // Define the relationship.
    islandora_object_add_relationship($sub, $pred_uri, $pred, $object, $type);
    return TRUE;
  }
  /** @todo Produce error messages depending on why this function failed. */
  return FALSE;
}

/**
 * Removes a relationship to the given CModel.
 *
 * Takes a subject (either a AbstractObject or a FedoraDatastream), as well as
 * the parameters for FedoraRelsExt::remove() or FedoraRelsInt::remove(),
 * and removes the represented relationship.
 */
function islandora_cmodel_remove_relationship($sub, $pred_uri, $pred, $object, $type) {
  $sub_is_cmodel    = islandora_cmodel_load($sub);
  $object_is_cmodel = islandora_cmodel_load($object);
  $rel_exists = islandora_object_has_relationship($sub, $pred_uri, $pred, $object, $type);

  // If they're both CModels and the relationship is defined between them.
  if ($sub_is_cmodel and $object_is_cmodel and $rel_exists) {
    // Remove the relationship.
    islandora_object_remove_relationship($sub, $pred_uri, $pred, $object, $type);
    return TRUE;
  }
  /** @todo Produce error messages depending on why this function failed. */
  return FALSE;
}

/**
 * Returns a list of relationships.
 */
function islandora_cmodel_relationships($cmodel_id, $target_id=NULL, $direction=NULL) {
  $relationships = array();

  $cm_ids[] = $cmodel_id;
  array_merge($cm_ids, islandora_cmodel_ancestors($cmodel_id));

  /** @todo Merge all relationship arrays into one. */
  foreach ($cm_ids as $cm_id) {
    $cm = islandora_cmodel_load($cm_id);
    drupal_array_merge_deep($relationships, $cm->relationships);

  }

  return $relationships;
}


// Polymorphism
/**
 * Returns a list of ancestors, from closest to oldest.
 */
function islandora_cmodel_supertypes($cmodel_id) {
  $cmodel = islandora_cmodel_load($cmodel_id);
  if ($cmodel) {
    $ancestors = array($cmodel);
    $ancestor_ids = array($cmodel_id);

    /** While there are still parent CModels. */
    while (sizeof($cmodel->models) > 2) {
      /** Retrieve the parent CModel. */
      $cmodel = islandora_object_load($cmodel->models[0]);

      /** Append the parent to the list of ancestors. */
      $ancestors[] = $cmodel;
      $ancestor_ids[] = $cmodel->id;
    }

    return $ancestor_ids;
  }
  /** @todo Throw error message. */
}


/**
 * Creates and returns a Fedora CModel.
 *
 * @param      $cmodel_id
 * @param      $owner
 * @param      $label
 * @param null $parent_id
 *
 * @return AbstractObject|null
 *
 * @todo Write docs for `islandora_cmodel_create`
 */
function islandora_cmodel_create($cmodel_id,
                                 $owner,
                                 $label,
                                 $parent_id=NULL){

  $tuque = new IslandoraTuque();

  $module_path = drupal_get_path('module', 'iacm');

  // If a parent ID is given, but the Fedora Object doesn't exist, or
  // is not a CModel, return NULL.
  if ($parent_id && is_null(islandora_cmodel_load($parent_id))) {
    /** @todo Throw an error message. */
    return NULL;
  }

  // If the ID isn't being used by some Fedora Object.
  if(is_null(islandora_object_load($cmodel_id))){
    // Create Content Model
    $cmodel = $tuque->repository->constructObject($cmodel_id);
    $cmodel->owner = $owner;
    $cmodel->label = $label;
    $cmodel->models = 'fedora-system:ContentModel-3.0';

    // DS-COMPOSITE-MODEL Datastream.
    $dscm = $cmodel->constructDatastream('DS-COMPOSITE-MODEL', 'X');
    $dscm->label = 'Datastreams';
    $dscm->mimetype = 'text/xml';
    $dscm->setContentFromFile("$module_path/xml/ds_composite_model.xml", FALSE);
    $cmodel->ingestDatastream($dscm);

    // Inherit from parent
    if ($parent_id)
      $cmodel->relationships->add(FEDORA_MODEL_URI, 'hasModel', $parent_id);

    return $cmodel;
  }

  return NULL;
}

/**
 * Adds a datastream entry to the DS-COMPOSITE-MODEL of this Fedora CModel.
 */
function islandora_cmodel_add_datastream($cmodel_id) {}

/**
 * Removes a datastream entry to the DS-COMPOSITE-MODEL of this Fedora CModel.
 */
function islandora_cmodel_remove_datastream($cmodel_id) {}

/**
 * Returns an array of datastream entries from this Fedora CModel.
 */
function islandora_cmodel_datastreams() {}




