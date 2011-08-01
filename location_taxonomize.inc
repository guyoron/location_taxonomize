<?

/**
* @file
* Just some includes for Location Taxonomy
*/

define('LOCATION_TAXONOMIZE_MODULE_ID', 'location_taxonomize');
define('LOCATION_TAXONOMIZE_MODULE_NAME', 'Location taxonomize');
define('LOCATION_TAXONOMIZE_VOCAB_NAME', 'location_taxonomize');

function _location_taxonomize_del_variables() {
  $vars = _location_taxonomize_variables();
  foreach ($vars as $key => $value) {
    variable_del($key);
  }
  return "All variables deleted";
}

function _location_taxonomize_set_defaults() {
$defaults = _location_taxonomize_variables();
  foreach ($defaults as $key => $value) {
    if($value) variable_set($key,$value);
    else variable_del($key);
  }
  
}

function _location_taxonomize_reset() {
  location_taxonomize_empty_vocab();
  _location_taxonomize_set_defaults();
  drupal_set_message('Module reset');
}

/**
 * Keeps a list of all the variables maintained by this module, with their default values.
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
      'method' => NULL,
      'possible_vid' => NULL,
      'fields' => $fields,
    ),
    'location_taxonomize_settings' => array(
      'enable' => 1,
      'node_attach' => 0,
      'longname_enable' => 0,
      'longname' => array(
        'fields' => $fields,
        'separator' => ',',
        'country_code' => 1,
        'cap_co' => 1,
        'province_code' => 0,
        'cap_pr' => 0,
        'usa' => 1,
      ),
    ),
  );
}

/**
* Empties the Location Vocabulary
*/
function location_taxonomize_empty_vocab () {
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
 */
function _get_location_fields () {
  // get name of site database
  global $databases;
  $db = $databases['default']['default']['database'];
  // get names of fields in the location table
  $result = db_query("SELECT column_name FROM information_schema.columns WHERE table_name = 'location' AND table_schema = :db ORDER BY ordinal_position", array(':db' => $db));
  $fields = $result->fetchCol();
  $return = array();
  // remove unsupported fields
  foreach ($fields as $field) {
    if($field != 'lid' && $field != 'additional' && $field != 'latitude' && $field != 'longitude' && $field != 'source' && $field != 'is_primary' && $field != 'postal_code') {
      $return[$field] = $field;
    }
  }
  return array_reverse($return);
}

/**
 * Returns the hierarchy configured for the current Location Vocabulary
 */
function _location_taxonomize_get_hierarchy() {
  $vocab = variable_get('location_taxonomize_vocab');
  $fields = $vocab['fields'];
  foreach ($fields as $key => $value) {
    if (!$value) unset($fields[$key]);
  }
  return $fields;
}

function location_taxonomize_disassociate($form, $form_state) {
  // unset the vid variable
  variable_del('location_taxonomize_vid');
  drupal_set_message('Location Taxonomy successfully disassociated.');
}





