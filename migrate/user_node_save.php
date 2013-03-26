<? php
$user = new StdClass();
$user->name = 'Jhonny6';
$user->mail = 'jhon6@example.com';

user_save($user);
dpm($user);

$node = new StdClass();
$node->title = 'This is a node';
$node->type = 'page';
$node->uid = $user->uid;
$node->body = array(
  LANGUAGE_NONE => array(
    0 => array(
      'value' => 'This is a migrated node.  Look at it go.'
	)
  )
);
node_save($node);
dpm($node, 'node');
