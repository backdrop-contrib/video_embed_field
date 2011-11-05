<?php

/**
 * @file
 * Implement a video field.
 */

/**
 * Implements of hook_field_info().
 */
function video_embed_field_field_info() {
  return array(
    'video_embed_field' => array(
      'label' => 'Video Embed',
      'description' => 'Embed videos from youtube or vimeo',
      'settings' => array(),
      'instance_settings' => array(
        'description_field' => 0,
      ),
      'default_widget' => 'video_embed_field_video',
      'default_formatter' => 'video_embed_field',
    )
  );
}

/**
 * Implements hook_field_instance_settings_form().
 */
function video_embed_field_field_instance_settings_form($field, $instance) {
  $settings = $instance['settings'];
  
  $form['description_field'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable <em>Description</em> field'),
    '#default_value' => isset($settings['description_field']) ? $settings['description_field'] : '',
    '#description' => t('The description field allows users to enter a description about the video.'),
    '#parents' => array('instance', 'settings', 'description_field'),
    '#weight' => 11,
  );
  
  return $form;
}

/**
 * Implements hook_field_widget_info().
 */
function video_embed_field_field_widget_info() {
  return array(
    'video_embed_field_video' => array(
      'label' => 'Video',
      'description' => 'Provides a video embed field',
      'field types' => array('video_embed_field'),
      'settings' => array(),
      'behaviors' => array(
        'multiple values' => FIELD_BEHAVIOR_DEFAULT,
        'default value' => FIELD_BEHAVIOR_DEFAULT
      ),
    ),
  );
}

/**
 * Implements hook_field_widget_form().
 */
function video_embed_field_field_widget_form(&$form, &$form_state, $field, $instance, $langcode, $items, $delta, $element) {
  // Don't need to check the type right now because we're only defining one
  $element += array(
    '#type' => 'video_embed_field_widget'
  );
  
  $element['video_url'] = array(
    '#type' => 'textfield',
    '#title' => t('URL'),
    '#description' => $element['#description'],
    '#attributes' => array('class' => array('video_embed_url')),
    '#attached' => array( 
      'css' => array( 
        drupal_get_path('module', 'video_embed_field') .'/video_embed_field.form.css'
      )
    ),
    '#default_value' => isset($items[$delta]['video_url'])?$items[$delta]['video_url']:'',
    '#required' => $element['#required']
  );
  
  // Add the description field if enabled.
  if (!empty($instance['settings']['description_field'])) {
    $element['description'] = array(
      '#type' => 'textfield',
      '#title' => t('Description'),
      '#default_value' => isset($items[$delta]['description']) ? $items[$delta]['description'] : '',
      '#description' => t('The description which may be used as a label.'),
    );
  }
  
  return $element;
}

/**
 * Validates video URL.
 */
function video_embed_field_field_validate($entity_type, $entity, $field, $instance, $langcode, $items, &$errors) {
  foreach ($items as $delta => $item) {
    if (!empty($item['video_url'])) {
      $item['video_url'] = trim($item['video_url']);
      if (stristr($item['video_url'], '.') && !stristr($item['video_url'], 'http://') && !stristr($item['video_url'], 'https://')) {
        $item['video_url'] = 'http://' . $item['video_url'];
      }
      
      $parts = parse_url($item['video_url']);
      if (!$parts || !isset($parts['host'])) {
        $errors[$field['field_name']][$langcode][$delta][] = array(
          'error' => t('Invalid Url'), 
          'message' => t('Video: Invalid Video URL.', array('%name' => $instance['label'])),
        );
      }
      else {
        $host = $parts['host'];
        if (stripos($host, 'www.') > -1) {
          $host = substr($host, 4);
        }
        $domains = _video_embed_field_get_provider_domains();
        if (!array_key_exists($host, $domains)) {
          $errors[$field['field_name']][$langcode][$delta][] = array(
            'error' => t('Unsupported Video Provider'), 
            'message' => t('%name: This video provider is not currently supported.', array('%name' => $instance['label'])),
          );
        }
      }
    }
  }
}

/**
 * Implements of hook_field_is_empty().
 */
function video_embed_field_field_is_empty($item, $field){
  return empty($item) || empty($item['video_url']) || $item['video_url'] === '';
}

/**
 * Implements hook_field_formatter_info().
 */
function video_embed_field_field_formatter_info() {
  return array(
    'video_embed_field' => array(
      'label' => t('Video Player'),
      'field types' => array('video_embed_field'),
      'settings' => array('video_style' => 'normal', 'description' => 1),
    ),
  );
}

/**
 * Implements hook_field_formatter_settings_form().
 */
function video_embed_field_field_formatter_settings_form($field, $instance, $view_mode, $form, &$form_state) {
  $display = $instance['display'][$view_mode];
  $settings = $display['settings'];
  
  $video_styles = video_embed_field_video_style_options(FALSE);
  $element['video_style'] = array(
    '#title' => t('Video style'),
    '#type' => 'select',
    '#default_value' => $settings['video_style'],
    '#options' => $video_styles,
  );
  
  if ($instance['settings']['description_field']) {
    $element['description'] = array(
      '#title' => t('Show description'),
      '#type' => 'select',
      '#default_value' => $settings['description'],
      '#options' => array(1 => t('Yes'), 0 => t('No')),
    );
  }
  
  return $element;
}

/**
 * Implements hook_field_formatter_settings_summary().
 */
function video_embed_field_field_formatter_settings_summary($field, $instance, $view_mode) {
  $display = $instance['display'][$view_mode];
  $settings = $display['settings'];
  
  $summary = array();
  
  $video_styles = video_embed_field_video_style_options(FALSE);
  // Styles could be lost because of enabled/disabled modules that defines
  // their styles in code.
  if (isset($video_styles[$settings['video_style']])) {
    $summary[] = t('Video style: @style', array('@style' => $video_styles[$settings['video_style']]));
  }
  
  if ($settings['description'] && $instance['settings']['description_field']) {
    $summary[] = t('Show description');
  }
  
  return implode('<br />', $summary);
}

/**
 * Implements hook_field_widget_form().
 */
function video_embed_field_field_formatter_view($entity_type, $entity, $field, $instance, $langcode, $items, $display){
  $element = array();
  
  foreach ($items as $delta => $item) {
    if (isset($item['description']) && $item['description'] && $display['settings']['description'] && $instance['settings']['description_field']) {
      $description = array(
        '#prefix' => '<div class="video-embed-description">',
        '#markup' => $item['description'],
        '#suffix' => '</div>',
      );
    }
    else {
      $description = array();
    }
    
    $element[$delta] = array(
      array(
        '#theme' => 'video_embed_field_embed_code',
        '#url' => $item['video_url'],
        '#style' => $display['settings']['video_style'],
      ),
      $description,
    );
  }
  
  return $element;
}

/**
 * Implements hook_requirements().
 * Warn users if they are using the default settings because they haven't updated their fields
 */
function video_embed_field_requirements($phase) {
  if ($phase == 'runtime') {
    $fields = array_filter(field_info_fields(), '_video_embed_field_array_filter');
    
    $errors = array();
    foreach ($fields as $field_name => $field) {
      $instances = field_read_instances(array('field_name' => $field_name));
      
      foreach ($instances as $key => $instance) {
        if (!isset($instance['settings']['playbacksettings'])) {
          $errors[] = $instance['field_name'] . ' on ' . $instance['entity_type'] . ' type ' . $instance['bundle'];
        }
      }
    }
    
    if (!empty($errors)) {
      $requirements = array(
        'video_embed_field' => array(
          'description' =>  t('Some video_embed_fields are using the old settings format.  You will need to go set the preferences for each field manually. '),
          'severity' => REQUIREMENT_ERROR,
          'title' => t('Video Embed Field settings:'),
          'value' => implode(', ', $errors),
        ),
      );
      return $requirements;
    }
  }
}