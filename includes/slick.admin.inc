<?php
/**
 * @file
 * Contains optional functions called only if needed by admin pages.
 */

use Drupal\Component\Utility\String;

/**
 * Gets default layout options for the core Image, or Views.
 *
 * @return array
 *   An array of available basic layouts.
 */
function slick_layouts() {
  $layouts = &drupal_static(__FUNCTION__, NULL);

  if (!isset($layouts)) {
    $layouts = array(
      'bottom'      => t('Caption bottom'),
      'top'         => t('Caption top'),
      'right'       => t('Caption right'),
      'left'        => t('Caption left'),
      'center'      => t('Caption center'),
      'center-top'  => t('Caption center top'),
      'below'       => t('Caption below the slide'),
      'stage-right' => t('Caption left, stage right'),
      'stage-left'  => t('Caption right, stage left'),
      'split-right' => t('Caption left, stage right, split half'),
      'split-left'  => t('Caption right, stage left, split half'),
      'stage-zebra' => t('Stage zebra'),
      'split-zebra' => t('Split half zebra'),
    );
  }

  return $layouts;
}

/**
 * Load all slick optionsets as select options.
 *
 * @return array
 *   An array containing slicks indexed by their names.
 */
function slick_optionset_options() {
  $optionsets = &drupal_static(__FUNCTION__, NULL);

  if (!isset($optionsets)) {
    $optionsets = array();
    $slicks = slick_optionset_load_all();
    foreach ($slicks as $key => $optionset) {
      $optionsets[$key] = String::checkPlain($optionset->label);
    }
    asort($optionsets);
  }

  return $optionsets;
}

/**
 * Checks whether an option set with the given name already exists.
 */
function slick_optionset_exists($optionset_name) {
  $optionset = entity_load('slick_optionset', $optionset_name);
  return isset($optionset->name);
}

/**
 * Saves the given option set to the database.
 *
 * Set the $new flag if this set has not been written before.
 *
 * @return object
 *   Returns the newly saved object, FALSE otherwise.
 */
function slick_optionset_save($optionset, $new = FALSE) {
  // If the machine name is missing or already in use, return an error.
  if (empty($optionset->name) or (FALSE != slick_optionset_exists($optionset->name) and $new)) {
    return FALSE;
  }

  // Check for an invalid list of options.
  if (isset($optionset->options) and !is_array($optionset->options)) {
    return FALSE;
  }

  if (empty($optionset->label)) {
    $optionset->label = $optionset->name;
  }

  // Merge default settings with any given settings.
  $breakpoints = 0;
  if (isset($optionset->breakpoints)) {
    $breakpoints = $optionset->breakpoints;
  }

  $optionset_defaults = _slick_optionset_defaults(NULL, NULL, $breakpoints);
  $optionset->options = $optionset_defaults += $optionset->options;

  // Prepare the database values.
  $db_values = array(
    'name' => $optionset->name,
    'label' => $optionset->label,
    'breakpoints' => $breakpoints,
    'options' => _slick_typecast_optionset($optionset->options, $breakpoints),
  );

  if ($new) {
    $result = drupal_write_record('slick_optionset', $db_values);
  }
  else {
    $result = drupal_write_record('slick_optionset', $db_values, 'name');
  }

  // Return the object if the values were saved successfully.
  if (($new and SAVED_NEW == $result) or (!$new and SAVED_UPDATED == $result)) {
    return $optionset;
  }

  // Otherwise, an error occured.
  return FALSE;
}

/**
 * Deletes the given option set from the database.
 *
 * @param string|object $optionset
 *   Optionset object, or machine name.
 */
function slick_optionset_delete($optionset) {
  if (isset($optionset->name)) {
    $name = $optionset->name;
  }
  else {
    $name = $optionset;
  }
  db_delete('slick_optionset')->condition('name', $name)->execute();
}
