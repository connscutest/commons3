<?php

/**
 * @file
 * Common code for all Acme migrations.
 */

/**
 * Base class for all Acme migrations - encapsulates common behavior and provides
 * convenience methods to the real Migration classes.
 */
abstract class AcmeMigration extends DynamicMigration {
  public function __construct() {
    parent::__construct();

    $this->team = array(
      new MigrateTeamMember('Jane Roe', 'jroe@client.com', 'Stakeholder'),
      new MigrateTeamMember('John Doe', 'john_doe@partner.com', 'Site-builder'),
      new MigrateTeamMember('Mike Rater', 'mike.rater@acquia.com', 'Migrator'),
    );
  }

  /**
   * Convert an Acme millisecond-based timestamp to a standard Unix second-based
   * timestamp.
   *
   * @param int $value
   *  A timestamp in milliseconds.
   *
   * @return int
   *  A timestamp in seconds.
   */
  protected function processTimestamp($value) {
    return round($value/1000);
  }
}

