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
  $variables['supertypes'] = islandora_cmodel_supertypes($cmodel->id);
  $variables['subtypes'] = islandora_cmodel_subtypes($cmodel->id);
  $variables['relationships'] = islandora_cmodel_relationships($cmodel->id);
}