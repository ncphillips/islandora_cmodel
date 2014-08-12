<?php
$cmodel = $variables['islandora_object'];
$supertypes = $variables['supertypes'];
$subtypes = $variables['subtypes'];
$child_relationships = $variables['relationships']['subject'];
$parent_relationships = $variables['relationships']['subject'];
?>


<h2>Types</h2>

<h3>Supertypes</h3>
<table>
<thead>
  <th>Type</th>
</thead>
<tbody>
<?php
  foreach($supertypes as $st_id){
    $st = islandora_cmodel_load($st_id);
    echo "<tr>";
    echo    "<td>{$st->label}</td>";
    echo "</tr>";
  }
?>
</tbody>
</table>

<h3>Subtypes</h3>
<table>
<thead>
  <th>Type</th>
</thead>
<tbody>
<?php
foreach($subtypes as $st_id){
  $st = islandora_cmodel_load($st_id);
  echo "<tr>";
  echo    "<td>{$st->label}</td>";
  echo "</tr>";
}
?>
</tbody>
</table>

<h2>Relationships</h2>
<h3>Object Relationships</h3>
This is a list of relationships defined by the <?echo $cmodel->label; ?> content
model. Each of these relationships point from the <?echo $cmodel->label; ?>
 towards some other content model.
<table>
  <thead>
    <th>Object</th>
    <th>Namespace</th>
    <th>Relationship</th>
    <th>Subject</th>
  </thead>
  <tbody>
  <?php
  foreach($parent_relationships as $rel ){
    $link = l($rel['object']['value'], "islandora/object/{$rel['object']['value']}}");
    echo "<tr>";
    echo "<td>{$cmodel}</td>";
    echo "<td>{$rel['predicate']['alias']}</td>";
    echo "<td>{$rel['predicate']['value']}</td>";
    echo "<td>$link</td>";
    echo "</tr>";
  }
  ?>
  </tbody>
</table>
<h3>Subject Relationships</h3>
This is a list of relationships defined by other content
models. The <?echo $cmodel->label; ?> content model is the subject (target) of these
relationships.
<table>
  <thead>
    <th>Object</th>
    <th>Namespace</th>
    <th>Relationship</th>
    <th>Subject</th>
  </thead>
  <tbody>
  <?php
  foreach($child_relationships as $rel ){
    $link = l($rel['object']['value'], "islandora/object/{$rel['object']['value']}}");
    echo "<tr>";
    echo "<td>$link</td>";
    echo "<td>{$rel['predicate']['alias']}</td>";
    echo "<td>{$rel['predicate']['value']}</td>";
    echo "<td>{$cmodel}</td>";
    echo "</tr>";
  }
  ?>
  </tbody>
</table>
</tbody>
</table>
