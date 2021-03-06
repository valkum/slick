<?php

/**
 * @file
 * Preprocessors and helper functions to make theming easier.
 */

use Drupal\Core\Template\Attribute;
use Drupal\Component\Serialization\Json;
use Drupal\Component\Utility\Html;
use Drupal\Component\Utility\String;
use Drupal\Component\Utility\Xss;

/**
 * Implements hook_preprocess_HOOK().
 *
 * Elements:
 * - #settings is set via sub-modules and serves various purposes, and not
 *   related to JS settings, mostly slide layouts or attaching assets.
 * - #options is set programmatically, or hand-crafted, and only accepts direct
 *   key|value pairs related to JS settings, or at least optionset name.
 *
 * @todo: Improve customs and settings array.
 */
function template_preprocess_slick(&$variables) {

  $element = $variables['element'];
  $items = $element['#items'];
  $count = count($items);

  // HTML or layout related settings, none of JS settings, except mousewheel.
  $settings = array(
    'optionset' => '',
    'optionset_thumbnail' => '',
    'skin' => '',
    'skin_thumbnail' => '',
    'grid' => '',
    'visible_slides' => '',
    'media_switch' => FALSE,
    'is_carousel' => FALSE,
    'has_pattern' => FALSE,
    'has_arrow_down' => FALSE,
    'mousewheel' => FALSE,
    'arrows' => FALSE,
    'aspect_ratio_overlay' => FALSE,
    'current_display' => 'main',
    'count' => $count,
  );

  $settings = isset($element['#settings']) ? array_merge($settings, $element['#settings']) : $settings;
  $custom_settings = isset($element['#options']) ? $element['#options'] : array();

  // Optionset may be defined via custom code, CTools UI, or sub-modules.
  $optionset = isset($custom_settings['optionset']) ? $custom_settings['optionset'] : $settings['optionset'];

  // Load the selected optionset.
  if (!empty($optionset)) {
    $optionset = slick_optionset_load($optionset);
  }

  // Check if an optionset was loaded.
  if (empty($optionset)) {
    // Fall back to 'default' option set.
    $optionset = slick_optionset_load('default');
    \Drupal::logger('slick')->notice("Fallback to default optionset.");
  }

  $general = $optionset->general;
  $goodies = $general['goodies'];

  // The Slick JS settings defined either via custom #options, or CTools UI.
  $js = $custom_settings ? array_merge($optionset->settings, $custom_settings) : $optionset->settings;

  // Field or Views options override the optionset ones.
  $settings['has_arrow_down'] = $settings['has_arrow_down'] ? TRUE : isset($goodies['arrow-down']) && $goodies['arrow-down'];

  $settings['has_pattern'] = $settings['has_pattern'] ? TRUE : isset($goodies['pattern']) && $goodies['pattern'];

  // Only add asNavFor if we do have thumbnail optionset.
  if ($settings['optionset_thumbnail']) {
    if ($settings['current_display'] == 'main') {
      if (isset($settings['asnavfor_main']) && $settings['asnavfor_main']) {
        $js['asNavFor'] = String::checkPlain($settings['asnavfor_main']);
      }
    }
    elseif ($settings['current_display'] == 'thumbnail') {
      if (isset($settings['asnavfor_thumbnail']) && $settings['asnavfor_thumbnail']) {
        $js['asNavFor'] = String::checkPlain($settings['asnavfor_thumbnail']);
      }
    }
  }

  // Configure attributes for containing elements.
  $variables['attributes']['class'][] = 'slick';

  // Merge with defined attributes.
  if (isset($settings['attributes']) && is_array($settings['attributes'])) {
    $variables['attributes'] += $settings['attributes'];
  }

  // Set the ID for each slick instance, if none provided, for quick JS lookup.
  if (empty($variables['attributes']['id'])) {
    $slick_id = &drupal_static('slick_id', 0);
    $id = 'slick-' . ++$slick_id;
    $variables['attributes']['id'] = $id;
    if ($settings['current_display'] == 'thumbnail') {
      $variables['attributes']['id'] = $id . '-thumbnail';
    }
  }

  // Add the slick classes.
  if ($count == 1) {
    // Prevent JS errors when only one item given, e.g.: when content editors
    // provide a single image rather than multiple items at their blog.
    $variables['attributes']['class'][] = 'unslick';
    $variables['attributes']['class'] = array_diff($variables['attributes']['class'], array('slick'));
  }
  else {
    // Use this flag to animate the appearance of each slide sequentially using
    // WOW, inView or jquery.appear.
    $settings['is_carousel'] = $js['slidesToShow'] > 1;
    $variables['attributes']['class'][] = $settings['is_carousel'] ? 'slick--carousel' : 'slick--slider';
  }

  $variables['attributes']['class'][] = Html::cleanCssIdentifier('slick--optionset--' . $optionset->id);

  // Field or Views options override the optionset ones.
  // With asNavFor, skin maybe skin or skin_thumbnail.
  $skin = $settings['skin'] ? $settings['skin'] : $optionset->skin;
  if ($settings['current_display'] == 'thumbnail') {
    $skin = isset($settings['skin_thumbnail']) && $settings['skin_thumbnail'] ? $settings['skin_thumbnail'] : $skin;
  }

  if ($skin) {
    if ($skin !== 'boxed' && strpos($skin, 'boxed') !== FALSE) {
      $variables['attributes']['class'][] = 'slick--skin--boxed';
    }
    if ($skin !== 'split' && strpos($skin, 'split') !== FALSE) {
      $variables['attributes']['class'][] = 'slick--skin--split';
    }
    $variables['attributes']['class'][] = Html::cleanCssIdentifier('slick--skin--' . $skin);
  }

  $variables['attributes']['class'][] = $js['centerMode'] ? 'slick--center' : 'slick--float';

  // @todo drop, is it really needed anywhere else?
  if (isset($settings['field_type'])) {
    $variables['attributes']['class'][] = Html::cleanCssIdentifier('slick--' . $settings['field_type']);
  }

  // @todo drop or more test with elementTransition, etc.
  // if ($general['transition']) {
  // $attributes['class'][] = 'slick--has-transition';
  // $attributes['class'][] = $general['transition'];
  // }
  //
  // @todo refactor some classes into skin instead.
  // Some settings are only reasonable for the main display, not thumbnail.
  if ($settings['current_display'] == 'main') {
    $variables['attributes']['class'][] = 'slick--display--main';
    if ($settings['has_pattern']) {
      $variables['attributes']['class'][] = 'slick--has-pattern';
    }

    if ($settings['media_switch'] == 'iframe-switch') {
      if ($settings['aspect_ratio']) {
        $variables['attributes']['class'][] = $settings['aspect_ratio_overlay'] ? 'slick--ratio-overlay' : 'slick--ratio';
        $variables['attributes']['class'][] = Html::cleanCssIdentifier('slick--ratio--' . $settings['aspect_ratio']);
        $variables['attributes']['class'][] = Html::cleanCssIdentifier('slick--ratio--' . $settings['field_type']);
      }
    }

    if ($settings['has_arrow_down']) {
      $variables['attributes']['class'][] = 'slick--has-arrow-down';

      $settings['arrow_down_target'] = '';
      if ($general['arrow_down_target']) {
        $settings['arrow_down_target'] = str_replace(array('#', '.'), '', $general['arrow_down_target']);
      }
      $settings['arrow_down_target'] = $settings['arrow_down_target'] ? ' data-target="#' . $settings['arrow_down_target'] . '"' : '';
      $settings['arrow_down_offset'] = $general['arrow_down_offset'] ? $general['arrow_down_offset'] : 125;
      $settings['arrow_down_offset'] = $settings['arrow_down_offset'] ? ' data-offset="' . $settings['arrow_down_offset'] . '"' : '';
    }
  }
  elseif ($settings['current_display'] == 'thumbnail') {
    $variables['attributes']['class'][] = 'slick--display--thumbnail';
  }

  if (!empty($general['template_class'])) {
    $variables['attributes']['class'][] = strip_tags($general['template_class']);
  }

  // The slider must have the attribute "dir" set to "rtl", if so configured.
  if ($js['rtl']) {
    $variables['attributes']['dir'] = 'rtl';
  }

  // Arrows may be unset and empty if arrows option is disabled, so fallback to
  // the default in code.
  // Limit sanitization with button and a few reasonable selectors.
  $allowed_tags = array(
    'a',
    'em',
    'strong',
    'button',
    'p',
    'div',
    'i',
    'span',
  );
  if ($settings['arrows'] = $js['arrows']) {
    $is_prev = isset($js['prevArrow']) && $js['prevArrow'];
    $is_next = isset($js['nextArrow']) && $js['nextArrow'];
    $settings['prev_arrow'] = $is_prev ? Xss::filter($js['prevArrow'], $allowed_tags) : _slick_optionset_defaults('prevArrow', 'settings');
    $settings['next_arrow'] = $is_next ? Xss::filter($js['nextArrow'], $allowed_tags) : _slick_optionset_defaults('nextArrow', 'settings');

    // This is the only default supported appendArrows selector.
    $default_append_arrows = _slick_optionset_defaults('appendArrows', 'settings');
    if ($js['appendArrows'] == $default_append_arrows) {
      $js['appendArrows'] = '.slick__arrow';
    }
  }

  // Add the configuration as JSON object into slick container.
  // Avoid possible future common namedspaced: data-slick.
  // @todo: Use Drupal\Component\Utility\DiffArray.
  if (!isset($attributes['data-config']) && $count > 1) {
    $js_data = _slick_remove_default_optionset_options($optionset, $js);

    // Remove empty lazyLoad, or left to default ondemand, to avoid JS error.
    if (empty($js_data['lazyLoad'])) {
      unset($js_data['lazyLoad']);
    }

    // Do not pass arrows HTML to JSON object, as we process separately above.
    unset($js_data['prevArrow'], $js_data['nextArrow']);
    $variables['attributes']['data-config'] = Json::encode($js_data);
  }

  // Add the wrapper attributes.
  $settings['id'] = $variables['attributes'];

  // Process individual item.
  $build = array();
  foreach ($items as $delta => $item) {
    $settings['current_item'] = $settings['current_display'];

    $build[$delta] = array(
      '#theme' => 'slick_item',
      '#item' => isset($item['slide']) ? $item['slide'] : $item,
      '#delta' => $delta,
      '#settings' => $settings,
      '#caption' => isset($item['caption']) ? $item['caption'] : array(),
    );
  }

  $variables['items'] = $build;
  $variables += $settings;

  // Add basic library.
  if ($count > 1) {
    $variables['#attached']['library'][] = 'slick/jquery.slick';
  }
}

/**
 * Implements hook_preprocess_HOOK().
 */
function template_preprocess_slick_item(&$variables) {
  $element = $variables['element'];
  $item = $element['#item'];
  $settings = $element['#settings'];
  $captions = $element['#caption'];

  $variables['item'] = $item;
  $variables['item_prefix'] = array();
  $variables['item_suffix'] = array();

  $variables['tag'] = $variables['content_tag'] = 'div';
  $variables['content_attributes'] = new Attribute();
  $variables['content_attributes']['class'] = array();

  if ($settings['current_item'] == 'grid') {
    $one = 'slide__grid';
    $two = 'grid';
    $three = 'grid--' . $element['#delta'];
    $variables['content_attributes']['class'][] = 'grid__content';

    // Borrows Foundation block-grid so that reusable site wide.
    // Override/empty skin Grid css when Foundation grid already in place, and
    // block-grid classes are maintained.
    // @see hook_slick_skins_info_alter(), or other ways to override CSS.
    $variables['tag'] = 'li';
    $variables['content_tag'] = 'div';
  }
  else {
    $one = 'slick__slide';
    $two = 'slide';
    // Useful to add custom text color based on delta.
    $three = 'slide--' . $element['#delta'];
    $variables['content_attributes']['class'][] = 'slide__content';
    $variables['tag'] = 'div';
  }

  $variables['attributes']['class'][] = $one;
  $variables['attributes']['class'][] = $two;
  $variables['attributes']['class'][] = $three;

  // Only add relevant Foundation block-grid classes if the skin is Grid so that
  // you can use own grid system, otherwise overrides at several levels.
  if ($settings['grid'] && $settings['current_item'] == 'main') {
    // Allows semantic grid.
    if ($settings['skin'] == 'grid') {
      if ($settings['grid_small']) {
        $variables['content_attributes']['class'][] = 'small-block-grid-' . $settings['grid_small'];
      }
      if ($settings['grid_medium']) {
        $variables['content_attributes']['class'][] = 'medium-block-grid-' . $settings['grid_medium'];
      }
      $variables['content_attributes']['class'][] = 'large-block-grid-' . $settings['grid'];
    }

    $variables['attributes']['class'][] = 'slide--grid';

    $variables['content_tag'] = 'ul';
  }

  // Main image/background. Media module has type: image, audio, video, etc.
  // as opposed to field_type.
  if (isset($settings['type'])) {
    $variables['attributes']['class'][] = Html::cleanCssIdentifier('slide--' . $settings['type']);
  }

  // @todo, disabled for thumb by now when Slick resolves selector issue.
  $variables['caption'] = '';
  $variables['slide_pattern'] = '';

  // @todo, thumbnails may have captions when Slick resolves selector issue.
  if ($settings['current_item'] != 'thumbnail') {
    // Add layout/ caption position classes if so configured.
    if (!empty($captions['layout'])) {
      $layout = Html::cleanCssIdentifier('slide--caption--' . $captions['layout']);
      $variables['attributes']['class'][] = $layout;
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
    if ((!empty($stage) && $item) || (strpos($settings['skin'], '3d') !== FALSE)) {
      $variables['item_prefix'] = '<div class="slide__media">';
      $variables['item_suffix'] = '</div>';
    }

    // If fullwidth or fullscreen, add additional wrappers to hold caption and
    // overlay properly within the max-width containner.
    if (strpos($settings['skin'], 'full') !== FALSE) {
      $variables['title_prefix'] = '<div class="slide__constrained">';
      $variables['title_suffix'] = '</div>';
    }

    // Exclude colorbox switcher as it has its own pattern DIV within A tag.
    // @see theme_slick_image_lazy().
    if (isset($settings['has_pattern']) && $settings['has_pattern'] && $settings['media_switch'] != 'colorbox-switch') {
      $variables['slide_pattern'] = '<div class="slide__pattern"></div>';
    }
  }

  $variables['settings'] = $settings;
}

/**
 * Implements hook_process_HOOK().
 *
 * Tips:
 *   - Avoid variableWidth and adaptiveHeight.
 *   - Use slidesToSHow > 1 to build more complex grids.
 */
function template_preprocess_slick_grid(&$variables) {
  $element = $variables['element'];
  $settings = $element['#settings'];

  $build = array();

  // Implementors should be responsible to pass proper grid chunks.
  foreach ($element['#items'] as $delta => $item) {
    $settings['current_item'] = 'grid';

    $build[$delta] = array(
      '#theme' => 'slick_item',
      '#item' => isset($item['slide']) ? $item['slide'] : '',
      '#delta' => $delta,
      '#settings' => $settings,
      '#caption' => isset($item['caption']) ? $item['caption'] : array(),
    );
  }

  $variables['items'] = $build;
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
        if (($scheme == 'youtube' && $media['#options']['enablejsapi']) || ($scheme == 'vimeo' && $media['#options']['api'])) {
          $variables['content_attributes_array']['id'] = $settings['api_id_attribute'];
        }
      }

      $variables['id'] = $settings['media_fid'];

      // Iframe attributes.
      $variables['content_attributes_array']['class'][] = 'media__iframe';
      $variables['content_attributes_array']['height'] = $settings['media_height'];
      $variables['content_attributes_array']['width'] = $settings['media_width'];

      // Non-JS fallback.
      // The media URL containing all the params extracted from their iframe.
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

        // If no colorbox, it is a media player iframe.
        if ($settings['media_switch'] != 'colorbox-switch') {
          $attributes['class'][] = 'media--player';
          $attributes['class'][] = 'media--switch--player';
          $attributes['class'][] = Html::cleanCssIdentifier('media--' . $scheme . '-player');
        }

        // Gets consistent with colorbox to share JS manipulation.
        $data_media = array('type' => $type);
        $data_media['player'] = TRUE;
        $data_media['scheme'] = $scheme;
        // Add iframe attributes.
        $variables['content_attributes_array']['data-media'] = drupal_json_encode($data_media);
        $variables['content_attributes_array']['data-lazy'] = $variables['url'];
      }
      $variables['alternative_content'] = String::checkPlain($settings['media_filename']);
    }

    // Colorbox switcher applies to all media types, including core image field.
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
 * Implements hook_preprocess_HOOK().
 *
 * @see theme_slick_wrapper()
 */
function template_preprocess_slick_wrapper(&$variables) {
  $element = $variables['element'];
  $settings = $element['#settings'] ? $element['#settings'] : array();

  $variables['items'] = array();
  foreach ($element['#items'] as $delta => $item) {
    if (!empty($item)) {
      $variables['items'][$delta] = $item;
    }
  }

  $variables['attributes']['class'][] = 'slick-wrapper';

  if (isset($settings['skin']) && $settings['skin']) {
    $variables['attributes']['class'][] = Html::cleanCssIdentifier('slick-wrapper--' . $settings['skin']);
  }

  if (isset($settings['asnavfor_thumbnail']) && $settings['asnavfor_thumbnail']) {
    $variables['attributes']['class'][] = Html::cleanCssIdentifier('slick-wrapper--asnavfor');
  }
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
 *
 * @todo The function drupal_render is deprecated. Drop this? Or change to use #markup.
 */
function theme_slick_wrapper(array $variables) {
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
 * Notes:
 * The issue here is no longer valid with the latest Slick. However if the issue
 * persists at your end, please try two possible fixes below.
 *
 * v1.3.7 allows dummy/ placeholder image:
 * https://github.com/kenwheeler/slick/pull/223
 * However lazyLoad ondemand has issue failing to generate _newly created image
 * derivatives, causing 403. It has no issue if derivatives are in place
 * already. Only _new images fail. lazyLoad progressive has no issue with it.
 *
 * May relate to itok: https://www.drupal.org/drupal-7.20-release-notes
 * Possible fixes without compromising security:
 *   - https://www.drupal.org/project/imageinfo_cache, to have derivatives
 *     in place earlier.
 *   - Or use lazyLoad "progressive" instead.
 *
 * ondemand also has issue with dummy image excessive height, so use it
 * with care. Dummy image is for valid HTML5.
 *
 * @ingroup themeable
 *
 * @todo: As theme_image now accpets inline image drop this?
 */
function theme_slick_image_lazy(array $variables) {
  $build = '';

  $path = $variables['path'];
  $variables['attributes']['class'][] = 'slick__image';

  // Make sure to avoid images with points and without image style.
  if (!isset($variables['breakpoints']) && isset($variables['style_name'])) {
    $attributes = $variables['attributes'];
    foreach (array('width', 'height', 'alt', 'title') as $key) {
      if (isset($variables[$key])) {
        // Backported https://www.drupal.org/node/999338.
        // If the property has already been defined in the attributes,
        // do not override, including NULL.
        if (array_key_exists($key, $attributes)) {
          continue;
        }
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

      // This is why we have this function, D7 core theme_image() only expects
      // a file, not inline image. Dropped at D8 that supports it, and merged to
      // theme_slick_image_formatter_lazy() instead.
      $attributes['src'] = 'data:image/gif;base64,R0lGODlhAQABAAAAACH5BAEKAAEALAAAAAABAAEAAAICTAEAOw==';
      $attributes['data-lazy'] = $path;
    }

    $build .= '<img ' . drupal_attributes($attributes) . ' />';

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
 *
 * @todo: Need cleanup/refactor for D8
 */
function theme_slick_image_formatter_lazy(array $variables) {
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
  $image['options']['type'] = $variables['options']['type'];

  $output = theme('slick_image_lazy', $image);

  // The link path and link options are both optional, but for the options to be
  // processed, the link path must at least be an empty string.
  if (isset($variables['path']['path'])) {
    if (isset($variables['options']['colorbox']) && $variables['options']['colorbox']) {
      $output .= '<i class="media-icon media-icon--colorbox"></i>';
      $output .= '<div class="slide__pattern"></div>';
    }

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
 */
function template_preprocess_slick_view(&$variables) {
  // Prepare all data to be passed to theme_slick().
  if (!empty($variables['view'])) {
    $view = $variables['view'];
    $rows = $variables['rows'];

    $style = $view->style_plugin;
    $settings = $style->options;

    $items = array();
    $thumbs = array();

    $attach = array(
      'attach_media' => FALSE,
      'attach_colorbox' => FALSE,
      'attach_skin' => NULL,
      'attach_skin_thumbnail' => NULL,
    );

    foreach ($variables['rows'] as $id => $row) {
      // Sets row_index to get the rendered fields.
      $view->row_index = $id;
      $item = array(
        'item' => array(
          'type' => 'views',
          'view_name' => String::checkPlain($view->storage->id()),
        ),
        'slide' => array(),
      );
      $thumb = array();

      // @todo Is this even working?
      if (!empty($settings['slick']['slide_field_wrapper'])) {
        $slide_image = $settings['slick']['slide_image'];

        // Main image, if so configured.
        if (isset($row->image) && !empty($row->image)) {
          $attach['attach_media'] = TRUE;
          $item['slide'] = $row->image;
          // @todo refine for asNavFor.
          if (isset($row->thumbnail) && !empty($row->thumbnail)) {
            $settings['slick']['thumbnail_style'] = TRUE;
            $thumb['slide'] = $row->thumbnail;
          }
        }

        // Caption fields.
        $item['caption'] = array();
        if (isset($row->caption) && !empty($row->caption)) {
          $item['caption']['data'] = $row->caption;
        }

        // Overlay, if so configured.
        if (isset($row->overlay) && !empty($row->overlay)) {
          $attach['attach_media'] = TRUE;
          $item['caption']['overlay'] = $row->overlay;
        }

        // Title, if so configured.
        if (isset($row->title) && !empty($row->title)) {
          $item['caption']['title'] = filter_xss_admin($row->title);
        }

        // Link, if so configured.
        if (isset($row->link) && !empty($row->link)) {
          $item['caption']['link'] = $row->link;
        }

        // Layout, if so configured.
        if (isset($row->layout) && !empty($row->layout)) {
          $item['caption']['layout'] = String::checkPlain($row->layout);
        }
      }
      else {
        // @todo refine more.
        // $attach['attach_colorbox'] = TRUE;
        $attach['attach_media'] = TRUE;
        $item['slide'] = $row;
      }

      $items[] = $item;

      if ($thumb) {
        $thumb['item']['type'] = 'views';
        $thumb['item']['view_name'] = $view->storage->id();
        // @todo thumbnails may have caption when Slick resolves selector issue.
        $thumb['caption'] = array();
        $thumbs[] = $thumb;
      }
    }

    unset($view->row_index);
    $options = array();
    $options['attributes'] = new Attribute();

    // Add the settings.
    $settings['slick']['current_display'] = 'main';
    if ($settings['slick']['skin']) {
      $attach['attach_skin'] = $settings['slick']['skin'];
    }

    if (isset($settings['slick']['skin_thumbnail']) && $settings['slick']['skin_thumbnail']) {
      $attach['attach_skin_thumbnail'] = $settings['slick']['skin_thumbnail'];
    }
    $settings['attributes'] = new Attribute();
    // Build the Slick attributes.
    if (!empty($settings['slick']['id'])) {
      $options['attributes']['id'] = $settings['slick']['id'];
    }

    // Build the Slick grid.
    $grid = isset($settings['slick']['grid']) && $settings['slick']['grid'] ? $settings['slick']['grid'] : '';
    if ($grid) {
      $visible_slides = $settings['slick']['visible_slides'];
      // Only if we do have a reasonable amount of grid items.
      if ($visible_slides && count($variables['rows']) > $visible_slides) {
        $grid_items = array_chunk($items, $visible_slides);

        $grids = array();
        foreach ($grid_items as $delta => $grid_children) {
          $grids[] = array(
            '#theme' => 'slick_grid',
            '#items' => $grid_children,
            '#delta' => $delta,
            '#settings' => $options,
          );
        }

        // Update the $items with grid chunks.
        $items = $grids;
      }
    }

    // Build the Slick main display.
    $element[0] = array(
      '#theme' => 'slick',
      '#items' => $items,
      '#settings' => $options,
    );

    // Build the Slick thumbnail display.
    if (isset($settings['optionset_thumbnail']) && $settings['optionset_thumbnail'] && $thumbs) {
      if ($settings['id']) {
        $options['attributes']['id'] = $settings['id'] . '-thumbnail';
      }
      $options['optionset'] = $settings['slick']['optionset_thumbnail'];
      $options['current_display'] = 'thumbnail';
      $element[1] = array(
        '#theme' => 'slick',
        '#items' => $thumbs,
        '#settings' => $options,
        '#attached' => array(),
      );
    }

    // Updates rows to contain Slick instances now.
    $variables['rows'] = $element;

    // Build the Slick wrapper attributes.
    $variables['attributes']['class'][] = 'slick-wrapper';
    if (isset($settings['skin']) && $settings['skin']) {
      $variables['attributes']['class'][] = Html::cleanCssIdentifier('slick-wrapper--' . $settings['slick']['skin']);
    }

    if (isset($settings['slick']['asnavfor_thumbnail']) && $settings['slick']['asnavfor_thumbnail']) {
      $variables['attributes']['class'][] = Html::cleanCssIdentifier('slick-wrapper--asnavfor');
    }
  }
}
