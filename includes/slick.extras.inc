<?php

/**
 * @file
 * Slick extras.
 *
 * Contains optional functions called only if needed.
 */

/**
 * Gets a media URL from the rendered iframe already containing all the params.
 *
 * This is quicker than rebuilding media params per media contrib module.
 * Basically, to merge all media audio/video templates into one.
 *
 * @param string $rendered_media
 *   The HTML markups of media output, mostly are iframes.
 *
 * @return string
 *   A URL extracted from the iframe SRC attribute.
 */
function slick_get_media_url($rendered_media) {
  $url = '';

  if (!empty($rendered_media) && strpos($rendered_media, 'src') !== FALSE) {
    $dom = new DOMDocument();
    libxml_use_internal_errors(TRUE);
    $dom->loadHTML($rendered_media);
    $url = $dom->getElementsByTagName('iframe')->item(0)->getAttribute('src');
  }

  return $url;
}

/**
 * Adjusts colors by step/number (0-255).
 *
 * @param string $hex
 *   A string hex value.
 *
 * @param int $steps
 *   An int step between -255 and 255 where negative means darker, and positive
 *   lighter.
 *
 * @return string
 *   A new hex color string value dependent on steps.
 */
function slick_color_brightness($hex, $steps) {
  // Steps should be -255 and 255.
  $steps = max(-255, min(255, $steps));

  // Format the hex color string.
  $hex = str_replace('#', '', $hex);
  if (strlen($hex) == 3) {
    $hex = str_repeat(substr($hex, 0, 1), 2) . str_repeat(substr($hex, 1, 1), 2) . str_repeat(substr($hex, 2, 1), 2);
  }

  // Get decimal values.
  $r = hexdec(substr($hex, 0, 2));
  $g = hexdec(substr($hex, 2, 2));
  $b = hexdec(substr($hex, 4, 2));

  // Adjust number of steps and keep it inside 0 to 255.
  $r = max(0, min(255, $r + $steps));
  $g = max(0, min(255, $g + $steps));
  $b = max(0, min(255, $b + $steps));

  $r_hex = str_pad(dechex($r), 2, '0', STR_PAD_LEFT);
  $g_hex = str_pad(dechex($g), 2, '0', STR_PAD_LEFT);
  $b_hex = str_pad(dechex($b), 2, '0', STR_PAD_LEFT);

  return '#' . $r_hex . $g_hex . $b_hex;
}

/**
 * Example to return the settings for a slick instance if using old approach.
 *
 * @param array $settings
 *   An array of the Slick library JS settings.
 *
 * We don't use regular JS settings injection into the <head>.
 * Instead we add individual settings as JSON object ready to consume by jQuery
 * via HTML5 data attributes, at slick container via #attached property to get
 * along well with Drupal 8.
 *
 * This is only needed if you want to use regular JS <head> injection outside
 * theme_slick(). In this case you have to load/ call it accordingly.
 * However it is recommended to use #attached property passed to theme_slick().
 *
 * @todo drop unsupported functions.
 */
function _slick_add_settings($settings = NULL) {
  static $already_added = FALSE;

  // Don't add the JavaScript and CSS multiple times.
  if ($already_added) {
    return;
  }

  if (empty($settings)) {
    $settings = _slick_optionset_defaults(NULL, 'settings');
  }

  drupal_add_js(array('slick' => $settings), 'setting');
  $already_added = TRUE;
}

/**
 * Converts hex to RGB.
 *
 * @param string $hex
 *   A string hex value.
 *
 * @return string
 *   A comma separated RGB color string value.
 * @todo drop unsupported functions.
 */
function slick_hex2rgb($hex) {
  $hex = str_replace("#", "", $hex);

  if (strlen($hex) == 3) {
    $r = hexdec(substr($hex, 0, 1) . substr($hex, 0, 1));
    $g = hexdec(substr($hex, 1, 1) . substr($hex, 1, 1));
    $b = hexdec(substr($hex, 2, 1) . substr($hex, 2, 1));
  }
  else {
    $r = hexdec(substr($hex, 0, 2));
    $g = hexdec(substr($hex, 2, 2));
    $b = hexdec(substr($hex, 4, 2));
  }
  $rgb = array($r, $g, $b);

  return implode(",", $rgb);
}

/**
 * List of all custom transition effects.
 * @todo drop it or test elementTransition.js, or other alternatives.
 */
function _slick_transition_options() {
  return array();
}
