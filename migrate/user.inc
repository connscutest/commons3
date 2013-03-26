<?php

/**
 * @file
 * Implementation of migration from Acme users to Drupal user accounts.
 */

/**
 * Extension of AcmeMigration to import Drupal users from the legacy acme_users
 * table.
 */
class AcmeUserMigration extends AcmeMigration {
  public function __construct() {
    parent::__construct();
    $this->description = t('Import of Acme users');

    $this->map = new MigrateSQLMap(
      $this->machineName,
      array(
        'userid' => array(
          'type' => 'int',
          'unsigned' => TRUE,
          'not null' => TRUE,
          'description' => 'User ID',
        )
      ),
      MigrateDestinationUser::getKeySchema()
    );

    $fields = array(
      // acme_users table
      'userid' => 'User ID',
      'username' => 'User name',
      'password' => 'AES-265 One-way hash of the user password and salt',
      'name' => 'Full name',
      'firstname' => 'First name',
      'lastname' => 'Last name',
      'email' => 'Email address',
      'creationdate' => 'Creation date',
      'modificationdate' => 'Last modified date',
      'lastloggedin' => 'Last time the user logged in',
      'status' => 'The current status of a newly registered user',
      'userenabled' => 'Whether the current user is enabled or not',
    );

    // We select all fields from the source table for import.
    $query = Database::getConnection('default', 'legacy')
             ->select('acme_users', 'u')
             ->fields('u');

    $this->source = new MigrateSourceSQL($query, $fields);
    $this->destination = new MigrateDestinationUser();

    // Mapped fields
    $this->addFieldMapping('name', 'username')
         ->description('We will allow usernames to be deduped')
         ->dedupe('users', 'name');
    $this->addFieldMapping('field_first_name', 'firstname');
    $this->addFieldMapping('field_last_name', 'lastname');
    $this->addFieldMapping('mail', 'email');
    $this->addFieldMapping('status', 'status')
         ->description('Set based on both status and userenabled');
    $this->addFieldMapping(NULL, 'userenabled')
         ->description('Used to help determine status');
    $this->addFieldMapping('created', 'creationdate')
         ->callbacks(array($this, 'processTimestamp'));
    $this->addFieldMapping('field_modified_date', 'modificationdate')
         ->callbacks(array($this, 'processTimestamp'));
    $this->addFieldMapping('login', 'lastloggedin')
         ->callbacks(array($this, 'processTimestamp'));
    $this->addFieldMapping('roles')
         ->defaultValue(DRUPAL_AUTHENTICATED_RID);

    // Unmapped destination fields
    $this->addUnmigratedDestinations(array('is_new', 'theme', 'signature',
             'signature_format', 'access', 'timezone','language', 'picture',
             'init', 'path'));

    // Unmapped source fields
    $this->addUnmigratedSources(array('name'));

    // The passwords deserve some annotation
    $this->addFieldMapping('pass')
         ->description('Users will be externally authenticated, no need for password')
         ->issueGroup('DNM');
    $this->addFieldMapping(NULL, 'password')
         ->description('Users will be externally authenticated, no need for password')
         ->issueGroup('DNM');
  }

  /**
   * Implementation of Migration::prepareRow().
   *
   * @param object $row
   *  The raw source row from the SQL query.
   *
   * @return bool
   *  FALSE to indicate the row should be skipped.
   */
  public function prepareRow($row) {
    if (parent::prepareRow($row) === FALSE) {
      return FALSE;
    }
    // Our rule is that a legacy status of 6 means the Drupal status follows the
    // legacy userenabled value...
    if ($row->status == 6) {
      if ($row->userenabled == 1) {
        $row->status = 1;
      }
      else {
        $row->status = 0;
      }
    }
    // ...and a legacy status of 3 means to not import the user at all.
    elseif ($row->status == 3) {
      return FALSE;
    }
  }
}

