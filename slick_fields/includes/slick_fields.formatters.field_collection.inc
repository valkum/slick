<?php

/**
 * @file
 * Fields formatter for the Slick fields Field collection integration.
 */

/**
 * Format field collection data.
 */
function _slick_fields_format_field_collection(&$settings, $items, $field, $view_mode, $langcode) {
  $build = array();
  $field_type = $field['type'];
  $image_style = $settings['image_style'];

  foreach ($items as $key => $item) {
    $tmp = $item;
    $item = array(
      'item' => $tmp,
      'slide' => array(),
      'caption' => array(),
    );

    if ($field_collection = field_collection_field_get_entity($item['item'])) {
      $field_collection_view = $field_collection->view($view_mode);
      $field_collection = (array) $field_collection;

      // Get field collection ID.
      $id = array_keys($field_collection_view['field_collection_item']);
      $id = reset($id);
      $fc_fields = $field_collection_view['field_collection_item'][$id];

      // Manually insert type for theme to get the context.
      $item['item']['type'] = 'field_collection';
      $settings['type'] = $item['item']['type'];

      // Customize display to tame Field collection.
      // Main background dimage.
      if ($settings['slide_image']) {
        if (isset($fc_fields[$settings['slide_image']])) {
          $slide_image = $fc_fields[$settings['slide_image']];
          $field_type  = $slide_image['#field_type'];

          // It is un-reasonable to make large image as colorbox, so disable it.
          $settings['colorbox_ready'] = FALSE;
          $settings['picture_ready']  = TRUE;
          switch ($field_type) {
            case 'file':
              // Large background video is not supported, any taker?
              $file            = $slide_image[0]['file'];
              $fallback        = $slide_image['#items'][0];
              $image_style     = isset($file['#style_name']) ? $file['#style_name'] : $image_style;
              $image['uri']    = isset($file['#path']) ? $file['#path'] : $fallback['uri'];
              $image['width']  = isset($file['#width']) ? $file['#width'] : NULL;
              $image['height'] = isset($file['#height']) ? $file['#height'] : NULL;
              $image['alt']    = isset($file['#alt']) ? $file['#alt'] : $fallback['alt'];
              $image['title']  = isset($file['#title']) ? $file['#title'] : $fallback['title'];

              break;

            case 'image':
              $file            = $slide_image[0]['#item'];
              $image_style     = isset($file['#image_style']) ? $file['#image_style'] : $image_style;
              $image['uri']    = $file['uri'];
              $image['width']  = $file['width'];
              $image['height'] = $file['height'];
              $image['alt']    = $file['alt'];
              $image['title']  = $file['title'];

              break;
          }

          $settings['type']         = $field_type;
          $settings['media_height'] = $image['height'];
          $settings['media_width']  = $image['width'];

          // Image with picture supports.
          $item['slide'] = _slick_fields_get_picture($settings, $image, $key);

          // Provide thumbnail pagers if so configured.
          if (!empty($settings['thumbnail_style'])) {
            $image['path'] = $image['uri'];
            $image['style_name'] = $settings['thumbnail_style'];

            $thumbnail = theme('image_style', $image);

            if ($settings['thumbnail_hover']) {
              $item['slide'] .= '<div class="slide__thumbnail slide__thumbnail--hover"><div class="slide__thumbnail__box">' . $thumbnail . '</div>';
              $item['slide'] .= '<button type="button">' . ($key + 1) . '</button></div>';
            }
            else {
              $item['slide'] .= '<div class="slide__thumbnail">' . $thumbnail . '</div>';
            }
          }
        }
      }

      // Overlay image/video.
      if ($settings['slide_overlay']) {
        if (isset($fc_fields[$settings['slide_overlay']])) {
          $settings['has_overlay']    = TRUE;
          $settings['is_media']       = FALSE;
          $settings['colorbox_ready'] = TRUE;
          $settings['picture_ready']  = FALSE;
          $slide_overlay              = $fc_fields[$settings['slide_overlay']];
          $field_type                 = $slide_overlay['#field_type'];

          $media = '';
          switch ($field_type) {
            case 'image':
              $file              = $slide_overlay[0]['#item'];
              $image_style       = isset($file['#image_style']) ? $file['#image_style'] : $image_style;
              $overlay['uri']    = $file['uri'];
              $overlay['width']  = $file['width'];
              $overlay['height'] = $file['height'];
              $overlay['alt']    = $file['alt'];
              $overlay['title']  = $file['title'];
              $settings['type']  = 'image';

              break;

            case 'file':
              $fid        = $slide_overlay['#items'][0]['fid'];
              $file       = $slide_overlay['#items'][0];
              $item_type  = $file['type'];
              $file_style = $slide_overlay[0]['file'];

              $image_style       = isset($file_style['#style_name']) ? $file_style['#style_name'] : $image_style;
              $overlay['uri']    = $file['uri'];
              $overlay['width']  = isset($file_style['#width']) ? $file_style['#width'] : NULL;
              $overlay['height'] = isset($file_style['#height']) ? $file_style['#height'] : NULL;
              $overlay['alt']    = $file['alt'];
              $overlay['title']  = $file['title'];

              $settings['media_height'] = $overlay['height'];
              $settings['media_width']  = $overlay['width'];

              // Make object for file_view_file().
              $file = (object) $file;
              $uri = $file->uri;
              // If a file has an invalid type, allow file_view_file() to work.
              if (!file_type_is_enabled($file->type)) {
                $file->type = file_get_type($file);
              }

              $media  = file_view_file($file, $view_mode, $langcode);
              $scheme = file_uri_scheme($uri);

              $settings['scheme']         = $scheme;
              $settings['type']           = $item_type;
              $settings['has_media']      = TRUE;
              $settings['media_uri']      = $uri;
              $settings['media_fid']      = $file->fid;
              $settings['media_filename'] = $file->filename;

              // Get audio/video thumbnail uri.
              if ($media_image_uri = _slick_fields_get_media_thumbnail($settings, $uri, $image_style)) {
                $overlay['uri'] = $media_image_uri;
              }

              break;
          }

          $settings['image_style'] = $image_style;

          // If has Media fields.
          $media_url = '';
          if ($media && isset($media['#uri'])) {
            $wrapper = file_stream_wrapper_get_instance_by_uri($media['#uri']);
            $settings['is_media'] = TRUE;
            if (isset($media['#options'])) {
              $settings['media_height'] = $media['#options']['height'] ?: 640;
              $settings['media_width']  = $media['#options']['width'] ?: 360;
            }

            $rendered_media = render($media);
            $safe = TRUE;
            if ($settings['scheme'] == 'soundcloud') {
              $safe = FALSE;
              $soundcloud_url = $wrapper->interpolateUrl();
              // Prevent fatal error for no internet connection.
              if ($embed_data = $wrapper->getOEmbed($soundcloud_url)) {
                $settings['soundcloud'] = TRUE;
                $safe = TRUE;
              }
            }

            if ($safe) {
              $media_url = slick_get_media_url($rendered_media);
              $settings['url'] = $media_url;
            }
          }

          // Image with picture supports, but disabled for small overlay, since
          // we don't provide UI to manage small pictures, and picture is only
          // reasonale for large images.
          $overlay_item = _slick_fields_get_picture($settings, $overlay, $key, $media_url);

          // Pass it over to theme_slick_media.
          $item['caption']['overlay'] = array(
            '#theme' => 'slick_media',
            '#item' => $overlay_item,
            '#settings' => $settings,
          );

          // If has Media fields.
          if ($settings['is_media']) {
            $item['caption']['overlay']['#media'] = $media;
          }
        }
      }

      // Title, if so configured.
      if (!empty($settings['slide_title'])) {
        $layout = $field_collection[$settings['slide_title']][$langcode][0]['safe_value'];
        $item['caption']['title'] = $layout;
      }

      // Other caption fields, if so configured.
      if (!empty($settings['slide_caption'])) {
        $caption_items = array();
        foreach ($settings['slide_caption'] as $i => $caption_field) {
          if (isset($field_collection_view['field_collection_item'][$id][$caption_field])) {
            $caption_items[$i] = $field_collection_view['field_collection_item'][$id][$caption_field];
          }
        }

        if ($caption_items) {
          $item['caption']['data'] = $caption_items;
        }
      }

      // Link, if so configured.
      if (!empty($settings['slide_link'])) {
        if (isset($field_collection_view['field_collection_item'][$id][$settings['slide_link']])) {
          $item['caption']['link'] = $field_collection_view['field_collection_item'][$id][$settings['slide_link']];
        }
      }

      // Layout, if so configured.
      if (!empty($settings['slide_layout'])) {
        $layout = $field_collection[$settings['slide_layout']][$langcode][0]['value'];
        $item['caption']['layout'] = check_plain($layout);
      }

      $item['settings'] = $settings;

      $build[$key] = $item;
    }
  }

  return $build;
}
