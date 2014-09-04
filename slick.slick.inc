<?php

/**
 * @file
 * Default skins for the Slick carousel.
 */

/**
 * Implements hook_slick_skins_info().
 *
 * All the skins are very basic, to perform possible variants. It doesn't care
 * about looking great, and other element stylings, but basic one and layouts.
 * Detailed styling is all yours to match your actual design.
 * It should be used in relation to individual slide layout  to get most out of
 * it, see README.txt on slick_fields.module for possible Slide layout using a
 * field.
 *
 * Each skin supports 4 keys:
 * - name: The human readable name of the skin.
 * - description: The description about the skin.
 * - css: An array of CSS files to attach.
 * - js: An array of JS files to attach.
 *
 * @see https://drupal.org/node/1887922
 */
function slick_slick_skins_info() {
  $module_path = drupal_get_path('module', 'slick');

  $skins = array(
    'default' => array(
      'name' => t('Default'),
      'css' => array(
        $module_path . '/css/theme/slick.theme--default.css',
      ),
    ),
    'asnavfor' => array(
      'name' => t('Thumbnail: asNavFor'),
      'css' => array(
        $module_path . '/css/theme/slick.theme--asnavfor.css',
      ),
      'description' => t('Affected thumbnail navigation only.'),
    ),
    'classic' => array(
      'name' => t('Classic'),
      'description' => t('Adds dark background color over white caption, only good for slider (single slide visible), not carousel (multiple slides visible), where small captions are placed over images.'),
      'css' => array(
        $module_path . '/css/theme/slick.theme--classic.css',
      ),
    ),
    'fullwidth' => array(
      'name' => t('Full width'),
      'description' => t('Adds .slide__constrained wrapper to hold caption overlay within the max-container.'),
      'css' => array(
        $module_path . '/css/theme/slick.theme--fullwidth.css',
      ),
    ),
    'boxed' => array(
      'name' => t('Boxed'),
      'description' => t('Adds margins to the sides of slick-list revealing arrows.'),
      'css' => array(
        $module_path . '/css/theme/slick.theme--boxed.css',
      ),
    ),
    'split' => array(
      'name' => t('Split'),
      'description' => t('Puts image and caption side by side, related to slide layout options.'),
      'css' => array(
        $module_path . '/css/theme/slick.theme--split.css',
      ),
    ),
    'boxed-carousel' => array(
      'name' => t('Box carousel'),
      'description' => t('Carousel that has margins, alternative to centerMode.'),
      'css' => array(
        $module_path . '/css/theme/slick.theme--boxed.css',
        $module_path . '/css/theme/slick.theme--boxed--carousel.css',
      ),
    ),
    'boxed-split' => array(
      'name' => t('Box split'),
      'description' => t('Adds margins and split caption and image.'),
      'css' => array(
        $module_path . '/css/theme/slick.theme--boxed.css',
        $module_path . '/css/theme/slick.theme--split.css',
      ),
    ),
    'rounded' => array(
      'name' => t('Rounded'),
      'description' => t('Rounds the .slide__image, great for 3-5 visible-slides carousel.'),
      'css' => array(
        $module_path . '/css/theme/slick.theme--rounded.css',
      ),
    ),
  );

  return $skins;
}
