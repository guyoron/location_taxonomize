<?php

/**
 * @file
 * Contains the admin forms for Location Taxonomize
 */


/**
 * Location Taxonomize administration form
 */
function location_taxonomize_form($form, &$form_state) {
  $opts_vocab = variable_get('location_taxonomize_vocab');
  // check if there is already a source selected in form_state
  if (!empty($form_state['values']['location_taxonomize_vocab']['source'])) {
    $source = $form_state['values']['location_taxonomize_vocab']['source'];
    // remove the choices for the previous Fields field
    unset($form_state['input']['location_taxonomize_vocab']['fields']);
  }
  else $source = $opts_vocab['source'];
  $vid = variable_get('location_taxonomize_vid');
  $initialized = !empty($vid);
  
  // Vocab Info Fieldset
  $form['location_taxonomize_vocab'] = array(
    '#type' => 'fieldset',
    '#title' => t('Initialization'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#tree' => TRUE,
    '#description' => 'Before you use Location Taxonomize, you need to
    initialize it. This will bind Location Taxonomize to a specific vocabulary,
    and determine the hierarchy structure of the vocabulary. A new Location
    Vocabulary can be created automatically (you need to not have any
    vocabularies named location_taxonomize for this), or you can start with
    an existing vocabulary (to do this you must change its name to
    location_taxonomize first). These settings cannot be changed after
    initialization. But you can always disassociate the vocabulary you
    choose here, and then initialize again with other settings.',
  );
  if ($initialized) {
    $form['location_taxonomize_vocab']['#title'] = t('Location Vocabulary Info');
    $form['location_taxonomize_vocab']['#collapsed'] = TRUE;
    unset($form['location_taxonomize_vocab']['#description']);
  }
  
  // hidden form element used to indicate if LT is initialized already or not
  $form['location_taxonomize_vocab']['state'] = array(
    '#type' => 'hidden',
  );
  if ($initialized) {
    $form['location_taxonomize_vocab']['state']['#value'] = 'settings';
  } else {
    $form['location_taxonomize_vocab']['state']['#value'] = 'initialization';
  }
  
  // display the status of the current Location Taxonomy, if initialized
  if ($initialized) {
    $form['location_taxonomize_vocab']['vid'] = array(
      '#type' => 'item',
      '#title' => t('Vocabulary'),
      '#markup' => t('Currently using Vocabulary vid @vid',
                    array('@vid' => variable_get('location_taxonomize_vid'))),
      '#description' => t('If you want to change this, you need to disassociate
                          the vocabulary first, using the button below.')
    );
  }
  
  // Source selection
  $source_options = location_taxonomize_get_sources();
  $form['location_taxonomize_vocab']['source'] = array(
    '#type'     => 'radios',
    '#title'    => t('Location data source'),
    '#options'  => $source_options,
    '#default_value'  => $source,
    '#required' => TRUE,
    '#ajax'     => array(
      'wrapper' => 'source-ajax-wrapper',
      'callback'=> 'location_taxonomize_source_ajax_callback',
      'progress'  => array(
        'message' => ''
      ),
    ),
  );
  if (empty($source_options)) {
    $form['location_taxonomize_vocab']['source']['#description'] = 'You must enable one of the available source modules first.';
    // Stop the form here
    $form = system_settings_form($form);
    $form['actions']['submit']['#value'] = t('Initialize Location Taxonomy');
    $form['#submit'][] = 'location_taxonomize_initialize';
    return $form;
  }
  if ($initialized) {
    $form['location_taxonomize_vocab']['source']['#disabled'] = TRUE;
  }
  
  // Choose initialization method, if we have not initialized
  if(!$initialized) {
    // check if there is already a Location vocab we could use
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
        '#required' => TRUE,
        '#default_value'  => 'existing',
        '#options' => array(
          'new' => t('Create a new Vocabulary to associate with Location Taxonomize'),
          'existing' => t('Use the existing vocabulary vid @vid',
                          array('@vid' => $possible_vid)),
        ),
        '#description' => t('If you use the existing vocabulary, all your terms
                            will be maintained, and Location Taxonomize will
                            start adding terms on top of the existing ones. Note
                            that your vocabulary must match the hierarchy
                            structure you choose below, or there will be
                            unexpected behavior.'),
      );
    }
    else {
      $form['location_taxonomize_vocab']['method'] = array(
        '#type' => 'radios',
        '#title' => t('Choose an initialization method'),
        '#required' => TRUE,
        '#options' => array(
          'new' => t('Create a new Vocabulary to associate with Location Taxonomize'),
        ),
        '#default_value' => 'new',
        '#description' => t("If you want to use an existing Vocabulary, you must
                            change that Vocabulary's machine name to '@name'.
                            It will then be available here as an option.",
                            array('@name' => LOCATION_TAXONOMIZE_VOCAB_NAME)),
      );

    }
  }

  // get the options for the selected or default source
  $source_settings = _location_taxonomize_get_source_settings($source);
  // Hierarchy selection
  $form['location_taxonomize_vocab']['hierarchy'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Location Taxonomy Hierarchy'),
    '#description' => t('Choose which Location fields to save as terms in the
                        Location Taxonomy. For now, their relative hierarchy
                        cannot be changed. They will be nested hierarchically
                        in the order shown above.'),
    '#options' => _location_taxonomize_get_fields(TRUE, TRUE, $source),
    '#default_value' => $source_settings['fields'],
    '#prefix' => '<div id="source-ajax-wrapper">',
    '#suffix' => '</div>'
  );
  if ($initialized) {
    $form['location_taxonomize_vocab']['hierarchy']['#disabled'] = TRUE;
    $form['location_taxonomize_vocab']['hierarchy']['#default_value'] = $opts_vocab['hierarchy'];
  }
  
  // Stop the form here if we have not yet initialized
  if (!$initialized) {
    $form = system_settings_form($form);
    $form['actions']['submit']['#value'] = t('Initialize Location Taxonomy');
    $form['#submit'][] = 'location_taxonomize_initialize';
    return $form;
  }
  
  // operations fieldset
  $form['location_taxonomize_ops'] = array(
    '#type' => 'fieldset',
    '#title' => 'Operations',
    '#tree' => FALSE,
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#description' => t('<p>The three buttons below will let you:</p><ul><li>
      <strong>Empty Vocab:</strong> Remove all terms from the location vocabulary
      (All terms will be removed IMMEDIATELY. Mostly here for testing purposes)</li>
      <li><strong>Disassociate Vocab:</strong> Remove the association with the current
      Location Vocabulary. The vocabulary will remain with all its terms, but it will
      not be connected to this module. You can then change basic settings and
      re-associate it.</li><li><strong>Bulk Taxonomize Locations:</strong> This will
      process all locations that have been saved on this site by the Location module
      and add them to the Location Vocabulary</li></ul>'),
  );
  
  // button to empty the location vocabulary
  $form['location_taxonomize_ops']['emptyvocab'] = array(
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
  
  // disassociate button
  $form['location_taxonomize_ops']['disassociate'] = array(
    '#type' => 'submit',
    '#value' => t('Disassociate Vocab'),
    '#submit' => array('location_taxonomize_disassociate', ),
  );
  
  // bulk taxonomize button
  $form['location_taxonomize_ops']['bulk_taxonomize'] = array(
    '#type' => 'submit',
    '#value' => t('Bulk Taxonomize Locations'),
    '#submit' => array('location_taxonomize_bulk_taxonomize', ),
  );
  
  // placeholder div for ajax message
  $form['location_taxonomize_ops']['msg-div'] = array(
    '#type' => 'container',
    '#id' => 'msg-div',
  );
  
  // get settings variables
  $opts_settings = variable_get('location_taxonomize_settings');
  
  // settings fieldset
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
    '#title' => t('Synchronize the Location Taxonomy with data from 
                  the Location module'),
    '#default_value' => $opts_settings['enable'],
    '#description' => t('Turn on or off the main functionality of this module.'),
  );
  
  // naming fieldset
  $form['location_taxonomize_settings']['naming'] = array(
    '#type'         => 'fieldset',
    '#title'        => t('Term Naming Settings'),
    '#collapsible'  => TRUE,
    '#collapsed'    => FALSE,
    '#tree'         => TRUE,
    '#description' => t('Change the way Location terms are named in the
                        Location Vocabulary.'),
  );
  
  $form['location_taxonomize_settings']['naming']['country'] = array(
    '#type'       => 'radios',
    '#title'      => t('Country naming'),
    '#required'   => TRUE,
    '#options'    => array(
      'name' => t('Use full name (i.e. <em>United States</em>)'),
      'code' => t('Use country code (i.e. <em>US</em>)'),
    ),
    '#default_value' => $opts_settings['naming']['country'],
  );
  
  $form['location_taxonomize_settings']['naming']['usa'] = array(
    '#type'       => 'checkbox',
    '#title'      => t("Use 'USA' instead of 'US'"),
    '#default_value' => $opts_settings['naming']['usa'],
    '#states' => array(
      'enabled' => array(
        ':input[name="location_taxonomize_settings[naming][country]"]' => array('value' => 'code'),
      ),
    ),
  );
  
  $form['location_taxonomize_settings']['naming']['province'] = array(
    '#type'       => 'radios',
    '#title'      => t('Province naming'),
    '#required'   => TRUE,
    '#options'    => array(
      'name' => t('Use full name (i.e. <em>California</em>) if available (will default to the province code if name is unavailable)'),
      'code' => t('Use province code (i.e. <em>CA</em>)'),
    ),
    '#default_value' => $opts_settings['naming']['province'],
  );
  
  // hide provine naming if there is no province field
  $province_name = location_taxonomize_source_field_name('province');
  //debug($province_name, 'province_name');
  $has_province_field = isset($source_settings['fields'][$province_name]);
  //debug($has_province_field, 'has_province_field');
  $province_field_on = $source_settings['fields'][$province_name] != 0;
  //debug($province_field_on, 'province_field_on');
  if (!$has_province_field || !$province_field_on) {
    $form['location_taxonomize_settings']['naming']['province']['#access'] = FALSE;
  }
  
  // text to use if there is no value to save for a term
  $form['location_taxonomize_settings']['na_text'] = array(
    '#type'           => 'textfield',
    '#title'          => 'N/A text',
    '#description'    => "This text will be saved if there is nothing to save
      for a term. For instance, if your hierarchy includes the City field, but
      the field is not required. If a user doesn't enter a city, this text will
      appear as the term name instead. To avoid this being entered in your
      Location Vocabulary, make sure that all the fields in your hierarchy are
      also required by the Location module settings.",
    '#default_value'  => $opts_settings['na_text'],
    '#required'       => 1,
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
    '#description' => t('Choose which fields to use when
                        constructing the Long Name'),
    '#options' => _location_taxonomize_get_hierarchy(TRUE, TRUE),
    '#default_value' => $longname_opts['fields'],
  );
  
  $form['location_taxonomize_settings']['longname']['separator'] = array(
    '#type' => 'textfield',
    '#title' => t('Separator'),
    '#maxlength' => 1,
    '#default_value' => $longname_opts['separator'],
    '#size' => 1,
    '#required' => FALSE,
    '#states' => array(
      // Mark this field required when the Long Name checkbox is enabled.
      'required' => array(
        ':input[name="location_taxonomize_settings[longname_enable]"]' => array('checked' => TRUE),
      ),
    ),
  );
  
  $form['location_taxonomize_settings']['longname']['country_naming'] = array(
    '#type' => 'radios',
    '#title' => t('Country naming in Long Names'),
    '#description' => t('Determines how the country will be saved in Long Names'),
    '#options' => array(
      'name' => t('Always use full name (i.e. <em>United States</em>)'),
      'code' => t('Use province code (i.e. <em>US</em>) when it is not the primary
                  item in the Full Name (i.e. <em>California, US</em>)'),
    ),
    '#default_value' => $longname_opts['country_naming'],
  );
  
  $form['location_taxonomize_settings']['longname']['usa'] = array(
    '#type' => 'checkbox',
    '#title' => t("Use 'USA' instead of 'US'"),
    '#default_value' => $longname_opts['usa'],
    '#states' => array(
      // Only show this field when the checkbox is enabled.
      'enabled' => array(
        ':input[name="location_taxonomize_settings[longname][country_naming]"]' => array('value' => 'code'),
      ),
    ),
  );
  
  $form['location_taxonomize_settings']['longname']['province_naming'] = array(
    '#type' => 'radios',
    '#title' => t('Province naming in Long Names'),
    '#description' => t('Determines how the province will be saved in Long Names'),
    '#options' => array(
      'name' => t('Always use full name (i.e. <em>California</em>)'),
      'code' => t('Use province code (i.e. <em>CA</em>) when it is not the primary
                  item in the Full Name (i.e. <em>San Francisco, CA</em>)'),
    ),
    '#default_value' => $longname_opts['province_naming'],
  );
  if (!$has_province_field || !$province_field_on) {
    $form['location_taxonomize_settings']['longname']['province_naming']['#access'] = FALSE;
  }
  return system_settings_form($form);
}

function location_taxonomize_source_ajax_callback($form, &$form_state) {
  //dpm('ajax callback');
  return $form['location_taxonomize_vocab']['fields'];
}

/**
 * Validation hook for the module's admin form
 */
function location_taxonomize_form_validate($form, &$form_state) {
  $values = $form_state['values'];
  //  if we're initializing, make sure that there are no interfering vocabs
  $state = $values['location_taxonomize_vocab']['state'];
  switch ($state) {
    case 'initialization':
      $values_init = $values['location_taxonomize_vocab'];
      $vid = variable_get('location_taxonomize_vid');
      if ($vid) form_set_error('', t('There seems to already be a Vocabulary
                              associated with Location Taxonomize.'));
      $vocab = taxonomy_vocabulary_machine_name_load(LOCATION_TAXONOMIZE_VOCAB_NAME);
      if ($values_init['method'] == 'new' && $vocab) {
        form_set_error('', t("Could not create new Location Taxonomy vocabulary.
                       A vocabulary with the name '@name' already exists.",
                       array('@name' => LOCATION_TAXONOMIZE_VOCAB_NAME)));
      }
      if ($values_init['method'] == 'existing') {
        if (!$vocab) form_set_error('', t("Could not use the existing vocabulary.
                                    There is no existing vocabulary with the name
                                    '@name'", array('@name' => LOCATION_TAXONOMIZE_VOCAB_NAME)));
        elseif ($vocab->vid != $values_init['possible_vid']) {
          form_set_error('', t('Could not use the existing vocabulary (vid @vid). Wrong vid.',
                               array('@vid' => $values_init['possible_vid'])));
        }
      }
    break;
    case 'settings':
      // if the Long Name option is on, the Separator field is required
      $longname_enabled = $form_state['values']['location_taxonomize_settings']['longname_enable'];
      $separator_empty = empty($form_state['values']['location_taxonomize_settings']['longname']['separator']);
      if ($longname_enabled && $separator_empty) {
        form_set_error(
          'location_taxonomize_settings][longname][separator', 
          t('Separator field is required.')
        );
      }
    break;
  }
}
