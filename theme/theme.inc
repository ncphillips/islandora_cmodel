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
  $variables['supertypes'] = islandora_cmodel_types($cmodel);
  $variables['subtypes'] = islandora_cmodel_subtypes($cmodel);
  $variables['relationships'] = islandora_cmodel_relationships($cmodel);
}