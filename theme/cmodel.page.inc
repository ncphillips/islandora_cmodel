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

  $page_state['variables'] = $variables;

  $page['supertypes'] = islandora_cmodel_supertypes_element($page_state);
  $page['subtypes'] = islandora_cmodel_subtypes_element($page_state);
  $page['relationships'] = islandora_cmodel_relationships_element($page_state);

  drupal_alter('islandora_cmodel_page', $page, $page_state);

  $supertype_hooks = array();
  foreach($variables['supertypes'] as $supertype){
    $supertype = str_replace(array(':', '-', '.'), "_", $supertype);
    $hook = "{$supertype}_islandora_cmodel_page";
    drupal_alter($hook, $page, $page_state);
  }
  $string = render($page);
  return $string;
}

function islandora_cmodel_supertypes_element(&$page_state){
  $element = array();

  $element['head'] = array(
    '#markup' => 'Supertypes',
    '#prefix' => '<h1>',
    '#suffix' => '</h1>',
  );

  $entries = array();
  foreach($page_state['variables']['supertypes'] as $id){
    $supertype = islandora_cmodel_load($id);
    if($supertype){
      $label = $supertype->label ? $supertype->label : $supertype->id;
      $entries[$id] = array('label' => l($label, "islandora/object/{$id}"));
    }
  }

  $element['table']['#theme'] = 'table';
  $element['table']['#header'] = array('label' => 'Content Type');
  $element['table']['#rows'] = $entries;
  return $element;
}

function islandora_cmodel_subtypes_element(&$page_state){
  $element = array();

  $element['#prefix'] = '<div id="cmodel-subtypes">';
  $element['#suffix'] = '</div>';

  $element['head'] = array(
    '#markup' => 'Subtypes',
    '#prefix' => '<h1>',
    '#suffix' => '</h1>',
  );

  $element['create'] = array(
    '#type' => 'button',
    '#value' => 'Create Subtype',
    '#ajax' => array(
      'callback' => 'ajax_create_subtype',
      'wrapper' => 'cmodel-subtypes',
    )
  );

  $entries = array();
  foreach($page_state['variables']['subtypes'] as $id){
    $subtype = islandora_cmodel_load($id);
    if($subtype){
      $label = $subtype->label ? $subtype->label : $subtype->id;
      $entries[$id] = array('label' => l($label, "islandora/object/{$id}"));
    }
  }

  $element['table']['#theme'] = 'table';
  $element['table']['#header'] = array('label' => 'Content Type');
  $element['table']['#rows'] = $entries;
  return $element;
}

function ajax_create_subtype($form, $form_state){
  return '';
}

function islandora_cmodel_relationships_element(&$page_state){
  $element = array();

  $self = $page_state['variables']['islandora_object'];

  $towards = array();
  $away = array();

  foreach($page_state['variables']['relationships'] as $rel){
    if($rel['object']['value'] == $self->id){
      $away[] = array(
        'subject' => $rel['subject']['value'],
        'relationship' => $rel['predicate']['alias'] .':'.$rel['predicate']['value'],
        'object' => $rel['object']['value'],
      );
    }
    else{
      $towards[] = array(
        'subject' => $rel['subject']['value'],
        'relationship' => $rel['predicate']['alias'] .':'.$rel['predicate']['value'],
        'object' => $rel['object']['value'],
      );
    }
  }

  $element['head'] = array(
    '#markup' => 'Relationships',
    '#prefix' => '<h1>',
    '#suffix' => '</h1>',
  );

  $element['towards'] = array(
    '#theme' => 'table',
    '#rows' => $towards,
    '#header' => array(
      'subject' => 'Subject',
      'relationship' => 'Reationship',
      'object' => 'Object',
    )
  );

  $element['away'] = array(
    '#theme' => 'table',
    '#rows' => $away,
    '#header' => array(
      'subject' => 'Subject',
      'relationship' => 'Reationship',
      'object' => 'Object',
    )
  );

  return $element;
}