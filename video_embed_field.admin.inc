<?php

/**
 * Form builder; Form for editing a video style.
 * Used by CTools export ui
 *
 * @ingroup forms
 * @see video_embed_field_video_style_form_submit()
 */
function video_embed_field_video_style_form(&$form, &$form_state) {
  if(isset($form_state['item'])){
    $style = (array) $form_state['item'];
  } else {
    $style = array();
  }
  $form_state['video_style'] = $style;

  //Grab the settings off the parser form
  $values = isset($style['data']) ? $style['data'] : array();
  $parser_form = video_embed_field_get_form($values);

  //General settings for playback - formerly in the configuration section
  $form['data'] = array(
    '#type' => 'vertical_tabs',
    '#title' => t('Playback settings'),
  ) + $parser_form; //add in our extra settings

  return $form;
}

/**
 * Submit handler for adding a new image style.
 */
function video_embed_field_video_style_form_submit($form, &$form_state) {
  //for some reason drupal doesn't want to group the stuff in the vertical tabs inside of values
  $data = array();
  foreach (array_keys($form['data']) as $key) {
    if (strpos($key, '#') !== 0 && isset($form_state['values'][$key])) {
      $data[$key] = $form_state['values'][$key];
    }
  }
  $form_state['item']->data = $data;
}