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
    'location_taxonomize_init' => array(
      'method' => NULL,
      'fields' => $fields,
    ),
    'location_taxonomize_enable' => 1,
    'location_taxonomize_vid' => NULL,
    'location_taxonomize_node_attach' => 0,
    'location_taxonomize_longname' => array(
      'enable' => 0,
      'fields' => $fields,
      'separator' => ',',
      'country_code' => 1,
      'cap_co' => 1,
      'province_code' => 0,
      'cap_pr' => 0,
      'usa' => 1,
    ),
  );
}