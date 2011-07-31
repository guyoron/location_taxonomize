<?php 

/**
* @file
* Contains the admin forms for Location Taxonomize
*/

/**
* Form function, called by drupal_get_form()
* in location_taxonomize_menu().
*/
function location_taxonomize_form($form, &$form_state) {
  $form['location_taxonomize_default'] = array(
    '#type' => 'button',
    '#value' => t('Set variables to defaults'),
    '#ajax' => array(
                'callback' => '_location_taxonomize_set_defaults',
                'wrapper' => 'msg--div',
                'method' => 'replace',
                'effect' => 'fade',
                'event' => 'mousedown',
                ),
  );
  // placeholder div for ajax message
  $form['msg--div'] = array(
    '#type' => 'container',
    '#id'=> 'msg--div',
  );
  // initialization
  $opts_vocab = variable_get('location_taxonomize_vocab');
  $vid = variable_get('location_taxonomize_vid');
  if (!$vid) {
    $form['location_taxonomize_vocab'] = array(
      '#type' => 'fieldset',
      '#title' => t('Initialization'),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
      '#tree' => TRUE,
    );
    $form['location_taxonomize_vocab']['state'] = array(
      '#type' => 'hidden',
      '#value' => 'initialization',
    );
    // check if there is already a Location vocab
    $vocab = taxonomy_vocabulary_machine_name_load(LOCATION_TAXONOMIZE_VOCAB_NAME);
    if ($vocab) {
      $possible_vid = $vocab->vid;
      $form['location_taxonomize_vocab']['possible_vid'] = array(
        '#type' => 'hidden',
        '#value' => $possible_vid,
      );
      $form['location_taxonomize_vocab']['method'] = array(
        '#type' => 'radios',
        '#title' => t('Choose an initialization method'),
        '#options' => array(
          'new' => t('Create a new Vocabulary to associate with Location Taxonomize'),
          'existing' => t('Use the existing vocabulary vid ' . $possible_vid),
        ),
        '#default_value' => $opts_vocab['method'],
        '#description' => t('If you use the existing vocabulary, all your terms will be maintained, and Location 
                            Taxonomize will start adding terms on top of the existing ones. Note that your
                            vocabulary must match the hierarchy structure you choose below, or there 
                            will be unexpected behavior.')
      );
    } else {
      $form['location_taxonomize_vocab']['method'] = array(
        '#type' => 'radios',
        '#title' => t('Choose an initialization method'),
        '#options' => array(
          'new' => t('Create a new Vocabulary to associate with Location Taxonomize'),
        ),
        '#default_value' => 'new',
        '#description' => t('If you want to use an existing Vocabulary, you must change that 
                            Vocabulary\'s machine name to \'' . LOCATION_TAXONOMIZE_VOCAB_NAME . '\'.
                             It will then be available here as an option.'),
      );
      
    }
    $form['location_taxonomize_vocab']['fields'] = array(
      '#type' => 'checkboxes',
      '#title' => t('Location Taxonomy Hierachy'),
      '#description' => t('Choose which Location fields to save as terms in the Location Taxonomy. For now, their 
                          relative hierarchy cannot be changed.'),
      '#options' => _get_location_fields(),
      '#default_value' => $opts_vocab['fields'],
    );
    $form = system_settings_form($form);
    $form['actions']['submit']['#value'] = t('Initialize Location Taxonomy');
    $form['#submit'][] = 'location_taxonomize_initialize';
    return $form;
  }
  // Vocabulary info
  $form['location_taxonomize_vocab'] = array(
    '#type' => 'fieldset',
    '#title' => t('Location Vocabulary Info'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#tree' => TRUE,
  );
  $form['location_taxonomize_vocab']['state'] = array(
    '#type' => 'hidden',
    '#value' => 'settings',
  );
  // display the status of the current Location Taxonomy
  $form['location_taxonomize_vocab']['vid'] = array(
    '#type' => 'item',
    '#title' => 'Vocabulary',
    '#markup' => 'Currently using Vocabulary vid ' . variable_get('location_taxonomize_vid'),
  );
  $opts_vocab = variable_get('location_taxonomize_vocab');
  // fields used for hierarchy, none editable now
  $form['location_taxonomize_vocab']['fields'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Location Taxonomy Hierachy'),
    '#description' => t('This cannot be changed once the vocabulary is initialized. If you want to change them, you have to unassociate this vocabulary and initialize a new one.'),
    '#options' => _get_location_fields(),
    '#default_value' => $opts_vocab['fields'],
    '#disabled' => TRUE,
  );
  // button to empty the location vocabulary
  $form['location_taxonomize_vocab']['emptyvocab'] = array(
    '#type' => 'button',
    '#value' => t('Empty Location Vocab'),
    '#ajax' => array(
                'callback' => 'location_taxonomize_empty_vocab',
                'wrapper' => 'msg-div',
                'method' => 'replace',
                'effect' => 'fade',
                'event' => 'mousedown',
                ),
  );
  // placeholder div for ajax message
  $form['location_taxonomize_vocab']['msg-div'] = array(
    '#type' => 'container',
    '#id'=> 'msg-div',
  );
  // get settings variables
  $opts_settings = variable_get('location_taxonomize_settings');
  $form['location_taxonomize_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Settings'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#tree' => TRUE,
  );
  // option to enable or disable synchronization
  $form['location_taxonomize_settings']['enable'] = array(
    '#type' => 'checkbox',
    '#title' => t('Synchronize the Location Taxonomy with data from the Location module'),
    '#default_value' => $opts_settings['enable'],
    '#description' => t('Turn on or off the main functionality of this module.'),
  );
  // option to assign terms to nodes
  $form['location_taxonomize_settings']['node_attach'] = array(
    '#type' => 'checkbox',
    '#title' => t('Also attach the appropriate Taxonomy Location terms to nodes'),
    '#description' => t('If this is checked, the terms saved in the Location Taxonomy will also automatically be applied to the 
                        nodes with those locations. Note that...'),
    '#default_value' => $opts_settings['node_attach'],
  );
  // option to enable or disable the Full Name field
  $form['location_taxonomize_settings']['longname_enable'] = array(
    '#type' => 'checkbox',
    '#title' => t('Save a Long Name field for every term'),
    '#description' => t('e.g. San Francisco, CA, USA'),
    '#default_value' => $opts_settings['longname_enable'],
  );
  $longname_opts = $opts_settings['longname'];
  // Long Name fieldset
  $form['location_taxonomize_settings']['longname'] = array(
    '#type' => 'fieldset',
    '#title' => t('Long Name'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#tree' => TRUE,
    '#states' => array(
      // Only show this field when the checkbox is enabled.
      'visible' => array(
        ':input[name="location_taxonomize_settings[longname_enable]"]' => array('checked' => TRUE),
      ),
    ),
  );
  // choose fields to use for Full Name field
  $form['location_taxonomize_settings']['longname']['fields'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Long Name fields'),
    '#description' => t('Choose which fields to use when constructing the Long Name'),
    '#options' => _get_location_fields(),
    '#default_value' => $longname_opts['fields'],
  );
  $form['location_taxonomize_settings']['longname']['separator'] = array(
    '#type' =>'textfield',
    '#title' => t('Separator'),
    '#maxlength' => 1,
    '#default_value' => $longname_opts['separator'],
    '#size' => 1,
    '#required' => TRUE,
  );
  $form['location_taxonomize_settings']['longname']['country_code'] = array(
    '#type' => 'checkbox',
    '#title' => t('Use the country code instead of its name where it is not the primary item'),
    '#default_value' => $longname_opts['country_code'],
  );
  $form['location_taxonomize_settings']['longname']['cap_co'] = array(
    '#type' => 'checkbox',
    '#title' => t('Capitalize country code in Long Name'),
    '#default_value' => $longname_opts['cap_co'],
    '#states' => array(
        // Only show this field when the checkbox is enabled.
        'enabled' => array(
          ':input[name="location_taxonomize_settings[longname][country_code]"]' => array('checked' => TRUE),
        ),
      ),
  );
  $form['location_taxonomize_settings']['longname']['province_code'] = array(
    '#type' => 'checkbox',
    '#title' => t('Use the province code instead of its name where it is not the primary item'),
    '#default_value' => $longname_opts['province_code'],
  );
  $form['location_taxonomize_settings']['longname']['cap_pr'] = array(
    '#type' => 'checkbox',
    '#title' => t('Capitalize province code in Long Name'),
    '#default_value' => $longname_opts['cap_pr'],
    '#states' => array(
        // Only show this field when the checkbox is enabled.
        'enabled' => array(
          ':input[name="location_taxonomize_settings[longname][province_code]"]' => array('checked' => TRUE),
        ),
      ),
  );
  $form['location_taxonomize_settings']['longname']['usa'] = array(
    '#type' => 'checkbox',
    '#title' => t('Use \'USA\' instead of \'US\''),
    '#description' => t('Capitalization will depend on capitalization settings'),
    '#default_value' => $longname_opts['usa'],
    '#states' => array(
        // Only show this field when the checkbox is enabled.
        'enabled' => array(
          ':input[name="location_taxonomize_settings[longname][country_code]"]' => array('checked' => TRUE),
        ),
      ),
  );
  return system_settings_form($form);
}
