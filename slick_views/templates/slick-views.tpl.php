<?php

/**
 * @file
 * Default theme implementation for the Slick views template.
 *
 * - $rows: The array of items.
 * - $options: Array of available settings via Views UI.
 * - $attach: Available conditional JS and CSS assets.
 */

// Pass the Slick Views data to theme_slick().
$element = array(
  '#theme' => 'slick_wrapper',
  '#items' => $indexes,
  '#settings' => $options,
);
    
$element[0] = array(
  '#theme' => 'slick',
  '#items' => $rows,
  '#settings' => $options,
  '#attached' => $attach,
);

if (isset($options['optionset_thumbnail']) && $options['optionset_thumbnail'] && $thumbs) {
  if ($options['id']) {
    $options['attributes']['id'] = $options['id'] . '-thumbnail';
  }
  $options['optionset'] = $options['optionset_thumbnail'];
  $options['current_display'] = 'thumbnail';
  $element[1] = array(
    '#theme' => 'slick',
    '#items' => $thumbs,
    '#settings' => $options,
    '#attached' => array(),
  );
}

print render($element);
