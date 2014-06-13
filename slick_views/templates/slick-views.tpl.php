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
  '#theme' => 'slick',
  '#items' => $rows,
  '#settings' => $options,
  '#attached' => $attach,
);

print render($element);
