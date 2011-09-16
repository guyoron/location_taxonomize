<?php

/**
 * @file
 * Some useful functions for Location taxonomize
 */

define('LOCATION_TAXONOMIZE_MODULE_ID', 'location_taxonomize');
define('LOCATION_TAXONOMIZE_MODULE_NAME', 'Location taxonomize');
define('LOCATION_TAXONOMIZE_VOCAB_NAME', 'location_taxonomize');


/**
 * Resets the module, as if it was uninstalled and installed again
 */
function _location_taxonomize_reset() {
  location_taxonomize_empty_vocab();
  _location_taxonomize_set_defaults();
  drupal_set_message(t('Module reset'));
}

/**
 * Keeps a list of all the variables maintained by this module, with their
 * default values.
 */
function _location_taxonomize_variables() {
  $fields = _get_location_fields();
  // set field defaults
  foreach ($fields as $key => $value) {
    if ($key != 'country' && $key != 'province' && $key != 'city') {
      $fields[$key] = 0;
    }
  }
  return array(
    'location_taxonomize_vid' => NULL,
    'location_taxonomize_vocab' => array(
      'method' => 'existing',
      'possible_vid' => NULL,
      'fields' => $fields,
    ),
    'location_taxonomize_settings' => array(
      'enable' => 1,
      'naming' => array(
        'country' => 'name',
        'province' => 'name',
        'usa' => 1,
      ),
      'na_text' => 'Unknown',
      'longname_enable' => 0,
      'longname' => array(
        // this is set by the initialization process
        'fields' => NULL,
        'separator' => ',',
        'country_naming' => 'code',
        'province_naming' => 'code',
        'usa' => 1,
      ),
    ),
  );
}

/**
 * Empties the Location Vocabulary (called as an AJAX form callback)
 */
function location_taxonomize_empty_vocab() {
  $terms = taxonomy_get_tree(variable_get('location_taxonomize_vid'));
  foreach ($terms as $term) {
    taxonomy_term_delete($term->tid);
  }
  $termsnow = taxonomy_get_tree(variable_get('location_taxonomize_vid'));
  if (!$termsnow) return t('Location vocabulary emptied.');
  return t('There was a problem emptying the Location Vocabulary.');
}

/**
 * Returns the Location module fields that can be used for the Location Vocabulary
 * @param $assoc - whether to return an associative array (TRUE) or just an
 *                 indexed array (FALSE)
 */
function _get_location_fields($assoc = TRUE) {
  // get names of fields in the location table
  $fields = drupal_schema_fields_sql('location');
  $return = array();
  // remove unsupported fields
  foreach ($fields as $field) {
    /* formatted this way to try to preserve readability as well as accpetable
       line lengths */
    if ($field != 'lid' &&
        $field != 'additional' &&
        $field != 'latitude' &&
        $field != 'longitude' &&
        $field != 'source' &&
        $field != 'is_primary' &&
        $field != 'postal_code'
    ) {
      if ($assoc) $return[$field] = $field;
      else $return[] = $field;
    }
  }
  return array_reverse($return);
}

/**
 * Returns the hierarchy configured for the current Location Vocabulary
 * @param $assoc - whether to return an associative array (TRUE) or
 *                keyed array (FALSE)
 */
function _location_taxonomize_get_hierarchy($assoc = FALSE) {
  $vocab = variable_get('location_taxonomize_vocab');
  $fields = $vocab['fields'];
  $hierarchy = array();
  foreach ($fields as $key => $value) {
    if ($value) {
      if ($assoc) $hierarchy[$key] = $key;
      else $hierarchy[] = $key;
    }
  }
  return $hierarchy;
}

/**
 * Disassociates the vocabulary - unlinks the current Location Vocabulary
 * from the module
 */
function location_taxonomize_disassociate($form, $form_state) {
  // unset the vid variable
  variable_del('location_taxonomize_vid');
  drupal_set_message(t('Location Taxonomy successfully disassociated'));
}

/**
 * Sets all this module's variables to their default values
 */
function _location_taxonomize_set_defaults() {
  $defaults = _location_taxonomize_variables();
  foreach ($defaults as $key => $value) {
    if ($value) variable_set($key, $value);
    else variable_del($key);
  }
}

/**
 * Deletes all variables set by this module
 */
function _location_taxonomize_del_variables() {
  $vars = _location_taxonomize_variables();
  foreach ($vars as $key => $value) {
    variable_del($key);
  }
  return t('All variables deleted');
}



