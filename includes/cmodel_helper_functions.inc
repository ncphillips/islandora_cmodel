<?php
/**
 * @file cmodel_functions.php
 *
 * Provides a number of helper functions for when working with CModel objects.
 * The goal is to eventually implement these functions as methods in a Class.
 */

module_load_include('inc', 'islandora_related_content', 'includes/query_helper_functions');

/**
 * islandora_object_parent_model
 *
 * @param $object_id
 * @return String|null
 * The PID of the object's parent.
 */
function islandora_object_parent_model($object_id){
  $object = islandora_object_load($object_id);
  if($object){
    $object->relationships->registerNamespace('islandora-model', ISLANDORA_MODEL_URI);
    $parents = $object->relationships->get(ISLANDORA_MODEL_URI, 'hasParentModel');

    if (empty($parents)){
      return $object->models[0];
    }

    return $parents[0]['object']['value'];
  }

  return null;
}

/**
 * islandora_object_models
 *
 * @param $object_id
 *
 * @return array
 * The object's models array, minus the FedoraObject cmodel.
 */
function islandora_object_models($object_id){
  $object = islandora_object_load($object_id);

  return array_diff($object->models, array('fedora-system:FedoraObject-3.0'));
}


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
  if ($cmodel && !in_array('fedora-system:ContentModel-3.0', $cmodel->models)) {
    /** @todo Produce an error message explaining why this function failed. */
    return NULL;
  }
  return $cmodel;
}


// Relationships
/**
 * islandora_cmodel_add_relationship
 *
 * Adds a relationship to the given CModel.
 *
 * Takes a subject (either a AbstractObject or a FedoraDatastream), as well as
 * the parameters for FedoraRelsExt::add() and adds the represented relationship
 * if it is towards a CModel, and does't already exist.
 *
 * @see FedoraRelsExt::add()
 */
function islandora_cmodel_add_relationship($sub, $pred_uri, $pred, $object, $type) {
  module_load_include('inc', 'islandora', 'islandora.rules');
  $sub_is_cmodel    = islandora_cmodel_load($sub);
  $object_is_cmodel = islandora_cmodel_load($object);
  $rel_exists       = islandora_object_has_relationship($sub, $pred_uri, $pred, $object, $type);

  if ($sub_is_cmodel and $object_is_cmodel and !$rel_exists) {
    islandora_object_add_relationship($sub, $pred_uri, $pred, $object, $type);
    return TRUE;
  }
  /** @todo Produce error messages depending on why this function failed. */
  return FALSE;
}

/**
 * islandora_cmodel_remove_relationship
 *
 * Removes a relationship to the given CModel.
 *
 * Takes a subject (either a AbstractObject or a FedoraDatastream), as well as
 * the parameters for FedoraRelsExt::remove(), and removes the represented
 * relationship if it is towards a CModel, and it exists.
 *
 * @see FedoraRelsExt::remove()
 */
function islandora_cmodel_remove_relationship($sub, $pred_uri, $pred, $object, $type) {
  $sub_is_cmodel    = islandora_cmodel_load($sub);
  $object_is_cmodel = islandora_cmodel_load($object);
  $rel_exists = islandora_object_has_relationship($sub, $pred_uri, $pred, $object, $type);

  if ($sub_is_cmodel and $object_is_cmodel and $rel_exists) {
    islandora_object_remove_relationship($sub, $pred_uri, $pred, $object, $type);
    return TRUE;
  }
  /** @todo Produce error messages depending on why this function failed. */
  return FALSE;
}

/**
 * islandora_cmodel_relationships_towards
 *
 * @param $cmodel_id
 *
 * @return array
 *   This returns an indexed array with all CModel relationships towards this
 *   object. The array is of the form:
 *   @code
 *   Array
 *   (
 *       [0] => Array
 *           (
 *               [predicate] => Array
 *                   (
 *                       [value] => thepredicate
 *                       [alias] => thexmlprefix
 *                       [namespace] => http://crazycool.com#
 *                   )
 *
 *               [object] => Array
 *                   (
 *                       [literal] => TRUE
 *                       [value] => test
 *                   )
 *
 *           )
 *
 *   )
 *   @endcode
 */
function islandora_cmodel_relationships_towards($cmodel_id) {
  $cmodel_ids = array($cmodel_id);
  $cmodel_ids = array_merge($cmodel_ids, islandora_cmodel_supertypes($cmodel_id));

  // Construct the CModel tripples
  $cm_query_array = array();
  foreach($cmodel_ids as $id){
    $cm_query_array[] = '{' . "?id ?relationship <info:fedora/$id>" . '}';
  }
  $cm_query = implode(' UNION ', $cm_query_array);

  // Construct Query
  $query = "
    PREFIX fedora-model: <info:fedora/fedora-system:def/model#>
    SELECT *
    FROM <#ri>
    WHERE
    {
      {?id fedora-model:hasModel <info:fedora/fedora-system:ContentModel-3.0>}
      {
        $cm_query
      }
      FILTER(!REGEX(STR(?relationship), 'Subtype'))
      FILTER(!REGEX(STR(?relationship), 'Model'))
    }";

  // Connect to Tuque and perform the query.
  $tuque = new IslandoraTuque();
  $query_results= $tuque->repository->ri->sparqlQuery($query);

  // Regex Patterns
  $predicate_value_pattern  = '/^.*#(.*)$/';
  $alias_pattern            = '/^(.*):.*$/';
  $namespace_pattern        = '/^(.*#).*$/';

  // Transform the SPARQL results into the FedoraRelationship::get() format.
  $results = array();
  foreach($query_results as $qr){
    preg_match($predicate_value_pattern, $qr['relationship']['value'], $pred);
    preg_match($alias_pattern,           $qr['relationship']['value'], $alias);
    preg_match($namespace_pattern,       $qr['relationship']['uri'],   $namespace);
    $results[] = array(
      'predicate' => array(
        'value' => $pred[1],
        'alias' => $alias[1],
        'namespace' => $namespace[1],
      ),
      'object' => array(
        'literal' => false,
        'value' => $qr['id']['value'],
      ),
    );
  }

  return $results;
}

function islandora_cmodel_relationships_away($cmodel_id){
  $cmodel_ids = array($cmodel_id);
  $cmodel_ids = array_merge($cmodel_ids, islandora_cmodel_supertypes($cmodel_id));

  // Construct the CModel tripples
  $cm_query_array = array();
  foreach($cmodel_ids as $id){
    $cm_query_array[] = '{' . "<info:fedora/$id> ?relationship ?id " . '}';
  }
  $cm_query = implode(' UNION ', $cm_query_array);

  // Construct Query
  $query = "
    PREFIX fedora-model: <info:fedora/fedora-system:def/model#>
    SELECT *
    FROM <#ri>
    WHERE
    {
      {?id fedora-model:hasModel <info:fedora/fedora-system:ContentModel-3.0>}
      {
        $cm_query
      }
      FILTER(!REGEX(STR(?relationship), 'Subtype'))
      FILTER(!REGEX(STR(?relationship), 'Model'))
    }";


  // Connect to Tuque and perform the query.
  $tuque = new IslandoraTuque();
  $query_results= $tuque->repository->ri->sparqlQuery($query);

  // Regex Patterns
  $predicate_value_pattern  = '/^.*#(.*)$/';
  $alias_pattern            = '/^(.*):.*$/';
  $namespace_pattern        = '/^(.*#).*$/';

  // Transform the SPARQL results into the FedoraRelationship::get() format.
  $results = array();
  foreach($query_results as $qr){
    preg_match($predicate_value_pattern, $qr['relationship']['value'], $pred);
    preg_match($alias_pattern,           $qr['relationship']['value'], $alias);
    preg_match($namespace_pattern,       $qr['relationship']['uri'],   $namespace);
    $results[] = array(
      'predicate' => array(
        'value' => $pred[1],
        'alias' => $alias[1],
        'namespace' => $namespace[1],
      ),
      'object' => array(
        'literal' => false,
        'value' => $qr['id']['value'],
      ),
    );
  }

  return $results;
}


// Polymorphism
/**
 * islandora_cmodel_parent
 *
 * @param $cmodel_id
 *
 * @return array
 * Contains the PIDs that are parents of $cmodel_id. This array should always
 * have only one PID.
 */
function islandora_cmodel_parent($cmodel_id) {
  $cmodel = islandora_cmodel_load($cmodel_id);
  $parents = array(); // This should be an array of length 1.

  if($cmodel){
    $object->relationships->registerNamespace('islandora-model', ISLANDORA_MODEL_URI);
    $rels = $cmodel->relationships->get(ISLANDORA_MODEL_URI,  'hasParentModel');

    foreach($rels as $rel)
      $parents[] = $rel['object']['value'];
  }
  return $parents;
}

/**
 * islandora_cmodel_supertypes
 * @param $cmodel_id
 *
 * @return array
 * Contains the PIDs of all CModels that $cmodel_id subtypes from.
 */
function islandora_cmodel_supertypes($cmodel_id) {
  $cmodel = islandora_cmodel_load($cmodel_id);
  $supertypes = array();

  if ($cmodel) {
    $cmodel->relationships->registerNamespace('islandora-model', ISLANDORA_MODEL_URI);
    $rels = $cmodel->relationships->get(ISLANDORA_MODEL_URI, 'isSubtypeOf');

    $supertypes = array();
    foreach($rels as $rel){
      $supertypes[] = $rel['object']['value'];
    }
  }
  return $supertypes;
}

/**
 * islandora_cmodel_subtypes
 *
 * @param $cmodel_id
 *
 * @return array|null
 * An array of object PIDs that
 */
function islandora_cmodel_subtypes($cmodel_id){
  $cmodel = islandora_cmodel_load($cmodel_id);

  if($cmodel){
    $ro_args = array(
      'object' => $cmodel_id,
      'cmodels' => array(),
      'relationships' => array('isSubtypeOf'),
    );

    $query_results = related_islandora_objects($ro_args)['ids'];

    $subtypes = array();
    foreach($query_results as $subtype_id){
      $subtypes[] = $subtype_id;
    }
    return $subtypes;
  }
  else return null;
}

/**
 * islandora_cmodel_create
 *
 * @param        $cmodel_id
 * @param        $label
 * @param string $dscm_path
 * @param null   $parent_id
 * @param string $owner
 *
 * @return AbstractObject|null
 *
 * Creates a new CModel.
 */
function islandora_cmodel_create($cmodel_id, $label, $dscm_path = '', $parent_id = NULL, $owner = 'fedoraAdmin') {

  $tuque = new IslandoraTuque();

  // If a parent ID is given, but the Fedora Object doesn't exist, or
  // is not a CModel, return NULL.
  if ($parent_id && is_null(islandora_cmodel_load($parent_id))) {
    /** @todo Throw an error message. */
    return NULL;
  }

  // If the ID isn't being used by some Fedora Object.
  // Create Content Model
  $cmodel = $tuque->repository->constructObject($cmodel_id);
  $cmodel->owner = $owner;
  $cmodel->label = $label;
  $cmodel->models = 'fedora-system:ContentModel-3.0';

  if ($dscm_path){
    // DS-COMPOSITE-MODEL Datastream.
    $dscm = $cmodel->constructDatastream('DS-COMPOSITE-MODEL', 'X');
    $dscm->label = 'Datastreams';
    $dscm->mimetype = 'text/xml';
    $dscm->setContentFromFile($dscm_path, FALSE);
    $cmodel->ingestDatastream($dscm);
  }
  else {
    /** @todo Set up a skeleton dscm. */
  }

  // Inherit from parent
  if ($parent_id) {
    // Set the Parent
    $cmodel->relationships->registerNamespace('islandora-model', ISLANDORA_MODEL_URI);
    $cmodel->relationships->add(ISLANDORA_MODEL_URI, 'hasParentModel', $parent_id);

    // Set the supertypes.
    $supertypes = islandora_cmodel_supertypes($parent_id);
    $supertypes[] = $parent_id;

    foreach($supertypes as $supertype){
      $cmodel->relationships->add(ISLANDORA_MODEL_URI, 'isSubtypeOf', $supertype);
    }
  }

  return $cmodel;

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




