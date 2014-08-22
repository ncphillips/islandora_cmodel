<?php

/**
 * CModel Page
 *
 * This function gets called when an islandora CModel is viewed.
 *
 * @param $variables
 * @return string
 *
 * Preprocessed variables passed in by
 * @see islandora_cmodel_preprocess_cmodel()
 *
 */
function islandora_cmodel_page($variables){
  $page = array();
  $page_state = array();

  drupal_alter('islandora_cmodel_page', $page, $page_state);

  $supertype_hooks = array();
  foreach($variables['supertypes'] as $supertype){
    $supertype = str_replace(array(':', '-', '.'), "_", $supertype);
    $hook = "{$supertype}_islandora_cmodel_page";
    drupal_alter($hook, $page, $page_state);
  }

  return "Hello";
}
