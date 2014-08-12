<?php
/**
 * @file query_helper_functions.inc
 *       Provides a couple generic functions for performing Solr/Sparql queries.
 */

/**
 * islandora_object_related_islandora_objects
 *
 * @param       $object_id
 * @param array $cmodel_ids
 * @param array $relationships
 * @code
 *  array(
 *    'object' => 'islandora:555',
 *    'cmodels' => array('islandora:specimen_cmodel'),
 *    'relationships' => array(
 *      'fedora' => array('isPartOf'),
 *      'islandora' => array('someRelationship', 'anotherRelationship'),
 *    ),
 *    'page' => 2,
 *    'other' => 'PID:*', // Some Solr query
 * )
 *
 * @endcode
 *
 * @return array
 * @code
 *  array(
 *    'ids' => array(''),
 *    'num_found' => 10,
 *    'num_pages' => 2
 *  )
 * @endcode
 */
function related_islandora_objects($args){
  // Setup defaults
  $results['ids'] = array();
  $results['num_pages'] = 0;
  $results['num_found'] = 0;

  // Executes Solr Queries
  $query_processor = new IslandoraSolrQueryProcessor();
  islandora_xacml_api_islandora_solr_query($query_processor);

  // Object, CModels, Relationships
  $object_id = !empty($args['object']) ? str_replace(':', '\:', $args['object']) : '*';
  $cmodel_ids = !empty($args['cmodels']) ? $args['cmodels'] : array();
  $relationships = !empty($args['relationships']) ? $args['relationships'] : array();
  $page = !empty($args['page']) ? $args['page'] : 0;
  $other = !empty($args['other']) ? $args['other'] : array();
  $unrelated = !empty($args['unrelated']) ? $args['unrelated'] : false;

  $query_parts = array();

  if($other)
    $query_parts[] = $other;

  // CModel rels queries
  $cmodel_subquery = array();
  foreach($cmodel_ids as $cmodel_id){
    $cmodel_id = str_replace(':', '\:', $cmodel_id);
    $cmodel_subquery[] = "RELS_EXT_hasModel_uri_ms:$cmodel_id";
    $cmodel_subquery[] = "RELS_EXT_hasModel_uri_ms:info\:fedora/$cmodel_id";
  }

  if ($cmodel_subquery)
    $query_parts[] = '(' . implode(' OR ', $cmodel_subquery) . ')';


  // RELSEXT queries
  $fields = array();
  $rel_subqueries = array();
  if($object_id != '*'){

    if(empty($relationships)){
        $FEDORA_RELS_EXT_RELATIONSHIPS =  array(
            'fedora' => array(
                  'fedoraRelationship',
                  'isPartOf',             'hasPart',
                  'isConstituentOf',      'hasConstituent',
                  'isMemberOf',           'hasMember',
                  'isSubsetOf',           'hasSubset',
                  'isMemberOfCollection', 'hasCollectionMember',
                  'isDerivationOf',       'hasDerivation',
                  'isDependentOf',        'hasDependent',
                  'isDescriptionOf',      'hasDescription',
                  'isMetadataFor',        'hasMetadata',
                  'isAnnotationOf',       'hasAnnotation',
                  'hasEquivalent'
                )
            );
        $relationships = $FEDORA_RELS_EXT_RELATIONSHIPS;
      }

    foreach($relationships as $ns => $rels){
      foreach($rels as $rel){
        $rel_subqueries[] = "RELS_EXT_{$rel}_uri_ms:{$object_id}";
        $rel_subqueries[] = "RELS_EXT_{$rel}_uri_ms:info\:fedora/{$object_id}";
        $rel_subqueries[] = "RELS_EXT_{$ns}_{$rel}_uri_ms:{$object_id}";
        $rel_subqueries[] = "RELS_EXT_{$ns}_{$rel}_uri_ms:info\:fedora/{$object_id}";
      }
    }
  }

  if ($rel_subqueries) {
    if (!$unrelated){
      $query_parts[] = '(' . implode(' OR ',$rel_subqueries) . ')';
    }
    else {
      $query_parts[] = '( *:* NOT ('.implode(' OR ', $rel_subqueries).'))';
    }
  }

  // Whole Query
  $query = implode(' AND ', $query_parts);
//echo '<br/><pre>';
//var_dump($query);
//echo '</pre>';

  // Set page
  $_GET['page'] =  $page;

  file_put_contents('/Users/nolan/Desktop/query.txt', $query);

  // Execute Query
  $query_processor->buildAndExecuteQuery($query);

  $solr_results = $query_processor->islandoraSolrResult;

//  var_dump($solr_results);


  if(!empty($solr_results)){
    // Generate list of PIDs
    foreach($solr_results['response']['objects'] as $object_id){
      $results['ids'][] = $object_id['PID'];
    }
    $results['num_found'] = $solr_results['response']['numFound'];
    $results['num_pages'] = (int)($results['num_found'] / variable_get('islandora_solr_num_of_results', 5)) + 1;
  }

  return $results;
}


/**
 * The following function does basically the same thing as the one above it,
 * except it does it using SPARQL instead of Solr. Probably not going to use it.
 * @param null $id
 * @param null $cmodels
 * @param null $relationships
 * @param bool $subject
 *
 * @return array
 *
function i($id=null, $cmodels=null,  $relationships=null, $subject=FALSE) {

  // The default query finds objects that are targeting $id. If subject is set
  // to true then it finds objects that $id is targeting.
  if($id)
    $id = "<info:fedora/$id>";
  else
    $id = "?id";
  $sub = "?relative";
  $obj = $id;



  if($subject){
    $sub = $id;
    $obj = "?relative";
  }

  $cm_query = '';
  $rel_query = '{' . "$sub ?relationship $obj" . '}';

  // CModels - The objects in question must be one of the listed CModels.
  if($cmodels){
    $cm_triples = array();
    foreach($cmodels as $cmodel)
      $cm_triples[] = "{ ?relative fedora-model:hasModel  <info:fedora/$cmodel>}";

    $cm_query = '{' . implode('UNION', $cm_triples) . '}';
  }

  // Prefices and Relationships
  $pre[FEDORA_MODEL_URI] = 'fedora-model:';
  $prefix_def_array[] = 'PREFIX fedora-model: <info:fedora/fedora-system:def/model#>';
  if($relationships) {
    // We define the fedora-model prefix by default.
    $prefix = 'a';
    $rel_tripples = array();

    foreach ($relationships as $namespace => $rels) {
      // Create a PREFIX for each namespace.
      $pre[$namespace] = $prefix++ . ':';
      $prefix_def_array[] = "PREFIX {$pre[$namespace]} <$namespace>";

      // Create a triple for each relationship in the namespace.
      foreach($rels as $rel){
        $nsrel = $pre[$namespace] . $rel;
        $rel_tripples[] = '{' . "$sub $nsrel $obj" . '}';
      }
    }
    $rel_query = implode('UNION', $rel_tripples);
  }
  $prefix_definitions = implode(' ', $prefix_def_array);

  $query = "
    $prefix_definitions
    SELECT *
    FROM <#ri>
    WHERE
    {
      $cm_query
      $rel_query
    }
  ";

  /**file_put_contents('/Users/nolan/Desktop/query.txt', $query);

  $tuque = new IslandoraTuque();
  $result = $tuque->repository->ri->sparqlQuery($query);

  $related_objects = array();
  foreach ($result as $related_object) {
    $obj = islandora_object_load($related_object['relative']['value']);
    $related_objects[] = $obj ;
  }

  return $related_objects;
*/
