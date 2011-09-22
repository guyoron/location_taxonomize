<?php

/**
 * @file
 * Some useful functions for Location taxonomize
 */
 
/**
 * Keeps a list of all the variables maintained by this module, with their
 * default values.
 */
function _location_taxonomize_af_variables() {
  $fields = location_taxonomize_af_get_fields(TRUE, TRUE);
  // set field defaults
  foreach ($fields as $key => $value) {
    if ($key != 'country' && $key != 'administrative_area' && $key != 'locality') {
      $fields[$key] = 0;
    }
  }
  return array(
    'location_taxonomize_af_settings' => array(
      'fields'  => $fields,
    ),
  );
}

/**
 * Returns the Location module fields that can be used for the Location Vocabulary
 * @param $assoc - whether to return an associative array (TRUE) or just an
 *                 indexed array (FALSE)
 */
function location_taxonomize_af_get_fields($assoc = TRUE, $opts = FALSE) {
  // get fields from Address Field
  $fields = addressfield_data_property_info();
  foreach($fields as $key => $value) {
    $fields[$key] = $value['label'];
  }
  $return = array();
  // remove unsupported fields
  foreach ($fields as $field => $value) {
    /* formatted this way to try to preserve readability as well as accpetable
       line lengths */
    if ($field != 'name_line' &&
        $field != 'first_name' &&
        $field != 'last_name' &&
        $field != 'organisation_name' &&
        $field != 'premise'
    ) {
      if ($opts) $return[$field] = $field;
      elseif ($assoc) $return[$field] = $value;
      else $return[] = $field;
    }
  }
  return $return;
}

/**
 * Sets all this module's variables to their default values
 */
function _location_taxonomize_af_set_defaults() {
  $defaults = _location_taxonomize_af_variables();
  foreach ($defaults as $key => $value) {
    if ($value) variable_set($key, $value);
    else variable_del($key);
  }
}

/**
 * Deletes all variables set by this module
 */
function _location_taxonomize_af_del_variables() {
  /*$vars = _location_taxonomize_location_variables();
  foreach ($vars as $key => $value) {
    variable_del($key);
  }*/
}