<?php

$node = new StdClass();
$node->title = 'This is a node';
$node->type = 'page';
$node->body = array(
  LANGUAGE_NONE => array(
    0 => array(
      'value' => 'This is a migrated node.  Look at it go.'
	)
  )
);
node_save($node);
dpm($node, 'node');
