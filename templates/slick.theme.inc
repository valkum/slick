<?php

/**
 * @file
 * Hooks and preprocess functions for slick module.
 */

use Drupal\Component\Utility\Html;
use Drupal\Component\Utility\Xss;
use Drupal\Component\Serialization\Json;
use Drupal\Core\Template\Attribute;


/**
 * Prepares variables for slick templates.
 *
 * Default template: slick.html.twig.
 *
 * @param array $variables
 *   An associative array containing:
 *   - #items: An array of items to be displayed in the slick slider. Each item
 *   has to be a render array.
 *   - #settings: Slick Settings.
 */
function template_preprocess_slick(array &$variables) {
  $element = $variables['element'];
  $items = $element['#items'];
  $count = count($items);
  $settings = (isset($element['#settings'])) ? $element['#settings'] : array();

  // Set the ID for each slick instance, if none provided, for quick JS lookup.
  if (empty($variables['attributes']['id'])) {
    $id = (isset($element['#id'])) ? Html::getUniqueId($element['#id']) : Html::getUniqueId('slick');
    $variables['attributes']['id'] = $id;
  }

  // Process individual item.
  $build = array();
  foreach ($items as $delta => $item) {
    $build[$delta] = array(
      '#theme' => 'slick_item',
      '#item' => isset($item['slide']) ? $item['slide'] : $item,
      '#delta' => $delta,
      '#settings' => $settings,
      '#caption' => isset($item['caption']) ? $item['caption'] : array(),
    );
  }

  $variables['items'] = $build;

  $variables['settings'] = $settings;
  if (!empty($settings)) {
    $js = Json::encode($variables['settings']);
    $variables['attributes']['data-config'] = $js;
  }

  // Add basic library.
  if ($count > 1) {
    $variables['#attached']['library'][] = 'slick/drupal.slick';
  }
}

/**
 * Prepares variables for slick item templates.
 *
 * Default template: slick-item.html.twig.
 *
 * @param array $variables
 *   - #item: Slick Slide.
 *   - #caption: Slide Caption.
 *   - #settings: Slick Settings.
 */
function template_preprocess_slick_item(array &$variables) {
  $element = $variables['element'];
  $item = $element['#item'];
  $captions = $element['#caption'];
  $settings = $element['#settings'];
  $variables['delta'] = $element['#delta'];

  $variables['item'] = $item;
  $variables['item_prefix'] = array();
  $variables['item_suffix'] = array();

  $variables['tag'] = $variables['content_tag'] = 'div';
  $variables['content_attributes'] = new Attribute();

  $variables['css_name'] = Html::cleanCssIdentifier(implode('-', array('slick-slide', $variables['delta'])));

  // Add layout/ caption position classes if so configured.
  if (!empty($captions['layout'])) {
    $layout = Html::cleanCssIdentifier('slide-caption--' . $captions['layout']);
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

  $variables['settings'] = $settings;
}
