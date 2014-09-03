<?php

/**
 * @file
 * Fields formatter for Slick fields core Image field integration.
 */

/**
 * Format core image data.
 */
function _slick_fields_format_image(&$settings, $items, $field, $thumb_nav = FALSE) {
  $build = array();
  $field_type = $field['type'];
  $asnavfor = 'main';
  foreach ($items as $key => $item) {
    $tmp = $item;
    $item = array(
      'item' => $tmp,
      'slide' => array(),
      'caption' => array(),
    );

    // Once media.module enabled, it populates type.
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

    $thumbnail = '';
    $thumbnail_hover = '';
    $image_slide = '';

    if (isset($settings['thumbnail_style']) && $settings['thumbnail_style']) {
      $image['path'] = $image['uri'];
      $image['style_name'] = $settings['thumbnail_style'];

      $thumbnail = theme('image_style', $image);

      if ($settings['thumbnail_hover']) {
        $thumbnail_hover = '<div class="slide__thumbnail slide__thumbnail--hover"><div class="slide__thumbnail__box">' . $thumbnail . '</div>';
        $thumbnail_hover .= '<button type="button" data-role="none">' . ($key + 1) . '</button></div>';
      }
    }

    if (!$thumb_nav) {
      // Slide captions.
      if (!empty($settings['slide_caption'])) {
        if (!empty($image['title'])) {
          $item['caption']['title']['#markup'] = filter_xss($image['title']);
        }
        if (!empty($image['alt'])) {
          $item['caption']['alt']['#markup'] = filter_xss($image['alt']);
        }
      }

      // Slide layouts.
      if (!empty($settings['slide_layout'])) {
        $item['caption']['layout'] = check_plain($settings['slide_layout']);
      }

      // Image with picture and colorbox supports.
      $image_slide = _slick_fields_get_picture($settings, $image, $key);

      // Thumbnail static.
      if ($settings['dots_main'] && $thumbnail_hover) {
        $image_slide .= $thumbnail_hover;
      }
    }
    else {
      // Thumbnail slider if so configured.
      $image_slide = $thumbnail;
      $settings['type'] = 'image';
      $asnavfor = 'thumbnail';

      // Thumbnail static.
      if ($settings['dots_thumbnail'] && $thumbnail_hover) {
        $image_slide .= $thumbnail_hover;
      }
    }

    // Pass it over to theme_slick_media.
    $item['slide'] = array(
      '#theme' => 'slick_media',
      '#item' => $image_slide,
      '#settings' => $settings,
      '#asnavfor' => $asnavfor,
    );

    $build[$key] = $item;
  }

  return $build;
}
