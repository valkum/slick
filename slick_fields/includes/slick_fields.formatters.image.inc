<?php

/**
 * @file
 * Fields formatter for Slick fields core Image field integration.
 */

/**
 * Format core image data.
 */
function _slick_fields_format_image(&$settings, $items, $field, $view_mode, $langcode) {
  $build = array();
  $media = '';
  $field_type = $field['type'];
  foreach ($items as $key => $item) {
    $tmp = $item;
    $item = array(
      'item' => $tmp,
      'slide' => array(),
      'caption' => array(),
    );

    $item_type = isset($item['item']['type']) ? $item['item']['type'] : $field_type;
    $settings['type'] = $item_type;

    // Setup the variables.
    $image['uri']    = $item['item']['uri'];
    $image['width']  = isset($item['item']['width']) ? $item['item']['width'] : NULL;
    $image['height'] = isset($item['item']['height']) ? $item['item']['height'] : NULL;
    $image['alt']    = $item['item']['alt'];
    $image['title']  = $item['item']['title'];

    $settings['media_height'] = $image['height'];
    $settings['media_width']  = $image['width'];

    // Slide captions.
    if (!empty($settings['slide_caption'])) {
      $item['caption']['title']['#markup'] = !empty($image['title']) ? filter_xss($image['title']) : '';
      $item['caption']['alt']['#markup'] = !empty($image['alt']) ? filter_xss($image['alt']) : '';
    }

    // Slide layouts.
    if (!empty($settings['slide_layout'])) {
      $item['caption']['layout'] = check_plain($settings['slide_layout']);
    }

    // Image with picture supports.
    $image_slide = _slick_fields_get_picture($settings, $image, $key);

    // Pass it over to theme_slick_media.
    $item['slide'] = array(
      '#theme' => 'slick_media',
      '#item' => $image_slide,
      '#settings' => $settings,
    );

    // Provide thumbnail pagers if so configured.
    if (!empty($settings['thumbnail_style'])) {
      $image['path'] = $image['uri'];
      $image['style_name'] = $settings['thumbnail_style'];
      $item['thumbnail'] = theme('image_style', $image);
      if ($settings['thumbnail_hover']) {
        $item['thumbnail'] = '<div class="slide__thumbnail__box">' . $item['thumbnail'] . '</div>';
        $item['thumbnail'] .= '<button type="button">' . ($key + 1) . '</button>';
      }
    }

    $item['settings'] = $settings;

    $build[$key] = $item;
  }

  return $build;
}
