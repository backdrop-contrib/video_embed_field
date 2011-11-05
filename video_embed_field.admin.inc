<?php

/**
 * @file
 * Administration pages for video styles.
 */

/**
 * Menu callback; Listing of all current image styles.
 */
function video_embed_field_video_style_list() {
  $page = array();
  
  $styles = video_embed_field_video_styles();
  $page['video_embed_field_video_style_list'] = array(
    '#markup' => theme('video_embed_field_video_style_list', array('styles' => $styles)),
  );
  
  return $page;
}

/**
 * Form builder; Form for editing a video style.
 *
 * @ingroup forms
 * @see video_embed_field_video_style_form_submit()
 */
function video_embed_field_video_style_form($form, &$form_state, $style) {
  $form_state['video_style'] = $style;
  
  $title = t('Edit %name style', array('%name' => $style['name']));
  drupal_set_title($title, PASS_THROUGH);
  
  // Adjust this form for styles that must be overridden to edit.
  $editable = (bool) ($style['storage'] & VIDEO_EMBED_FIELD_VIDEO_STORAGE_EDITABLE);
  
  //Grab the settings off the parser form
  $parser_form = video_embed_field_get_form($style['settings']);
  
  //General settings for playback - formerly in the configuration section
  $form['settings'] = array(
    '#type' => 'vertical_tabs',
    '#title' => t('Playback settings'),
  ) + $parser_form; //add in our extra settings
  
  // Show the Override or Submit button for this style.
  $form['actions'] = array('#type' => 'actions');
  $form['actions']['override'] = array(
    '#type' => 'submit',
    '#value' => t('Override defaults'),
    '#validate' => array(),
    '#submit' => array('video_embed_field_video_style_form_override_submit'),
    '#access' => !$editable,
  );
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Update style'),
    '#access' => $editable,
  );
  
  return $form;
}

/**
 * Submit handler for adding a new image style.
 */
function video_embed_field_video_style_form_submit($form, &$form_state) {
  $style = $form_state['video_style'];
  $style['data'] = _video_embed_field_get_settings($form_state);
  
  $style = video_embed_field_video_style_save($style);
  drupal_set_message(t('Style %name was updated.', array('%name' => $style['name'])));
  $form_state['redirect'] = 'admin/config/media/video-embed-field';
}

/**
 * Submit handler for overriding a module-defined style.
 */
function video_embed_field_video_style_form_override_submit($form, &$form_state) {
  $style = $form_state['video_style'];
  $style['data'] = _video_embed_field_get_settings($form_state);
  
  drupal_set_message(t('The %style style has been overridden, allowing you to change its settings.', array('%style' => $style['name'])));
  video_embed_field_default_style_save($style);
}

/**
 * Extracts player settings from a submitted form.
 */
function _video_embed_field_get_settings(&$form_state) {
  form_state_values_clean($form_state);
  unset($form_state['values']['settings__active_tab']);
  unset($form_state['values']['override']);
  if (isset($form_state['values']['name'])) {
    unset($form_state['values']['name']);
  }
  
  return $form_state['values'];
}

/**
 * Form builder; Form for adding a new video style.
 *
 * @ingroup forms
 * @see video_embed_field_video_style_add_form_submit()
 * @see video_embed_field_video_style_name_validate()
 */
function video_embed_field_video_style_add_form($form, &$form_state) {
  $form['name'] = array(
    '#type' => 'textfield',
    '#size' => '64',
    '#title' => t('Style name'),
    '#default_value' => '',
    '#description' => t('The name is used for referencing video styles. Use only lowercase alphanumeric characters, underscores (_), and hyphens (-).'),
    '#element_validate' => array('video_embed_field_video_style_name_validate'),
    '#required' => TRUE,
  );
  
  //Grab the settings off the parser form
  $parser_form = video_embed_field_get_form(array());
  
  //General settings for playback - formerly in the configuration section
  $form['settings'] = array(
    '#type' => 'vertical_tabs',
    '#title' => t('Playback settings'),
  ) + $parser_form; //add in our extra settings
  
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Create new style'),
  );
  
  return $form;
}

/**
 * Submit handler for adding a new image style.
 */
function video_embed_field_video_style_add_form_submit($form, &$form_state) {
  $style = array('name' => $form_state['values']['name']);
  $style['data'] = _video_embed_field_get_settings($form_state);
  
  $style = video_embed_field_video_style_save($style);
  drupal_set_message(t('Style %name was created.', array('%name' => $style['name'])));
  $form_state['redirect'] = 'admin/config/media/video-embed-field' ;
}

/**
 * Element validate function to ensure unique, URL safe style names.
 */
function video_embed_field_video_style_name_validate($element, $form_state) {
  // Check for duplicates.
  $styles = video_embed_field_video_styles();
  if (isset($styles[$element['#value']]) && (!isset($form_state['video_style']['vsid']) || $styles[$element['#value']]['vsid'] != $form_state['video_style']['vsid'])) {
    form_set_error($element['#name'], t('The video style name %name is already in use.', array('%name' => $element['#value'])));
  }
  
  // Check for illegal characters in image style names.
  if (preg_match('/[^0-9a-z_\-]/', $element['#value'])) {
    form_set_error($element['#name'], t('Please only use lowercase alphanumeric characters, underscores (_), and hyphens (-) for style names.'));
  }
}

/**
 * Returns HTML for the page containing the list of image styles.
 *
 * @param $variables
 *   An associative array containing:
 *   - styles: An array of all the video styles returned by video_embed_field_video_get_styles().
 *
 * @see video_embed_field_video_get_styles()
 * @ingroup themeable
 */
function theme_video_embed_field_video_style_list($variables) {
  $styles = $variables['styles'];
  
  $header = array(t('Style name'), t('Settings'), array('data' => t('Operations'), 'colspan' => 3));
  $rows = array();
  foreach ($styles as $style) {
    $row = array();
    $row[] = l($style['name'], 'admin/config/media/video-embed-field/edit/' . $style['name']);
    $link_attributes = array(
      'attributes' => array(
        'class' => array('video-style-link'),
      ),
    );
    if ($style['storage'] == VIDEO_EMBED_FIELD_VIDEO_STORAGE_NORMAL) {
      $row[] = t('Custom');
      $row[] = l(t('edit'), 'admin/config/media/video-embed-field/edit/' . $style['name'], $link_attributes);
      $row[] = l(t('delete'), 'admin/config/media/video-embed-field/delete/' . $style['name'], $link_attributes);
    }
    elseif ($style['storage'] == VIDEO_EMBED_FIELD_VIDEO_STORAGE_OVERRIDE) {
      $row[] = t('Overridden');
      $row[] = l(t('edit'), 'admin/config/media/video-embed-field/edit/' . $style['name'], $link_attributes);
      $row[] = l(t('revert'), 'admin/config/media/video-embed-field/revert/' . $style['name'], $link_attributes);
    }
    else {
      $row[] = t('Default');
      $row[] = l(t('edit'), 'admin/config/media/video-embed-field/edit/' . $style['name'], $link_attributes);
      $row[] = '';
    }
    $rows[] = $row;
  }

  if (empty($rows)) {
    $rows[] = array(array(
      'colspan' => 4,
      'data' => t('There are currently no styles. <a href="!url">Add a new one</a>.', array('!url' => url('admin/config/media/video-embed-field/add'))),
    ));
  }

  return theme('table', array('header' => $header, 'rows' => $rows));
}

/**
 * Form builder; Form for deleting a video style.
 *
 * @param $style
 *   A video style array.
 *
 * @ingroup forms
 * @see video_embed_field_video_style_delete_form_submit()
 */
function video_embed_field_video_style_delete_form($form, $form_state, $style) {
  $form_state['video_style'] = $style;
  
  $replacement_styles = array_diff_key(video_embed_field_video_style_options(), array($style['name'] => ''));
  $form['replacement'] = array(
    '#title' => t('Replacement style'),
    '#type' => 'select',
    '#options' => $replacement_styles,
    '#empty_option' => t('No replacement, just delete'),
  );
  
  return confirm_form(
    $form,
    t('Optionally select a style before deleting %style', array('%style' => $style['name'])),
    'admin/config/media/video-embed-field',
    t('If this style is in use on the site, you may select another style to replace it.'),
    t('Delete'),  t('Cancel')
  );
}

/**
 * Submit handler to delete an image style.
 */
function video_embed_field_video_style_delete_form_submit($form, &$form_state) {
  $style = $form_state['video_style'];

  video_embed_field_video_style_delete($style, $form_state['values']['replacement']);
  drupal_set_message(t('Style %name was deleted.', array('%name' => $style['name'])));
  $form_state['redirect'] = 'admin/config/media/video-embed-field';
}

/**
 * Confirmation form to revert a database style to its default.
 */
function video_embed_field_video_style_revert_form($form, $form_state, $style) {
  $form_state['video_style'] = $style;

  return confirm_form(
    $form,
    t('Revert the %style style?', array('%style' => $style['name'])),
    'admin/config/media/video-embed-field',
    t('Reverting this style will delete the customized settings and restore the defaults provided by the @module module.', array('@module' => $style['module'])),
    t('Revert'),  t('Cancel')
  );
}

/**
 * Submit handler to convert an overridden style to its default.
 */
function video_embed_field_video_style_revert_form_submit($form, &$form_state) {
  $style = $form_state['video_style'];
  
  video_embed_field_default_style_revert($style);
  drupal_set_message(t('The %name style has been revert to its defaults.', array('%name' => $style['name'])));
  $form_state['redirect'] = 'admin/config/media/video-embed-field';
}