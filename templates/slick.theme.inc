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
 *   related to JS settings, mostly slide layouts or attaching assests.
 * - #options is set programmatically, or hand-crafted, and only accepts direct
 *   key|value pairs related to JS settings, or at least optionset name.
 */
function template_preprocess_slick(&$variables) {

  $element   = $variables['element'];
  $items     = $element['#items'];
  $count     = count($items);

  // HTML or related layout settings, none of JS settings, except the new
  // custom mousewheel.
  $settings  = array(
    'optionset'       => '',
    'skin'            => '',
    'media_switch'    => FALSE,
    'is_carousel'     => FALSE,
    'has_pattern'     => FALSE,
    'has_arrow_down'  => FALSE,
    'thumbnail_style' => FALSE,
    'thumbnail_hover' => FALSE,
    'mousewheel'      => FALSE,
    'arrows'          => FALSE,
    'aspect_ratio_overlay' => FALSE,
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
  );

  // The Slick JS settings defined either via custom #options, or CTools UI.
  $js_settings = $customs ? array_merge($options['settings'], $customs) : $options['settings'];
  // Anon DIV is problematic, since Slick will slide every div element within
  // its container which makes it impossible to add a div to wrap buttons (such
  // as arrow down) nor wrap them for advanced customPaging. Hence we override
  // with more specific class .slick__slide instead.
  // Note: Having list type is irrelevant to the JS generated markups.
  $js_settings['slide'] = $js_settings['slide'] == 'div' ? '.slick__slide' : $js_settings['slide'];

  // Field or Views overrides the optionset one.
  $settings['has_arrow_down'] = $settings['has_arrow_down'] ? TRUE : isset($goodies['arrow-down']) && $goodies['arrow-down'];
  $settings['has_pattern'] = $settings['has_pattern'] ? TRUE : isset($goodies['pattern']) && $goodies['pattern'];

  // Configure attributes for containing elements.
  $attributes = $variables['attributes_array'];
  $attributes['class'] = $variables['classes_array'];

  // Merge with defined attributes.
  if (isset($settings['attributes']) && is_array($settings['attributes'])) {
    $attributes += $settings['attributes'];
  }

  // Set the ID for each slick instance if none is provided.
  if (empty($attributes['id'])) {
    $slick_id = &drupal_static('slick_id', 0);
    $attributes['id'] = 'slick-' . ++$slick_id;
  }

  // Add the slick classes.
  if ($count == 1) {
    // Prevent JS errors when only one item given.
    $attributes['class'][] = 'unslick';
    $attributes['class'] = array_diff($attributes['class'], array('slick'));
  }
  else {
    // $attributes['class'][] = 'slick';
    $settings['is_carousel'] = $js_settings['slidesToShow'] > 1;
    $attributes['class'][] = $settings['is_carousel'] ? 'slick--carousel' : 'slick--slider';
  }

  if ($optionset_name) {
    $attributes['class'][] = drupal_clean_css_identifier('slick--optionset--' . $optionset_name);
  }

  // Field or Views overrides the optionset one.
  $skin = $settings['skin'] ?: $optionset->skin;
  if ($skin) {
    if ($skin !== 'boxed' && strpos($skin, 'boxed') !== FALSE) {
      $attributes['class'][] = 'slick--skin--boxed';
    }
    if ($skin !== 'split' && strpos($skin, 'split') !== FALSE) {
      $attributes['class'][] = 'slick--skin--split';
    }
    $attributes['class'][] = drupal_clean_css_identifier('slick--skin--' . $skin);
  }

  if ($js_settings['centerMode']) {
    $attributes['class'][] = 'slick--center';
  }

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

  if (isset($settings['field_type'])) {
    $attributes['class'][] = drupal_clean_css_identifier('slick--' . $settings['field_type']);
  }

  // @todo drop or more test with elementTransition, etc.
  // if ($general['transition']) {
  // $attributes['class'][] = 'slick--has-transition';
  // $attributes['class'][] = $general['transition'];
  // }

  // @todo refactor some classes into skin instead.
  if ($settings['thumbnail_style']) {
    $attributes['class'][] = 'slick--has-thumbnail';
    if ($settings['thumbnail_hover']) {
      $attributes['class'][] = 'slick--has-thumbnail--hover';
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

  if (!empty($general['template_class'])) {
    $attributes['class'][] = strip_tags($general['template_class']);
  }

  // New 1.3.7 arrows options brought a complication, since no option to place
  // them by now. Arrows may be unset and empty if arrows option is disabled,
  // so fallback to the default in code.
  // Limit sanitization with button support.
  $allowed_tags = array('a', 'em', 'strong', 'button', 'p', 'div', 'i', 'span');
  if ($settings['arrows'] = $js_settings['arrows']) {
    $settings['prev_arrow'] = isset($js_settings['prevArrow'])
      && $js_settings['prevArrow'] ? filter_xss($js_settings['prevArrow'], $allowed_tags) :
      _slick_optionset_defaults('prevArrow', 'settings');
    $settings['next_arrow'] = isset($js_settings['nextArrow'])
      && $js_settings['nextArrow'] ? filter_xss($js_settings['nextArrow'], $allowed_tags) :
      _slick_optionset_defaults('nextArrow', 'settings');
  }

  $js_settings['mousewheel'] = $settings['mousewheel'] ? TRUE : FALSE;

  // Add the configuration as JSON object into slick container.
  if (!isset($attributes['data-config']) && $count > 1) {
    $js_data = _slick_remove_default_optionset_options($optionset, $js_settings);
    // Do not pass arrows HTML to JSON object, as we process separately.
    unset($js_data['prevArrow'], $js_data['nextArrow']);
    $attributes['data-config'] = drupal_json_encode($js_data);
  }

  // Add the wrapper attributes.
  $variables['attributes_array'] = $attributes;
  $settings['id'] = $attributes['id'];

  // Process individual item.
  $build = array();
  $thumbnails = array();
  foreach ($items as $delta => $item) {
    if (isset($item['settings'])) {
      $settings = is_array($item['settings']) ? array_merge($settings, $item['settings']) : $settings;
    }

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

  // Add the library. More fine grained control on the loaded library should use
  // attached instead. Also D8 recommended.
  // @see slick_fields, or slick_views for the attached ones.
  // This is here to load basic libraries if you don't use the sub-modules.
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

  // Add layout/ caption position classes if so configured.
  if (!empty($captions['layout'])) {
    $layout = drupal_clean_css_identifier($captions['layout']);
    $attributes['class'][] = 'slide--caption--' . $layout;
  }

  $variables['item'] = $item;

  // Main image/background. Media has type: image, audio, video, etc.
  // as opposed to field_type.
  if (isset($settings['type'])) {
    $type = drupal_clean_css_identifier($settings['type']);
    $attributes['class'][] = 'slide--' . $type;
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

  // Captions.
  if (!empty($captions['data'])) {
    $stage['data'] = $captions['data'];
  }

  // Link/ buttons.
  if (!empty($captions['link'])) {
    $stage['link'] = $captions['link'];
  }

  $variables['caption'] = $stage;

  // We split image from captions with a wrapper if we do have captions.
  if (!empty($stage)) {
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
  $variables['slide_pattern'] = '';
  if ($settings['has_pattern'] && $settings['media_switch'] != 'colorbox-switch') {
    $variables['slide_pattern'] = '<div class="slide__pattern"></div>';
  }
  $variables['attributes_array'] = $attributes;
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

  $type = isset($settings['type']) ? $settings['type'] : 'image';

  $variables['item_prefix'] = array();
  $variables['item_suffix'] = array();

  // Configure attributes for containing elements.
  $attributes = $variables['attributes_array'];
  $attributes['class'] = $variables['classes_array'];
  $attributes['class'][] = 'media';

  // Specific to Media integration.
  $media = isset($element['#media']) ? $element['#media'] : '';
  $variables['url'] = '';

  $variables['alternative_content'] = '';

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
        $attributes['class'][] = 'media--' . $scheme . '-player';
      }

      // Gets consistent with colorbox to share codes.
      $data_media = array('type' => $settings['type']);
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

  $attributes['class'][] = 'media--' . $type;

  $variables['settings'] = $settings;
  $variables['attributes_array'] = $attributes;
}

/**
 * Implements hook_process_HOOK().
 */
function template_process_slick_media(&$variables) {
  $variables['attributes'] = empty($variables['attributes_array']) ? '' : drupal_attributes($variables['attributes_array']);
}

/**
 * Overrides theme_image with lazy loading.
 *
 * @see https://github.com/kenwheeler/slick/pull/223
 * @todo https://github.com/BBC-News/Imager.js as fallback for a single image?
 * 'data:image/gif;base64,R0lGODlhAQABAAAAACH5BAEKAAEALAAAAAABAAEAAAICTAEAOw==';
 *
 * @ingroup themeable
 */
function theme_slick_image_lazy($variables) {
  $build = '';

  $path = $variables['path'];
  $variables['attributes']['class'][] = 'slick__image';

  // Make sure to avoid images with breakpoints and without image_style.
  if (!isset($variables['breakpoints']) && isset($variables['style_name'])) {
    $attributes = $variables['attributes'];
    foreach (array('width', 'height', 'alt', 'title') as $key) {
      if (isset($variables[$key])) {
        $attributes[$key] = $variables[$key];
      }
    }

    // @todo add SRC alternative, SRC not supported by 1.3.6, and HTML5 invalid.
    // https://github.com/kenwheeler/slick/pull/223
    // $attributes['src'] = 'some alt image, maybe preloader';
    $variables['path'] = image_style_url($variables['style_name'], $variables['path']);
    $count = $variables['count'];

    // Provide SRC attribute if a single image or lazyLoad disabled.
    if ($count == 1 || !$variables['lazy']) {
      $attributes['src'] = file_create_url($variables['path']);
    }
    else {
      $attributes['class'][] = 'lazy';
      $attributes['data-lazy'] = file_create_url($variables['path']);
    }
    $build .= '<img ' . drupal_attributes($attributes) . ' />';

    if ($variables['options']['colorbox']) {
      $build .= '<i class="media-icon media-icon--colorbox"></i>';
      $build .= '<div class="slide__pattern"></div>';
    }

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

  // Optionset determines lazyLoad obligatory, hence only applies to images > 1.
  $image['lazy'] = $image['count'] > 1;
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

/**
 * Implements hook_preprocess_HOOK().
 *
 * @todo drop, deprecated by theme_slick.
 */
function template_preprocess_slick_carousel(&$variables) {
  $items = $variables['items'];
  $options = $variables['options'];

  // Add item classes.
  foreach ($items as $index => $item) {
    $item_classes = array('item', 'item-' . $index);
    $variables['item_classes'][$index] = implode(" ", $item_classes);
  }

  // Add slick attributes.
  $variables['classes_array'][] = 'slick';

  // Make use of defaults.
  $optionset = slick_optionset_load('default');

  // Set settings as data attributes.
  $options['settings'] = $options;
  _slick_typecast_optionset($options['settings']);
  $configs = _slick_remove_default_optionset_options($optionset, $options['settings']);

  $variables['attributes_array']['data-config'] = drupal_json_encode($configs);

  // Add libraries.
  slick_add();
}
