<?php

/**
 * @file
 * Hooks and preprocess functions for the slick themes module.
 */

/**
 * Implements hook_preprocess_HOOK().
 *
 * Elements:
 * - #settings is set via sub-modules and serves various purposes, and not
 *   related to JS settings, mostly slide layouts or attaching assets.
 * - #options is set programmatically, or hand-crafted, and only accepts direct
 *   key|value pairs related to JS settings, or at least optionset name.
 */
function template_preprocess_slick(&$variables) {

  $element = $variables['element'];
  $items   = $element['#items'];
  $count   = count($items);

  // HTML or layout related settings, none of JS settings, except the new
  // custom mousewheel.
  $settings  = array(
    'optionset'            => '',
    'optionset_thumbnail'  => '',
    'skin'                 => '',
    'skin_thumbnail'       => '',
    'media_switch'         => FALSE,
    'is_carousel'          => FALSE,
    'has_pattern'          => FALSE,
    'has_arrow_down'       => FALSE,
    'mousewheel'           => FALSE,
    'arrows'               => FALSE,
    'aspect_ratio_overlay' => FALSE,
    'current_display'      => isset($settings['current_display']) && $settings['current_display'] ? $settings['current_display'] : 'main',
  );

  $settings  = isset($element['#settings']) ? array_merge($settings, $element['#settings']) : $settings;
  $customs   = isset($element['#options']) ? $element['#options'] : array();

  // Optionset may be defined via custom code, CTools UI, or sub-modules.
  $optionset = isset($customs['optionset']) ? $customs['optionset'] : $settings['optionset'];

  // Load the selected optionset.
  if (!empty($optionset)) {
    $optionset = slick_optionset_load($optionset);
  }

  // Check if an optionset was loaded.
  if (empty($optionset)) {
    // Fall back to 'default' option set.
    $optionset = slick_optionset_load('default');
    watchdog('slick', 'Fallback to default optionset.', array(), WATCHDOG_WARNING);
  }

  $options = $optionset->options;
  $general = $options['general'];
  $goodies = $general['goodies'];
  $optionset_name = $optionset->name;
  $js_settings = array(
    'centerMode'   => FALSE,
    'slidesToShow' => 1,
    'rtl' => FALSE,
  );

  // The Slick JS settings defined either via custom #options, or CTools UI.
  $js_settings = $customs ? array_merge($options['settings'], $customs) : $options['settings'];
  // Anon DIV is problematic, since Slick will slide every div element within
  // its container which makes it impossible to add a div to wrap buttons (such
  // as arrow down) nor wrap them for advanced customPaging. Hence we override
  // with more specific class .slick__slide instead.
  // Note: Having list type is irrelevant to the JS generated markups.
  $js_settings['slide'] = $js_settings['slide'] == 'div' ? '.slick__slide' : $js_settings['slide'];

  // Field or Views options override the optionset ones.
  $settings['has_arrow_down'] = $settings['has_arrow_down'] ? TRUE : isset($goodies['arrow-down']) && $goodies['arrow-down'];
  $settings['has_pattern'] = $settings['has_pattern'] ? TRUE : isset($goodies['pattern']) && $goodies['pattern'];

  // Only add asNavFor if we do have thumbnail optionset.
  if (isset($settings['optionset_thumbnail']) && $settings['optionset_thumbnail']) {
    if ($settings['current_display'] == 'main') {
      if (isset($settings['asnavfor_main']) && $settings['asnavfor_main']) {
        $js_settings['asNavFor'] = check_plain($settings['asnavfor_main']);
      }
    }
    elseif ($settings['current_display'] == 'thumbnail') {
      if (isset($settings['asnavfor_thumbnail']) && $settings['asnavfor_thumbnail']) {
        $js_settings['asNavFor'] = check_plain($settings['asnavfor_thumbnail']);
      }
    }
  }

  // Configure attributes for containing elements.
  $attributes = $variables['attributes_array'];
  $attributes['class'] = $variables['classes_array'];

  // Merge with defined attributes.
  if (isset($settings['attributes']) && is_array($settings['attributes'])) {
    $attributes += $settings['attributes'];
  }

  // Set the ID for each slick instance, if none provided, for quick JS lookup.
  if (empty($attributes['id'])) {
    $slick_id = &drupal_static('slick_id', 0);
    $id = 'slick-' . ++$slick_id;
    $attributes['id'] = $id;
    if ($settings['current_display'] == 'thumbnail') {
      $attributes['id'] = $id . '-thumbnail';
    }
  }

  // Add the slick classes.
  if ($count == 1) {
    // Prevent JS errors when only one item given, e.g.: when content editors
    // provide a single image rather than multiple items at their blog.
    $attributes['class'][] = 'unslick';
    $attributes['class'] = array_diff($attributes['class'], array('slick'));
  }
  else {
    // Use this flag if you want to animate the appearance of each slide
    // sequentially using WOW, inView or jquery.appear.
    $settings['is_carousel'] = $js_settings['slidesToShow'] > 1;
    $attributes['class'][] = $settings['is_carousel'] ? 'slick--carousel' : 'slick--slider';
  }

  $attributes['class'][] = drupal_clean_css_identifier('slick--optionset--' . $optionset_name);

  // Field or Views options override the optionset ones.
  // With asNavFor, skin maybe skin or skin_thumbnail.
  $skin = $settings['skin'] ? $settings['skin'] : $optionset->skin;
  if ($settings['current_display'] == 'thumbnail') {
    $skin = isset($settings['skin_thumbnail']) && $settings['skin_thumbnail'] ? $settings['skin_thumbnail'] : $skin;
  }

  if ($skin) {
    if ($skin !== 'boxed' && strpos($skin, 'boxed') !== FALSE) {
      $attributes['class'][] = 'slick--skin--boxed';
    }
    if ($skin !== 'split' && strpos($skin, 'split') !== FALSE) {
      $attributes['class'][] = 'slick--skin--split';
    }
    $attributes['class'][] = drupal_clean_css_identifier('slick--skin--' . $skin);
  }

  $attributes['class'][] = $js_settings['centerMode'] ? 'slick--center' : 'slick--float';

  if (isset($settings['field_type'])) {
    $attributes['class'][] = drupal_clean_css_identifier('slick--' . $settings['field_type']);
  }

  // @todo drop or more test with elementTransition, etc.
  // if ($general['transition']) {
  // $attributes['class'][] = 'slick--has-transition';
  // $attributes['class'][] = $general['transition'];
  // }

  // @todo refactor some classes into skin instead.
  // Some settings are only reasonable for the main display, not thumbnail.
  if ($settings['current_display'] == 'main') {
    $attributes['class'][] = 'slick--display--main';
    if ($settings['has_pattern']) {
      $attributes['class'][] = 'slick--has-pattern';
    }

    if ($settings['media_switch'] == 'iframe-switch') {
      if ($settings['aspect_ratio']) {
        $attributes['class'][] = $settings['aspect_ratio_overlay'] ? 'slick--ratio-overlay' : 'slick--ratio';
        $attributes['class'][] = drupal_clean_css_identifier('slick--ratio--' . $settings['aspect_ratio']);
        $attributes['class'][] = drupal_clean_css_identifier('slick--ratio--' . $settings['field_type']);
      }
    }

    if ($settings['has_arrow_down']) {
      $attributes['class'][] = 'slick--has-arrow-down';

      $settings['arrow_down_target'] = '';
      if ($general['arrow_down_target']) {
        $settings['arrow_down_target'] = str_replace(array('#', '.'), '', $general['arrow_down_target']);
      }
      $settings['arrow_down_offset'] = $general['arrow_down_offset'] ?: 125;
    }
  }
  elseif ($settings['current_display'] == 'thumbnail') {
    $attributes['class'][] = 'slick--display--thumbnail';
  }

  if (!empty($general['template_class'])) {
    $attributes['class'][] = strip_tags($general['template_class']);
  }

  // The slider must have the attribute "dir" set to "rtl", if so configured.
  if ($js_settings['rtl']) {
    $attributes['dir'] = 'rtl';
  }

  // New 1.3.7 arrows options brought a complication, since no option to place
  // them by now. Arrows may be unset and empty if arrows option is disabled,
  // so fallback to the default in code.
  // Limit sanitization with button and a few reasonable selectors.
  $allowed_tags = array('a', 'em', 'strong', 'button', 'p', 'div', 'i', 'span');
  if ($settings['arrows'] = $js_settings['arrows']) {
    $settings['prev_arrow'] = isset($js_settings['prevArrow'])
      && $js_settings['prevArrow'] ? filter_xss($js_settings['prevArrow'], $allowed_tags) :
      _slick_optionset_defaults('prevArrow', 'settings');
    $settings['next_arrow'] = isset($js_settings['nextArrow'])
      && $js_settings['nextArrow'] ? filter_xss($js_settings['nextArrow'], $allowed_tags) :
      _slick_optionset_defaults('nextArrow', 'settings');

    $default_append_arrows = _slick_optionset_defaults('appendArrows', 'settings');

    // This is the only default supported appendArrows selector.
    if ($js_settings['appendArrows'] == $default_append_arrows) {
      $js_settings['appendArrows'] = '.slick__arrow';
    }
  }

  $js_settings['mousewheel'] = $settings['mousewheel'] ? TRUE : FALSE;

  // Add the configuration as JSON object into slick container.
  if (!isset($attributes['data-config']) && $count > 1) {
    $js_data = _slick_remove_default_optionset_options($optionset, $js_settings);

    // Remove empty lazyLoad, or left to default ondemand, to avoid JS error.
    if (empty($js_data['lazyLoad'])) {
      unset($js_data['lazyLoad']);
    }

    // Do not pass arrows HTML to JSON object, as we process separately above.
    unset($js_data['prevArrow'], $js_data['nextArrow']);
    $attributes['data-config'] = drupal_json_encode($js_data);
  }

  // Add the wrapper attributes.
  $variables['attributes_array'] = $attributes;
  $settings['id'] = $attributes['id'];

  // Process individual item.
  $build = array();
  foreach ($items as $delta => $item) {
    $settings['current_item'] = $settings['current_display'];

    $build[] = array(
      '#theme' => 'slick_item',
      '#item' => $item['slide'],
      '#delta' => $delta,
      '#settings' => $settings,
      '#caption' => isset($item['caption']) ? $item['caption'] : array(),
    );
  }

  $variables['items'] = $build;
  $variables['settings'] = $settings;

  // Populate classes_array expected by Omega 3.
  // @todo drop it when Omega3/5 has a better preprocess like Omega 4.
  $variables['classes_array'] = $variables['attributes_array']['class'];

  // Add the library. More fine grained control on the loaded library should use
  // attached instead. Also D8 recommended.
  // @see slick_fields, or slick_views for the attached ones.
  // This is here to load basic libraries if you don't use the sub-modules.
  // @todo drop this babysitting. This won't work at D8 anyway.
  // If you rely on this line, make sure to change your way right away.
  if (!isset($element['#attached'])) {
    slick_add($settings);
  }
}

/**
 * Implements hook_process_HOOK().
 */
function template_process_slick(&$variables) {
  $variables['attributes'] = empty($variables['attributes_array']) ? '' : drupal_attributes($variables['attributes_array']);
}

/**
 * Implements hook_preprocess_HOOK().
 */
function template_preprocess_slick_item(&$variables) {

  $element  = $variables['element'];
  $item     = $element['#item'];
  $settings = $element['#settings'];
  $captions = $element['#caption'];

  $variables['item'] = $item;
  $variables['item_prefix'] = array();
  $variables['item_suffix'] = array();

  // Remove non-BEM default class.
  $variables['classes_array'] = array_diff($variables['classes_array'], array('slick-item'));

  // Configure attributes for containing elements.
  $attributes = $variables['attributes_array'];
  $attributes['class'] = $variables['classes_array'];
  $attributes['class'][] = 'slick__slide';
  $attributes['class'][] = 'slide';
  // Useful to add custom text color based on delta.
  $attributes['class'][] = 'slide--' . $element['#delta'];

  // Main image/background. Media module has type: image, audio, video, etc.
  // as opposed to field_type.
  if (isset($settings['type'])) {
    $attributes['class'][] = drupal_clean_css_identifier('slide--' . $settings['type']);
  }

  // @todo disable for thumb.
  $variables['caption'] = '';
  $variables['slide_pattern'] = '';

  // @todo, thumbnails may also have captions, but not now.
  if ($settings['current_item'] == 'main') {
    // Add layout/ caption position classes if so configured.
    if (!empty($captions['layout'])) {
      $layout = drupal_clean_css_identifier('slide--caption--' . $captions['layout']);
      $attributes['class'][] = $layout;
    }

    // Title, caption and overlay, or nested media.
    $stage = array();

    // Overlay image/ video.
    if (!empty($captions['overlay'])) {
      $stage['overlay'] = $captions['overlay'];
    }

    // Title.
    if (!empty($captions['title'])) {
      $stage['title'] = $captions['title'];
    }

    // Core image alt as description.
    if (!empty($captions['alt'])) {
      $stage['alt'] = $captions['alt'];
    }

    // Captions, can be multiple fields, useful for custom markups.
    if (!empty($captions['data'])) {
      $stage['data'] = $captions['data'];
    }

    // Link/ buttons.
    if (!empty($captions['link'])) {
      $stage['link'] = $captions['link'];
    }

    $variables['caption'] = $stage;

    // We split image from captions with a wrapper if we do have captions, and
    // main image. This allows more complex slide layout.
    if (!empty($stage) && !empty($item)) {
      $variables['item_prefix'] = '<div class="slide__media">';
      $variables['item_suffix'] = '</div>';
    }

    // If fullwidth, add additional wrappers to hold caption + overlay properly
    // within the max-width containner.
    if ($settings['skin'] == 'fullwidth') {
      $variables['title_prefix'] = '<div class="slide__constrained">';
      $variables['title_suffix'] = '</div>';
    }

    // $settings['attach_colorbox'] has its own pattern DIV within A tag, so
    // exclude it.
    // @see theme_slick_image_lazy.
    if ($settings['has_pattern'] && $settings['media_switch'] != 'colorbox-switch') {
      $variables['slide_pattern'] = '<div class="slide__pattern"></div>';
    }
  }

  $variables['settings'] = $settings;
  $variables['attributes_array'] = $attributes;

  // Populate classes_array expected by Omega 3.
  // @todo drop it when Omega3/5 has a better preprocess like Omega 4.
  $variables['classes_array'] = $variables['attributes_array']['class'];
}

/**
 * Implements hook_process_HOOK().
 */
function template_process_slick_item(&$variables) {
  $variables['attributes'] = empty($variables['attributes_array']) ? '' : drupal_attributes($variables['attributes_array']);
}

/**
 * Implements hook_preprocess_HOOK().
 */
function template_preprocess_slick_media(&$variables) {
  $element = $variables['element'];
  $settings = $element['#settings'];
  $variables['item'] = $element['#item'];
  // Views has no support for #asnavfor since it has no raw to work with yet.
  $variables['asnavfor'] = isset($element['#asnavfor']) ? $element['#asnavfor'] : 'main';

  $type = isset($settings['type']) ? $settings['type'] : 'image';

  $variables['item_prefix'] = array();
  $variables['item_suffix'] = array();

  // Configure attributes for containing elements.
  $attributes = $variables['attributes_array'];
  $attributes['class'] = $variables['classes_array'];
  $attributes['class'][] = 'media';

  $variables['url'] = '';
  $variables['alternative_content'] = '';

  // Only reasonable for the main display, not thumbnail navigation.
  if ($variables['asnavfor'] == 'main') {
    // Specific to Media integration.
    $media = isset($element['#media']) ? $element['#media'] : '';

    if ($media) {
      // Cherry-pick useful references for settings and options.
      $scheme = $settings['scheme'];
      $settings['api_id_attribute'] = drupal_html_id('media-' . $scheme . '-' . $settings['media_id']);

      // Soundcloud has no #options.
      if (isset($media['#options'])) {
        if (($scheme == 'youtube' && $media['#options']['enablejsapi'])
           || ($scheme == 'vimeo' && $media['#options']['api'])) {
          $variables['content_attributes_array']['id'] = $settings['api_id_attribute'];
        }
      }

      $variables['id'] = $settings['media_fid'];

      // Iframe attributes.
      $variables['content_attributes_array']['class'][] = 'media__iframe';
      $variables['content_attributes_array']['height'] = $settings['media_height'];
      $variables['content_attributes_array']['width'] = $settings['media_width'];

      // Non-JS fallback.
      // The media URL containing all the params from their iframe.
      if ($settings['url']) {
        $variables['url'] = $settings['url'];
        $variables['content_attributes_array']['src'] = $variables['url'];
      }

      // If iframe switch disabled, fallback to just iframe, and remove image
      // overlay.
      if (empty($settings['media_switch'])) {
        $variables['item'] = '';
      }
      else {
        $attributes['class'][] = 'media--switch';

        // If no colorbox, it is an media player iframe.
        if ($settings['media_switch'] != 'colorbox-switch') {
          $attributes['class'][] = 'media--player';
          $attributes['class'][] = 'media--switch--player';
          $attributes['class'][] = drupal_clean_css_identifier('media--' . $scheme . '-player');
        }

        // Gets consistent with colorbox to share JS manipulation.
        $data_media = array('type' => $type);
        $data_media['player'] = TRUE;
        $data_media['scheme'] = $scheme;
        // Add iframe attributes.
        $variables['content_attributes_array']['data-media'] = drupal_json_encode($data_media);
        $variables['content_attributes_array']['data-lazy'] = $variables['url'];
      }
      $variables['alternative_content'] = check_plain($settings['media_filename']);
    }

    // Applies to all media types, including core image field.
    if ($settings['media_switch'] == 'colorbox-switch') {
      $attributes['class'][] = 'media--switch--colorbox';
    }
  }

  $attributes['class'][] = 'media--' . $type;

  $variables['settings'] = $settings;
  $variables['attributes_array'] = $attributes;

  // Populate classes_array expected by Omega 3.
  // @todo drop it when Omega3/5 has a better preprocess like Omega 4.
  $variables['classes_array'] = $variables['attributes_array']['class'];
}

/**
 * Implements hook_process_HOOK().
 */
function template_process_slick_media(&$variables) {
  $variables['attributes'] = empty($variables['attributes_array']) ? '' : drupal_attributes($variables['attributes_array']);
}

/**
 * Implements hook_preprocess_HOOK().
 *
 * @see theme_slick_wrapper()
 */
function template_preprocess_slick_wrapper(&$variables) {
  $element = $variables['element'];
  $settings = $element['#settings'] ? $element['#settings'] : array();
  $variables['items'] = array();
  foreach ($element['#items'] as $delta => $item) {
    if (!empty($element[$delta])) {
      $variables['items'][$delta] = $element[$delta];
    }
  }

  $variables['attributes_array'] = array('class' => array('slick-wrapper'));

  if (isset($settings['asnavfor_thumbnail']) && $settings['asnavfor_thumbnail']) {
    $variables['attributes_array']['class'][] = drupal_clean_css_identifier('slick-wrapper--asnavfor');
  }

  // Populate classes_array expected by Omega 3.
  // @todo drop it when Omega3/5 has a better preprocess like Omega 4.
  $variables['classes_array'] = $variables['attributes_array']['class'];
}

/**
 * Implements hook_process_HOOK().
 */
function template_process_slick_wrapper(&$variables) {
  $variables['attributes'] = empty($variables['attributes_array']) ? '' : drupal_attributes($variables['attributes_array']);
}

/**
 * Returns HTML for a slick_wrapper.
 *
 * @param array $variables
 *   An associative array containing:
 *   - items: An array of slick items.
 *   - classes: A string containing the classes for the wrapping div.
 *   - attributes: A string containing the attributes for the wrapping div.
 *
 * @see template_preprocess_slick_wrapper()
 * @see template_process_slick_wrapper()
 *
 * @ingroup themeable
 */
function theme_slick_wrapper($variables) {
  $build = '';

  // Render the items.
  foreach ($variables['items'] as $delta => $item) {
    $build .= drupal_render($item);
  }

  // Render the top-level DIV.
  $build = '<div' . $variables['attributes'] . '>' . $build . '</div>';

  return $build;
}

/**
 * Overrides theme_image with lazy loading.
 *
 * @todo https://github.com/BBC-News/Imager.js as fallback for a single image?
 *
 * @ingroup themeable
 */
function theme_slick_image_lazy($variables) {
  $build = '';

  $path = $variables['path'];
  $variables['attributes']['class'][] = 'slick__image';

  // Make sure to avoid images with breakpoints and without image style.
  if (!isset($variables['breakpoints']) && isset($variables['style_name'])) {
    $attributes = $variables['attributes'];
    foreach (array('width', 'height', 'alt', 'title') as $key) {
      if (isset($variables[$key])) {
        $attributes[$key] = $variables[$key];
      }
    }

    $variables['path'] = image_style_url($variables['style_name'], $path);
    $count = $variables['count'];
    $path = file_create_url($variables['path']);

    // Provide SRC attribute if a single image, or lazyLoad disabled.
    if ($count == 1 || !$variables['lazy']) {
      $attributes['src'] = $path;
    }
    else {
      $attributes['class'][] = 'lazy';

      // @fixme
      // v1.3.7 allows dummy/ placeholder image:
      // https://github.com/kenwheeler/slick/pull/223
      // However lazyLoad ondemand has issue failing to generate _newly created
      // image derivatives, causing 403. It has no issue if derivatives are in
      // place already. Only _new images fail.
      // lazyLoad progressive has no issue with it.

      // Maybe data-lazy is removed before having a chance to generate one.
      // ondemand also has issue with dummy image excessive height, so only
      // progressive can have dummy image as of now.
      // Dummy image is for valid HTML5.

      // @todo drop ondemand part, when ondemand is fixed somewhere.
      // Maybe lazyLoadAhead solves it:
      // https://github.com/kenwheeler/slick/issues/466
      // https://github.com/kenwheeler/slick/issues/358
      if ($variables['lazy'] == 'progressive') {
        $attributes['src'] = 'data:image/gif;base64,R0lGODlhAQABAAAAACH5BAEKAAEALAAAAAABAAEAAAICTAEAOw==';
      }
      elseif ($variables['lazy'] == 'ondemand') {
        // Temp fix for failing image derivatives, negating lazyLoad at all.
        $attributes['src'] = $path;
      }

      $attributes['data-lazy'] = $path;
    }

    $build .= '<img ' . drupal_attributes($attributes) . ' />';

    if ($variables['options']['colorbox']) {
      $build .= '<i class="media-icon media-icon--colorbox"></i>';
      $build .= '<div class="slide__pattern"></div>';
    }

    // Only provide noscript to the first image, since the rest of the slides
    // always want JS with all this lazyLoad.
    if ($count > 1 && $variables['delta'] == 0) {
      $build .= '<noscript>' . theme_image($variables) . '</noscript>';
    }
  }
  else {
    $build = theme_image($variables);
  }

  return $build;
}

/**
 * Returns HTML for an image field formatter using lazyLoad technique.
 *
 * @param array $variables
 *   An associative array containing:
 *   - item: Associative array of image data, which may include "uri", "alt",
 *     "width", "height", "title" and "attributes".
 *   - image_style: An optional image style.
 *   - path: An array containing the link 'path' and link 'options'.
 *
 * @ingroup themeable
 */
function theme_slick_image_formatter_lazy($variables) {
  $item = $variables['item'];
  $output = '';
  $image = array(
    'path' => $item['uri'],
  );

  if (isset($item['alt'])) {
    $image['alt'] = $item['alt'];
  }

  if (isset($item['attributes'])) {
    $image['attributes'] = $item['attributes'];
  }

  // Do not output an empty 'title' attribute.
  if (isset($item['title']) && drupal_strlen($item['title']) > 0) {
    $image['title'] = $item['title'];
  }

  if ($variables['image_style']) {
    $image['style_name'] = $variables['image_style'];
    $dimensions = array(
      'width' => $item['width'],
      'height' => $item['height'],
    );

    image_style_transform_dimensions($image['style_name'], $dimensions);

    $image['width'] = $dimensions['width'];
    $image['height'] = $dimensions['height'];
  }

  $image['delta'] = $variables['delta'];
  $image['count'] = isset($variables['count']) ? $variables['count'] : 0;

  // Optional lazyLoad only reasonable if we do have slides.
  $image['lazy'] = FALSE;
  if ($image['count'] > 1) {
    $image['lazy'] = $variables['options']['lazy'];
  }
  $image['options']['type']     = $variables['options']['type'];
  $image['options']['colorbox'] = isset($variables['options']['colorbox']) && $variables['options']['colorbox'];

  $output = theme('slick_image_lazy', $image);

  // The link path and link options are both optional, but for the options to be
  // processed, the link path must at least be an empty string.
  if (isset($variables['path']['path'])) {
    $path = $variables['path']['path'];
    $options = isset($variables['path']['options']) ? $variables['path']['options'] : array();
    // When displaying an image inside a link, the html option must be TRUE.
    $options['html'] = TRUE;
    $output = l($output, $path, $options);
  }

  return $output;
}
