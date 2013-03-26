<? php

$user = new StdClass();
$user->name = 'Jhonny5';
$user->mail = 'jhon5@example.com';

user_save($user);
dpm($user);
