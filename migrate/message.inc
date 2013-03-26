<?php

/**
 * @file
 * Implementation of migration from Acme messages to Drupal message nodes.
 */

/**
 * Extension of AcmeMigration to import Drupal messages from the legacy
 * acme_messages table.
 */
class AcmeMessageMigration extends AcmeMigration {
  public function __construct() {
    parent::__construct();
    $this->description = t('Import of Acme messages');
    $this->dependencies = array('AcmeUser');

    $fields = array(
      // acme_messages table
      'messageid' => 'Message ID',
      'parentmessageid' => 'ID of parent message',
      'userid' => 'User ID',
      'subject' => 'Message subject',
      'body' => 'Content of the message',
      'teaser' => 'Teaser for the message',
      'creationdate' => 'Creation date',
      'modificationdate' => 'Last modified date',
      'status' => 'Published status',
    );

    // We select all fields from the source table for import.
    $query = Database::getConnection('default', 'legacy')
             ->select('acme_messages', 'm')
             ->fields('m');
    $this->source = new MigrateSourceSQL($query, $fields);

    $this->destination = new MigrateDestinationNode('message');

    $this->map = new MigrateSQLMap(
      $this->machineName,
      array(
        'messageid' => array(
          'type' => 'int',
          'unsigned' => TRUE,
          'not null' => TRUE,
          'description' => 'Message ID',
        )
      ),
      MigrateDestinationNode::getKeySchema()
    );

    // Mapped fields
    $this->addFieldMapping('uid', 'userid')
         ->sourceMigration('AcmeUser');
    $this->addFieldMapping('title', 'subject');
    $this->addFieldMapping('body', 'body');
    $this->addFieldMapping('body:summary', 'teaser');
    $this->addFieldMapping('body:format')
         ->defaultValue('filtered_html');
    $this->addFieldMapping('created', 'creationdate')
         ->callbacks(array($this, 'processTimestamp'));
    $this->addFieldMapping('changed', 'modificationdate')
         ->callbacks(array($this, 'processTimestamp'));
    $this->addFieldMapping('status', 'status');
    $this->addFieldMapping('field_parent_message', 'parentmessageid')
         ->sourceMigration('AcmeMessage');

    // Unmapped destination fields
    $this->addUnmigratedDestinations(array('is_new', 'revision_uid', 'promote',
      'sticky', 'revision', 'log', 'language', 'tnid', 'path', 'comment'));

    // No unmapped source fields
  }

  /**
   * Implementation of Migration::createStub().
   *
   * @return array
   *  Return an array containing the nid of the created stub node, or FALSE if
   *  node creation failed.
   */
  protected function createStub() {
    $node = new stdClass;
    $node->title = t('Stub');
    $node->body = t('Stub body');
    $node->type = $this->destination->getBundle();
    $node->uid = 1;
    $node->name = 'admin';
    $node->status = 0;
    node_save($node);
    if (isset($node->nid)) {
      return array($node->nid);
    }
    else {
      return FALSE;
    }
  }
}

