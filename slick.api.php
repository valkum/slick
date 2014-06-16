<?php

/**
 * @file
 * Hooks provided by the Slick module.
 *
 * Modules and themes may implement any of the available hooks to interact with
 * the Slick.
 */

/**
 * Register Slick skins.
 *
 * This hook can be used to register skins for the Slick. Skins will be
 * displayed and made selectable when configuring the formatter.
 *
 * Slick skins get a unique CSS class to use for styling, e.g.:
 * If you skin name is "my_module_slick_carousel_rounded", the class is:
 * slick--skin--my-module-slick-carousel-rounded
 *
 * A skin can specify an unlimited number of CSS and JS files to include when
 * the Slick is displayed.
 */
function hook_slick_skins_info() {
  return array(
    'skin_name' => array(
      // Human readable skin name.
      'name' => t('Skin name'),
      // Description of the skin.
      'description' => t('Skin description.'),
      'css' => array(
        // Full path to a CSS file to include with the skin.
        drupal_get_path('module', 'module_name') . '/css/module-name.slick.theme--slider.css',
        drupal_get_path('module', 'module_name') . '/css/module-name.slick.theme--carousel.css',
      ),
      'js' => array(
        // Full path to a JS file to include with the skin.
        drupal_get_path('module', 'module_name') . '/js/module-name.slick.theme--slider.js',
        drupal_get_path('module', 'module_name') . '/js/module-name.slick.theme--carousel.js',
      ),
    )
  );
}

/**
 * Alter Slick skins.
 *
 * @param $skins
 *   The associative array of skin information from hook_slick_skins_info().
 *
 * @see hook_slick_skins_info()
 */
function hook_slick_skins_info_alter(&$skins) {
  // Modify the default skin's name and description
  $skins['default']['name'] = t('My skin');
  $skins['default']['description'] = t('My owsem skin.');

  // Replace the default skin styling.
  // Namespace your asset files accordingly to avoid conflict since Drupal loads
  // CSS by basename.
  $skins['default']['css'] = drupal_get_path('module', 'module_name') . '/module-name.slick.theme--owsem.css';
}
