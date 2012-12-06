<?php

/**
 * @file
 *  A panels content type plugin to display a LinkedIn Profile widget.
 */


// Give a plugin definition to ctools. We won't specify render or settings
// callsbacks here, as we'll use the standard naming pattern.
$plugin = array(
  'title' => t('LinkedIn Profile Widget'),
  'description' => t('Display a users LinkedIn Profile.'),
  'defaults' => array(
    'profile_url' => 'http://www.linkedin.com/pub/brad-bowman/7/412/919',
  ),
  'category' => t('Misc'),
);


/**
 * Output function for the "LinkedIn Pane" content type.
 *
 * Output a linkedin profile based on the profile ID input during panel configuration.
 */
function linkedin_pane_linkedin_pane_content_type_render($subtype, $conf, $panel_args) {
  // Check to ensure we have both a profile URL and the global Drupal has_js cookie is
  // enabled. There's no real downside of trying any of this without having JS enabled, 
  // except for pane titles showing without content.
  if (empty($_COOKIES['has_js']) && empty($conf['profile_url'])) {
    return FALSE;
  }

  // Include linkedin API JS. 
  _linkedin_pane_add_js();

  // Formulate the profile markup.
  $pane = new stdClass();
  $pane->title = t('Profile');
  $pane->content = '<script type="IN/MemberProfile" data-id="' . $conf['profile_url'] . '" data-related="false" data-format="inline"></script>';
  return $pane;
}

/**
 * A configuration form to allow users to specify which linkedin profile they would
 * like to display.
 */
function linkedin_pane_linkedin_pane_content_type_edit_form($form, &$form_state) {
  $conf = $form_state['conf'];

  $form['settings']['profile_url'] = array(
    '#type' => 'textfield',
    '#default_value' => $conf['profile_url'],
    '#title' => t('Profile URL'),
    '#required' => TRUE,
    '#description' => t('The fully qualified URL of the LinkedIn profile you would like to display.'),
  );

  return $form;
}

/**
 * Save form settings on submit.
 */
function linkedin_pane_linkedin_pane_content_type_edit_form_submit($form, &$form_state) {
  $form_state['conf']['profile_url'] = $form_state['values']['profile_url'];
}

/**
 * Validate the profile URL. This isn't comprehensive but provides some reasonable sanity checking.
 */
function linkedin_pane_linkedin_pane_content_type_edit_form_validate(&$form, &$form_state) {

  if (empty($form_state['values']['profile_url'])) {
    form_set_error('profile_url', t('You must enter a Profile URL.'));
  } 
  elseif (!valid_url($form_state['values']['profile_url'], TRUE)) {
    form_set_error('profile_url', t("You must enter a valid URL. Don't forget to include http://"));
  }
  elseif (parse_url($form_state['values']['profile_url'], PHP_URL_HOST) != 'www.linkedin.com') {
    form_set_error('profile_url', t('Only Profile URLs from www.linkedin.com are valid.'));
  }

}

/**
 * Quick and dirty way to include the LinkedIn API JS.
 */
function _linkedin_pane_add_js() {
  $js = '<script type="text/javascript" src="http://platform.linkedin.com/in.js">
            api_key: ' . variable_get('linkedin_pane_api_key', LINKEDIN_PANE_DEFAULT_API_KEY) . '
        </script>';

  $element = array(
    '#type' => 'markup',
    '#markup' => $js
  );

  drupal_add_html_head($element, 'linked_api_js');
}