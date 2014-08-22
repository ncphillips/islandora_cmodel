<?php
/**
 * @file
 */

/**
 * islandora_object_parent_model
 *
 * @param $object_id
 * @return String|null
 * The PID of the object's parent.
 */
function islandora_object_parent_model($object_id){
  if(is_object($object_id)){
    $object = $object_id;
  }
  else{
    $object = islandora_object_load($object_id);
  }

  if($object){
    $object->relationships->registerNamespace('islandora-model', ISLANDORA_MODEL_URI);
    $parents = $object->relationships->get(ISLANDORA_MODEL_URI, 'hasParentModel');

    if (empty($parents)){
      /** If an islandora-model:hasParentModel relationship is not defined
       * for this object, then it will create that relationship to the first
       * object found in it's `models` property.
       *
       * This might be a little bold, and may cause some slowdown issues at times,
       * but oh well.
       */
      $parent = $object->models[0];
      $object->relationships->add(ISLANDORA_MODEL_URI, 'hasParentModel', $parent);
      return $parent;
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
  $parent = islandora_object_parent_model($object_id);
  return islandora_cmodel_supertypes($parent);

}
