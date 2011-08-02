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
      'method' => 'existing',
      'possible_vid' => NULL,
      'fields' => $fields,
    ),
    'location_taxonomize_settings' => array(
      'enable' => 1,
      'node_attach' => 0,
      'naming' => array(
        'country' => 'name',
        'province' => 'name',
        'usa' => 1,
      ),
      'longname_enable' => 0,
      'longname' => array(
        'fields' => NULL,   // this is set by the initialization process
        'separator' => ',',
        'country_naming' => 'code',
        'province_naming' => 'code',
        'usa' => 1,
      ),
    ),
  );
}

function _location_taxonomize_init_var_longname() {
  $hierarchy = _location_taxonomize_get_hierarchy(FALSE);
  $defaults = array();
  foreach ($hierarchy as $field) {
    if ($field != 'country' && $field != 'province' && $field != 'city') {
      $defaults[$field] = 0;
    } else $defaults[$field] = $field;
  }
  $settings = variable_get('location_taxonomize_settings');
  $settings['longname']['fields'] = $defaults;
  variable_set('location_taxonomize_settings',$settings);
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
 * @param $assoc - whether to return an associative array (TRUE) or just an indexed array (FALSE) 
 */
function _get_location_fields ($assoc = TRUE) {
  // get name of site database
  global $databases;
  $db = $databases['default']['default']['database'];
  // get names of fields in the location table
  $result = db_query("SELECT column_name FROM information_schema.columns WHERE table_name = 'location' AND table_schema = :db ORDER BY ordinal_position", array(':db' => $db));
  $fields = $result->fetchCol();
  $return = array();
  // remove unsupported fields
  foreach ($fields as $field) {
    if ($field != 'lid' && $field != 'additional' && $field != 'latitude' && $field != 'longitude' && $field != 'source' && $field != 'is_primary' && $field != 'postal_code') {
      if ($assoc) $return[$field] = $field;
      else $return[] = $field;
    }
  }
  return array_reverse($return);
}

/**
 * Returns the hierarchy configured for the current Location Vocabulary
 * @param $assoc - whether to return an associative array (TRUE) or keyed array (FALSE)
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

function location_taxonomize_disassociate($form, $form_state) {
  // unset the vid variable
  variable_del('location_taxonomize_vid');
  drupal_set_message('Location Taxonomy successfully disassociated.');
}





