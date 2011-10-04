<?php

/**
 * @file
 * Some useful functions for Location taxonomize
 */

define('LOCATION_TAXONOMIZE_MODULE_ID', 'location_taxonomize');
define('LOCATION_TAXONOMIZE_MODULE_NAME', 'Location taxonomize');
define('LOCATION_TAXONOMIZE_VOCAB_NAME', 'location_taxonomize');


/**
 * Resets the module, as if it was uninstalled and installed again and the
 * vocab was emptied.
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
  return array(
    'location_taxonomize_vid'   => NULL,
    'location_taxonomize_vocab' => array(
      'source'        => NULL,
      'method'        => 'existing',
      'possible_vid'  => NULL,
      'hierarchy'        => NULL,
    ),
    'location_taxonomize_settings' => array(
      'enable'  => 1,
      'naming'  => array(
        'country'   => 'name',
        'province'  => 'name',
        'usa'   => 1,
      ),
      'na_text'         => 'Unknown',
      'longname_enable' => 0,
      'longname'        => array(
        // this is set by the initialization process
        'fields'          => NULL,
        'separator'       => ',',
        'country_naming'  => 'code',
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
 * Returns the hierarchy configured for the current Location Vocabulary
 * @param $assoc - whether to return an associative array (TRUE) or
 *                keyed array (FALSE)
 * @param $labels - if TRUE and $assoc is TRUE, returns the labels
 *                  of the fields as values 
 */
function _location_taxonomize_get_hierarchy($assoc = FALSE, $labels = FALSE) {
  $settings = variable_get('location_taxonomize_vocab');
  $fields = $settings['hierarchy'];
  if ($labels) $withlabels = _location_taxonomize_get_fields(TRUE, TRUE);
  $hierarchy = array();
  foreach ($fields as $key => $value) {
    if ($value) {
      if ($assoc) {
        if ($labels) $hierarchy[$key] = $withlabels[$key];
        else $hierarchy[$key] = $key;
      }
      else $hierarchy[] = $key;
    }
  }
  return $hierarchy;
}

/**
 * Initializes the longname fields variable
 */
function _location_taxonomize_init_var_longname() {
  $hierarchy = _location_taxonomize_get_hierarchy(FALSE);
  $defaults = array();
  foreach ($hierarchy as $field) {
    if ($field != 'country' && $field != 'province' && $field != 'city') {
      $defaults[$field] = 0;
    }
    else $defaults[$field] = $field;
  }
  $settings = variable_get('location_taxonomize_settings');
  $settings['longname']['fields'] = $defaults;
  variable_set('location_taxonomize_settings', $settings);
}

/**
 * Returns an array indicating which of the possible Source modules are available
 */
function location_taxonomize_get_sources() {
  $possibilities = array(
    'location_taxonomize_location' => 'Location',
    'location_taxonomize_af' => 'Address Field'
  );
  $available = array();
  foreach ($possibilities as $module => $name) {
    if (module_exists($module)) {
      $available[$module] = $name;
    }
  }
  return $available;
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

/**
 * Returns the possible fields for the hierarchy
 * if $assoc, return value is in form key => key
 * if $labels, return value is in form key => value
 * if $source is set, gets the fields for that source
 */
function _location_taxonomize_get_fields($assoc = TRUE, $labels = FALSE, $source = NULL) {
  if ($source == NULL) {
    $source = _location_taxonomize_get_source();
  }
  $suffix = '_get_fields';
  $function = $source . $suffix;
  return $function($assoc);
}

/**
 * Returns the name of the current source module
 */
function _location_taxonomize_get_source() {
  $settings = variable_get('location_taxonomize_vocab');
  return $settings['source'];
}

/**
 * Returns the settings array for the current source module, or a given source
 */
function _location_taxonomize_get_source_settings($source = NULL) {
  if ($source == NULL) {
    $source = _location_taxonomize_get_source();
  }
  $suffix = '_settings';
  return variable_get($source . $suffix);
}

/**
 * Sets the default source to $module if the default source is not already set
 * Returns TRUE if set, FALSE if not set
 */
function location_taxonomize_source_default_set($module) {
  $settings = variable_get('location_taxonomize_vocab');
  if(empty($settings['source'])) {
    $settings['source'] = $module;
    variable_set('location_taxonomize_vocab', $settings);
    return TRUE;
  }
  return FALSE;
}

/**
 * Some fields are defined as 'Primary Fields'. 
 * This function returns a dictionary mapping internal field names
 * to source field names, or the other direction, if $reverse is TRUE
 */
function location_taxonomize_get_primary_fields($reverse = FALSE) {
  $source = _location_taxonomize_get_source();
  $suffix = '_primary_field_names';
  $function = $source . $suffix;
  $source_fields = $function();
  $internal_fields = array(
    'country',
    'country_name',
    'province',
    'province_name',
    'city',
  );
  if(!$reverse) {
    return array_combine($internal_fields, $source_fields);
  }
  else {
    return array_combine($source_fields, $internal_fields);
  }
}

/**
 * Converts a field name from the current source to an internal primary field
 * name if necessary. Otherwise returns the given name.
 */
function location_taxonomize_convert_field_name($name) {
  $primary_fields = location_taxonomize_get_primary_fields();
  foreach ($primary_fields as $key => $value) {
    if ($value == $name) {
      $name = $key;
      break;
    }
  }
  return $name;
}

/** 
 * Converts an internal name to a source name
 */
function location_taxonomize_source_field_name($internal_name) {
  $primary_fields = location_taxonomize_get_primary_fields();
  if (isset($primary_fields[$internal_name])) return $primary_fields[$internal_name];
  return $internal_name;
}

/**
 * Uses the core countries list to return a country code given a name
 */
function location_taxonomize_get_country_code($name) {
  require_once DRUPAL_ROOT . '/includes/locale.inc';
  $countries = country_get_list();
  $names = array_keys($countries, $name);
  if (!empty($names)) return array_pop($names);
  return NULL;
}

/**
 * Uses the core countries list to return a country name given a code
 */
function location_taxonomize_get_country_name($code) {
  require_once DRUPAL_ROOT . '/includes/locale.inc';
  $countries = country_get_list();
  if (isset($countries[strtoupper($code)])) return $countries[strtoupper($code)];
  return NULL;
}