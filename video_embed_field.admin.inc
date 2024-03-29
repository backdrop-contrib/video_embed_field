<?php

/**
 * @file
 * Form builder; Form for editing a video style.
 *
 */

/**
 * Video embed style form handler.
 */
function video_embed_field_video_style_form(&$form, &$form_state) {
  if (isset($form_state['item'])) {
    $style = (array) $form_state['item'];
  }
  else {
    $style = array();
  }
  $form_state['video_style'] = $style;

  // Grab the settings off the parser form.
  $values = isset($style['data']) ? $style['data'] : array();
  $parser_form = video_embed_field_get_form($values);

  // General settings for playback - formerly in the configuration section.
  $form['data'] = array(
    '#type' => 'vertical_tabs',
    '#title' => t('Playback settings'),
    '#tree' => TRUE,
  ) + $parser_form;

  return $form;
}

/**
 * VEF settings page form callback.
 */
function video_embed_field_settings_form($form, &$form_state) {
  $form['#config'] = 'video_embed_field.settings';
  $form['youtube_v3_api_key'] = array(
    '#type' => 'textfield',
    '#title' => t('Youtube v3 API key'),
    '#default_value' => config_get('video_embed_field.settings', 'youtube_v3_api_key'),
  );
  return system_settings_form($form);
}
