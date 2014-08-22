<?php
/**
 * @file
 */

/**
 * Implements hook_preprocess_THEME()
 *
 * Where THEME = cmodel
 *
 * @see hook_preprocess_theme()
 * @see islandora_cmodel_page()
 */
function islandora_cmodel_preprocess_cmodel(array &$variables){
  $cmodel = islandora_cmodel_load($variables['islandora_object']);
  $variables['supertypes'] = islandora_cmodel_supertypes($cmodel);
  $variables['subtypes'] = islandora_cmodel_subtypes($cmodel);
  $variables['relationships']['subject'] = islandora_cmodel_subject_relationships($cmodel);
  $variables['relationships']['object'] = islandora_cmodel_object_relationships($cmodel);
}