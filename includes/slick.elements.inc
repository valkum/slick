<?php

/**
 * @file
 * Slick elements.
 *
 * This file is called by admin and several front end functions.
 */

/**
 * Default settings for a newly created option set.
 *
 * @param string $key
 *   Get specific default value [optional].
 */
function _slick_optionset_defaults($key = NULL, $settings = NULL, $breakpoints = NULL) {

  // Main slick settings.
  $slick_options = slick_get_options();
  $defaults = array();
  foreach ($slick_options as $name => $values) {
    $defaults['settings'][$name] = $values['default'];
  }

  // Breakpoint settings.
  if ($breakpoints) {
    $slick_responsive_options = slick_get_responsive_options($breakpoints);
    foreach ($slick_responsive_options as $i => $items) {
      foreach ($items as $name => $groups) {
        if (is_array($groups)) {
          if (isset($groups['default'])) {
            $defaults['responsives']['responsive'][$i][$name] = $groups['default'];
          }
          else {
            foreach ($groups as $key => $group) {
              if (is_array($group) && isset($group['default'])) {
                $defaults['responsives']['responsive'][$i][$name][$key] = $group['default'];
              }
            }
          }
        }
      }
    }
  }

  $defaults['general']['normal'] = '';
  $defaults['general']['thumbnail'] = '';
  // $defaults['general']['transition'] = '';
  $defaults['general']['template_class'] = '';
  $defaults['general']['goodies'] = array();

  // Typecast the values.
  _slick_typecast_optionset($defaults, $breakpoints);

  // Return the specific item.
  if (isset($key) && !empty($key) && array_key_exists($key, $defaults)) {
    return $defaults[$key];
  }

  // Return a specific group, or all items.
  return !empty($settings) && $defaults[$settings] ? $defaults[$settings] : $defaults;
}

/**
 * Strip out options with default values from the Option Set options.
 *
 * @param object $optionset
 *   An array of Option Set options, default is loaded if none specified.
 *
 * @return array
 *   An array of reduced duplicated Option set options performance wise.
 */
function _slick_remove_default_optionset_options($optionset, $customs = NULL) {
  $config = array();

  $options = $optionset->options;
  $breakpoints = $optionset->breakpoints ? $optionset->breakpoints : 0;
  $defaults = _slick_optionset_defaults(NULL, NULL, $breakpoints);

  // Clean up the main options.
  $settings = $customs ? $customs : $options['settings'];
  $config = array_diff_assoc($settings, $defaults['settings']);

  // Must be empty if all is default optionset.
  if ($config) {
    // Anon DIV is problematic, since Slick will slide every div element
    // within its container which makes it impossible to add a div to wrap
    // buttons (such as arrow down) nor wrap them for advanced customPaging.
    // Hence we override with more specific class .slick__slide instead.
    // Having list type is irrelevant to the JS generated markups.
    if (isset($config['slide']) && ($config['slide'] == 'div' || empty($config['slide']))) {
      $config['slide'] = '.slick__slide';
    }

    // Remove wasted dependent options if disabled, even if not empty.
    if (empty($config['autoplay'])) {
      unset($config['pauseOnHover'], $config['autoplaySpeed']);
    }
    if (empty($config['centerMode'])) {
      unset($config['centerPadding']);
    }
  }

  // Clean up responsive options if similar to main defaults.
  $cleaned_responsives = array();
  if (isset($options['responsives']['responsive'])) {
    $responsives = $options['responsives'];
    foreach ($responsives['responsive'] as $key => $responsive) {
      if (empty($responsives['responsive'][$key]['breakpoint'])) {
        unset($responsives['responsive'][$key]);
      }
      if (isset($responsives['responsive'][$key])) {
        $cleaned_responsives[$key] = $responsive;
      }
    }
    if ($cleaned_responsives) {
      $cleaned_responsive_settings = array();
      foreach ($cleaned_responsives as $key => $responsive) {
        $cleaned_responsive_settings[$key]['breakpoint'] = $cleaned_responsives[$key]['breakpoint'];
        $cleaned_responsive_settings[$key]['settings'] = array_diff_assoc($cleaned_responsives[$key]['settings'], $defaults['settings']);
      }
      // Make sure to follow JS array.
      $config['responsive'] = $cleaned_responsive_settings;
    }
  }

  return $config;
}

/**
 * Adds the typecasting to the given values.
 *
 * With this, the generated JSON object has the right values.
 */
function _slick_typecast_optionset(&$options, $breakpoints = NULL) {
  if (isset($options) && !empty($options)) {
    $slick_options = slick_get_options();
    foreach ($slick_options as $name => $values) {
      if (isset($slick_options[$name]['cast'])) {
        settype($options['settings'][$name], $slick_options[$name]['cast']);
      }
    }

    if (isset($options['responsives']['responsive']) && $breakpoints && ($breakpoints > 0)) {
      $slick_responsive_options = slick_get_responsive_options($breakpoints);
      foreach ($slick_responsive_options as $i => $items) {
        foreach ($items as $name => $groups) {
          if (is_array($groups)) {

            if ($name == 'breakpoint' && isset($groups['cast'])) {
              settype($options['responsives']['responsive'][$i][$name], $groups['cast']);
            }
            elseif ($name == 'settings') {
              foreach ($groups as $key => $group) {
                if (is_array($group) && isset($group['cast']) && isset($group['default'])) {
                  settype($options['responsives']['responsive'][$i][$name][$key], $group['cast']);
                }
              }
            }
          }
        }
      }
    }
  }
}

/**
 * Gets supported Slick skins, and additional ones.
 */
function slick_skins($options = FALSE) {
  $skins = &drupal_static(__FUNCTION__, NULL);

  if (!isset($skins)) {
    // Collects a list of the currently available skins.
    $skins = module_invoke_all('slick_skins_info');

    // Allow skin information to be altered.
    drupal_alter('slick_skins_info', $skins);

    if ($options) {
      $available_skins = array();
      foreach ($skins as $skin => $properties) {
        $available_skins[$skin] = $properties['name'];
      }
      $skins = $available_skins;
    }
  }

  return $skins;
}

/**
 * Gets default layout options for the core Image, or Views.
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
      // Zebras are for less flexible like core image.
      'stage-zebra' => t('Stage zebra'),
      'split-zebra' => t('Split half zebra'),
    );
  }

  return $layouts;
}

/**
 * List of all custom transition effects.
 * @todo drop it or test elementTransition.js
 */
function _slick_transition_options() {
  return array();
}

/**
 * List of all easing methods available from jQuery Easing v1.3.
 */
function _slick_easing_options() {
  return array(
    '' => 'None',
    'linear' => 'Linear',
    'swing' => 'Swing',
    'easeInQuad' => 'easeInQuad',
    'easeOutQuad' => 'easeOutQuad',
    'easeInOutQuad' => 'easeInOutQuad',
    'easeInCubic' => 'easeInCubic',
    'easeOutCubic' => 'easeOutCubic',
    'easeInOutCubic' => 'easeInOutCubic',
    'easeInQuart' => 'easeInQuart',
    'easeOutQuart' => 'easeOutQuart',
    'easeInOutQuart' => 'easeInOutQuart',
    'easeInQuint' => 'easeInQuint',
    'easeOutQuint' => 'easeOutQuint',
    'easeInOutQuint' => 'easeInOutQuint',
    'easeInSine' => 'easeInSine',
    'easeOutSine' => 'easeOutSine',
    'easeInOutSine' => 'easeInOutSine',
    'easeInExpo' => 'easeInExpo',
    'easeOutExpo' => 'easeOutExpo',
    'easeInOutExpo' => 'easeInOutExpo',
    'easeInCirc' => 'easeInCirc',
    'easeOutCirc' => 'easeOutCirc',
    'easeInOutCirc' => 'easeInOutCirc',
    'easeInElastic' => 'easeInElastic',
    'easeOutElastic' => 'easeOutElastic',
    'easeInOutElastic' => 'easeInOutElastic',
    'easeInBack' => 'easeInBack',
    'easeOutBack' => 'easeOutBack',
    'easeInOutBack' => 'easeInOutBack',
    'easeInBounce' => 'easeInBounce',
    'easeOutBounce' => 'easeOutBounce',
    'easeInOutBounce' => 'easeInOutBounce',
  );
}

/**
 * Gets a media URL from the rendered iframe already containing all the params.
 *
 * This is quicker than rebuilding media params per media contrib module.
 * Basically, to merge all media audio/video templates into one.
 */
function slick_get_media_url($rendered_media) {
  $url = '';

  if (!empty($rendered_media)) {
    $dom = new DOMDocument();
    libxml_use_internal_errors(TRUE);
    $dom->loadHTML($rendered_media);
    $url = $dom->getElementsByTagName('iframe')->item(0)->getAttribute('src');
  }

  return $url;
}

/**
 * Defines a list of options available for slick.
 *
 * @see http://kenwheeler.github.io/slick
 */
function slick_get_options() {
  $options = array();

  $options['accessibility'] = array(
    'title' => t('Accessibility'),
    'description' => t('Enables tabbing and arrow key navigation'),
    'type' => 'checkbox',
    'cast' => 'bool',
    'default' => TRUE,
  );

  $options['autoplay'] = array(
    'title' => t('Autoplay'),
    'description' => t('Enables Autoplay'),
    'type' => 'checkbox',
    'cast' => 'bool',
    'default' => FALSE,
  );

  $options['pauseOnHover'] = array(
    'title' => t('Pause on hover'),
    'description' => t('Pause Autoplay On Hover'),
    'type' => 'checkbox',
    'cast' => 'bool',
    'default' => TRUE,
    'states' => array('visible' => array(':input[name*="autoplay"]' => array('checked' => TRUE))),
  );

  $options['autoplaySpeed'] = array(
    'title' => t('Autoplay speed'),
    'description' => t('Autoplay Speed in milliseconds'),
    'type' => 'textfield',
    'cast' => 'int',
    'default' => 3000,
    'states' => array('visible' => array(':input[name*="autoplay"]' => array('checked' => TRUE))),
  );

  $options['arrows'] = array(
    'title' => t('Arrows'),
    'description' => t('Show prev/next arrows'),
    'type' => 'checkbox',
    'cast' => 'bool',
    'default' => TRUE,
  );

  $options['centerMode'] = array(
    'title' => t('Center mode'),
    'description' => t('Enables centered view with partial prev/next slides. Use with odd numbered slidesToShow counts.'),
    'type' => 'checkbox',
    'cast' => 'bool',
    'default' => FALSE,
  );

  $options['centerPadding'] = array(
    'title' => t('Center padding'),
    'description' => t('Side padding when in center mode (px or %)'),
    'type' => 'textfield',
    'cast' => 'string',
    'default' => '50px',
    'states' => array('visible' => array(':input[name*="centerMode"]' => array('checked' => TRUE))),
  );

  $options['cssEase'] = array(
    'title' => t('cssEase'),
    'description' => t('CSS3 Animation Easing'),
    'type' => 'textfield',
    'cast' => 'string',
    'default' => 'ease',
  );

  $options['dots'] = array(
    'title' => t('Dots'),
    'description' => t('Show dot indicators'),
    'type' => 'checkbox',
    'cast' => 'bool',
    'default' => FALSE,
  );

  $options['draggable'] = array(
    'title' => t('Draggable'),
    'description' => t('Enable mouse dragging'),
    'type' => 'checkbox',
    'cast' => 'bool',
    'default' => TRUE,
  );

  $options['easing'] = array(
    'title' => t('Easing'),
    'description' => t('Add easing for jQuery animate. Use with <a href="@easing">easing</a> libraries or default easing methods. Install <a href="@jqeasing">jqeasing module</a> for more options. Or see Goodies under general option to load easing from CDN.', array('@jqeasing' => '//drupal.org/project/jqeasing', '@easing' => '//gsgd.co.uk/sandbox/jquery/easing/')),
    'type' => 'select',
    'cast' => 'string',
    'default' => 'linear',
    'options' => _slick_easing_options(),
  );

  $options['fade'] = array(
    'title' => t('Fade'),
    'description' => t('Enable fade'),
    'type' => 'checkbox',
    'cast' => 'bool',
    'default' => FALSE,
  );

  $options['infinite'] = array(
    'title' => t('Infinite'),
    'description' => t('Infinite loop sliding'),
    'type' => 'checkbox',
    'cast' => 'bool',
    'default' => TRUE,
  );

  $options['lazyLoad'] = array(
    'title' => t('Lazy load'),
    'description' => t("Set lazy loading technique. Accepts 'ondemand' or 'progressive'"),
    'type' => 'select',
    'cast' => 'string',
    'default' => 'ondemand',
    'options' => drupal_map_assoc(array('ondemand', 'progressive')),
  );

  // Play safe to use .slick__slide, rather than problematic anonymous div.
  $options['slide'] = array(
    'title' => t('Slide element'),
    'description' => t("Element query to use as slide. Make sure to be specific with the slide item class, default to .slick__slide if using Fields formatter, or the provided theme_slick(). Otherwise Slick will wrap all DIV as slide item which is problematic to add extra stuffs like arrows container or thumbnail pagers."),
    'type' => 'textfield',
    'cast' => 'string',
    'default' => 'div',
  );

  $options['slidesToShow'] = array(
    'title' => t('Slides to show'),
    'description' => t('Number of slides to show. If 1, it will behave like slideshow, more than 1 a carousel.'),
    'type' => 'textfield',
    'cast' => 'int',
    'default' => 1,
  );

  $options['slidesToScroll'] = array(
    'title' => t('Slides to scroll'),
    'description' => t('Number of slides to scroll, or steps at each scroll.'),
    'type' => 'textfield',
    'cast' => 'int',
    'default' => 1,
  );

  $options['speed'] = array(
    'title' => t('Speed'),
    'description' => t('Slide/Fade animation speed in milliseconds'),
    'type' => 'textfield',
    'cast' => 'int',
    'default' => 300,
    'field_suffix' => 'ms',
  );

  $options['swipe'] = array(
    'title' => t('Swipe'),
    'description' => t('Enable swiping'),
    'type' => 'checkbox',
    'cast' => 'bool',
    'default' => TRUE,
  );

  $options['touchMove'] = array(
    'title' => t('Touch move'),
    'description' => t('Enable slide motion with touch'),
    'type' => 'checkbox',
    'cast' => 'bool',
    'default' => TRUE,
  );

  $options['touchThreshold'] = array(
    'title' => t('Touch threshold'),
    'description' => t('Swipe distance threshold'),
    'type' => 'textfield',
    'cast' => 'int',
    'default' => 5,
    'states' => array('visible' => array(':input[name*="touchMove"]' => array('checked' => TRUE))),
  );

  $options['useCSS'] = array(
    'title' => t('Use CSS'),
    'description' => t('Enable/Disable CSS Transitions'),
    'type' => 'checkbox',
    'cast' => 'bool',
    'default' => TRUE,
  );

  $options['vertical'] = array(
    'title' => t('Vertical'),
    'description' => t('Vertical slide direction'),
    'type' => 'checkbox',
    'cast' => 'bool',
    'default' => FALSE,
  );

  return $options;
}

/**
 * Defines a list of options available for the responsive Slick.
 */
function slick_get_responsive_options($count = 0) {
  $options = array();

  $breakpoints = drupal_map_assoc(range(0, ($count - 1)));
  $slick_options = slick_get_options();

  foreach ($breakpoints as $key => $breakpoint) {
    $options[$key] = array(
      'title' => t('Breakpoint #@key', array('@key' => ($key + 1))),
      'type' => 'fieldset',
    );

    $options[$key]['breakpoint'] = array(
      'title' => t('Breakpoint'),
      'description' => t('Breakpoint'),
      'type' => 'textfield',
      'cast' => 'int',
      'default' => '',
      'field_suffix' => 'px',
    );

    $options[$key]['settings'] = array(
      'title' => t('Settings'),
      'description' => t('Settings'),
      'type' => 'fieldset',
    );

    // Duplicate main settings.
    foreach ($slick_options as $name => $values) {
      $options[$key]['settings'][$name] = $values;
    }
  }

  return $options;
}

/**
 * Create a new optionset object.
 *
 * Note that this function does not save the optionset to the database.
 * @see slick_optionset_save()
 */
function slick_optionset_create($values = array()) {
  ctools_include('export');
  $optionset = ctools_export_crud_new('slick_optionset');

  // Set the options to an array.
  $optionset->options = array();

  // Assign specified values.
  if (isset($values['name'])) {
    $optionset->name = $values['name'];
  }

  if (isset($values['label'])) {
    $optionset->label = $values['label'];
  }

  $optionset->breakpoints = 0;
  if (isset($values['breakpoints'])) {
    $optionset->breakpoints = $values['breakpoints'];
  }

  if (isset($values['options']) and is_array($values['options'])) {
    $optionset->options = $values['options'];
  }

  // Merge default settings with any given settings.
  $optionset_defaults = _slick_optionset_defaults(NULL, NULL, $optionset->breakpoints);
  $optionset->options = $optionset_defaults += $optionset->options;

  return $optionset;
}

/**
 * Fetches all option sets from the database.
 *
 * @return array
 *   The associative array of all options.
 */
function slick_optionset_load_all() {
  ctools_include('export');
  $optionsets = ctools_export_crud_load_all('slick_optionset');
  foreach ($optionsets as $optionset) {
    // Ensure the optionset is typecast after being loaded from DB.
    _slick_typecast_optionset($optionset->options, $optionset->breakpoints);
  }
  return $optionsets;
}

/**
 * Fetches the given option set.
 *
 * @return object
 *   Returns it as an object or NULL, if no set could be found.
 */
function slick_optionset_load($optionset_name) {
  ctools_include('export');
  $optionset = ctools_export_crud_load('slick_optionset', $optionset_name);
  // Ensure the optionset is typecast after being loaded from DB.
  _slick_typecast_optionset($optionset->options, $optionset->breakpoints);
  return $optionset;
}

/**
 * Checks whether an option set with the given name already exists.
 */
function slick_optionset_exists($optionset_name) {
  ctools_include('export');
  $optionset = ctools_export_crud_load('slick_optionset', $optionset_name);
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
 * @param object $optionset
 *   Optionset object or machine name.
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

/**
 * Implements hook_image_default_styles().
 */
function slick_image_default_styles() {
  $styles = array();

  // Default image preset for Slick carousel.
  $styles['slick'] = array(
    'effects' => array(
      array(
        'name' => 'image_scale_and_crop',
        'data' => array(
          'width' => 1380,
          'height' => 420,
        ),
        'weight' => 0,
      ),
    ),
  );

  return $styles;
}
