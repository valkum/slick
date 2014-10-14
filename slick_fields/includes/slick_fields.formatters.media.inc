<?php

/**
 * @file
 * Fields formatter for the Slick fields Media integration.
 */

/**
 * Format image/media file data.
 */
function _slick_fields_format_media(&$settings, $items, $field, $view_mode, $langcode, $thumb_nav = FALSE) {
  $build = array();
  $css = array();

  $slick_id = $settings['attributes']['id'];
  $asnavfor = 'main';
  
  foreach ($items as $key => $item) {
    $slide = array(
      'item' => $item,
      'slide' => array(),
      'caption' => array(),
    );

    $slide_type = isset($slide['item']['type']) ? $slide['item']['type'] : $field['type'];

    // Setup the variables for the image.
    $image_style     = $settings['image_style'];
    $fid             = $slide['item']['fid'];
    $image['uri']    = $slide['item']['uri'];
    $image['width']  = isset($slide['item']['metadata']['width']) ? $slide['item']['metadata']['width'] : NULL;
    $image['height'] = isset($slide['item']['metadata']['height']) ? $slide['item']['metadata']['height'] : NULL;
    $image['alt']    = $slide['item']['alt'];
    $image['title']  = $slide['item']['title'];

    $settings['has_media']      = TRUE;
    $settings['is_media']       = FALSE;
    $settings['media_height']   = $image['height'];
    $settings['media_width']    = $image['width'];

    $file = (object) $slide['item'];
    $uri = $file->uri;

    // If a file has an invalid type, allow file_view_file() to work.
    if (!file_type_is_enabled($file->type)) {
      $file->type = file_get_type($file);
    }

    $media = file_view_file($file, $view_mode, $langcode);

    if ($media && isset($media['#uri'])) {
      $settings['is_media'] = TRUE;
      if (isset($media['#options'])) {
        $settings['media_height']   = $media['#options']['height'] ?: 640;
        $settings['media_width']    = $media['#options']['width'] ?: 360;
      }
      $settings['media_uri']      = $media['#uri'];
      $settings['media_fid']      = $file->fid;
      $settings['media_filename'] = $file->filename;
    }
    $scheme = file_uri_scheme($uri);
    $image_style = isset($media['#style_name']) ? $media['#style_name'] : $image_style;

    $settings['image_style'] = $image_style;
    $settings['scheme']      = $scheme;
    $settings['type']        = $slide_type;

    // Get audio/video thumbnail uri.
    if ($media_image_uri = _slick_fields_get_media_thumbnail($settings, $uri, $image_style)) {
      $image['uri'] = $media_image_uri;
    }

    // Provide thumbnail pagers if so configured.
    $thumbnail = '';
    $thumbnail_hover = '';

    if (isset($settings['thumbnail_style']) && $settings['thumbnail_style']) {
      $image['path'] = $image['uri'];
      $image['style_name'] = $settings['thumbnail_style'];

      $thumbnail = theme('image_style', $image);

      if ($settings['thumbnail_hover']) {
        $thumbnail_hover = '<div class="slide__thumbnail--placeholder"><div class="slide__thumbnail">' . $thumbnail . '</div>';
        $thumbnail_hover .= '<button type="button" data-role="none">' . ($key + 1) . '</button></div>';
      }
    }

    if (!$thumb_nav) {
      // Title, if so configured.
      if (!empty($settings['slide_title'])) {
        if ($field_title = field_get_items('file', $file, $settings['slide_title'], $langcode)) {
          $title = $field_title[0]['safe_value'];
          $slide['caption']['title'] = $title;
        }
      }

      // Other caption fields, if so configured.
      if (!empty($settings['slide_caption'])) {
        $caption_items = array();
        foreach ($settings['slide_caption'] as $i => $caption_field) {
          $caption = field_view_field('file', $file, $caption_field, $view_mode, $langcode);
          if ($caption) {
            $caption_items[$i] = $caption;
          }
        }
        $slide['caption']['data'] = $caption_items;
      }

      // Link, if so configured.
      if (!empty($settings['slide_link'])) {
        if ($field_link = field_get_items('file', $file, $settings['slide_link'], $langcode)) {
          $link = field_view_field('file', $file, $settings['slide_link'], $view_mode, $langcode);
          $slide['caption']['link'] = $link;
        }
      }

      // Layout, if so configured.
      if (!empty($settings['slide_layout'])) {
        if ($field_layout = field_get_items('file', $file, $settings['slide_layout'], $langcode)) {
          $layout = $field_layout[0]['value'];
          $slide['caption']['layout'] = check_plain($layout);
        }
      }

      $media_url = '';
      if ($settings['is_media']) {
        $rendered_media = render($media);
        $media_url = slick_get_media_url($rendered_media);
        $settings['url'] = $media_url;
      }

      // Image with picture and colorbox supports.
      $image_slide = _slick_fields_get_picture($settings, $image, $key, $media_url);

      // Thumbnail static.
      if ($settings['dots_main'] && $thumbnail_hover) {
        $image_slide .= $thumbnail_hover;
      }
    }
    else {
      $image_slide = $thumbnail;
      $settings['type'] = 'image';
      $asnavfor = 'thumbnail';

      // Thumbnail static.
      if ($settings['dots_thumbnail'] && $thumbnail_hover) {
        $image_slide .= $thumbnail_hover;
      }
    }

    // Pass it over theme_slick_media.
    $slide['slide'] = array(
      '#theme' => 'slick_media',
      '#item' => $image_slide,
      '#settings' => $settings,
      '#asnavfor' => $asnavfor,
    );

    // If has Media fields.
    if ($settings['is_media'] && !$thumb_nav) {
      $slide['slide']['#media'] = $media;
    }

    // @todo map from picture, or refine for mobile.
    // @todo refine for video/audio.
    if (in_array($settings['skin'], array('fullscreen', 'parallax'))) {
      $slide['slide'] = array();
      $fullscreen_image_url = image_style_url($settings['image_style'], $image['uri']);
      $css[] = "#{$slick_id} .slide--{$key} {background-image: url('{$fullscreen_image_url}')}";
    }

    $build[$key] = $slide;
  }

  return $build;
}
