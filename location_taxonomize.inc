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
  return 'All variables set to default';
}

/**
 * Keeps a list of all the variables maintained by this module, with their default values.
 */
function _location_taxonomize_variables() {
  $fields = array(
    'country_name' => 'country_name', 
    'province_name' => 'province_name', 
    'city' => 'city'
  );
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
* Callback function for location_taxonomize_form
* clears the Location Vocabulary
*/
function location_taxonomize_empty_vocab () {
  $terms = taxonomy_get_tree(variable_get('location_taxonomize_vid'));
  foreach ($terms as $term) {
    taxonomy_term_delete($term->tid);
  }
  $termsnow = taxonomy_get_tree(variable_get('location_taxonomize_vid'));
  if (!$termsnow) return t('Location vocabulary cleared.');
  return t('There was a problem clearing the Location Vocabulary.');
}

function _get_location_fields () {
  $fields = location_field_names(TRUE);
  // for now, only country, city, and state are supported for this
  foreach ($fields as $field => $value) {
    if($field != 'country_name' && $field != 'province_name' && $field != 'city') {
      unset($fields[$field]);
    }
  }
  return $fields;
}
