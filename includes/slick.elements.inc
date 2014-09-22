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
function _slick_optionset_defaults($key = NULL, $set = NULL, $breakpoints = NULL) {

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

  // Return the specific item out of the JS settings alone.
  if (isset($key) && $key && $set && array_key_exists($key, $defaults[$set])) {
    return $defaults[$set][$key];
  }

  // Return a specific group, or all items.
  return !empty($set) && $defaults[$set] ? $defaults[$set] : $defaults;
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
  $settings = $customs ?: $options['settings'];
  $config = array_diff_assoc($settings, $defaults['settings']);

  // Must be empty if all is default optionset.
  if ($config) {
    // Remove wasted dependent options if disabled, even if not empty.
    if (empty($config['autoplay'])) {
      unset($config['pauseOnHover'], $config['pauseOnDotsHover'], $config['autoplaySpeed']);
    }
    if (empty($config['centerMode'])) {
      unset($config['centerPadding']);
    }
    if (isset($config['arrows']) && !$config['arrows']) {
      unset($config['prevArrow'], $config['nextArrow'], $config['appendArrows']);
    }
    if (empty($config['dots'])) {
      unset($config['dotsClass'], $config['appendDots']);
    }
  }

  // Clean up responsive options if similar to main defaults. JS takes care of
  // the copy based on defaults. This way we can avoid dup options in the
  // markups.
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
      $cleaned = array();
      foreach ($cleaned_responsives as $key => $responsive) {
        $cleaned[$key]['breakpoint'] = $cleaned_responsives[$key]['breakpoint'];
        $cleaned[$key]['settings'] = array_diff_assoc($cleaned_responsives[$key]['settings'], $defaults['settings']);

        // Remove wasted breakpoint dependent options if disabled, even if not
        // empty.
        if (empty($cleaned[$key]['settings']['autoplay'])) {
          unset(
            $cleaned[$key]['settings']['pauseOnHover'],
            $cleaned[$key]['settings']['pauseOnDotsHover'],
            $cleaned[$key]['settings']['autoplaySpeed']
          );
        }
        if (empty($cleaned[$key]['settings']['centerMode'])) {
          unset($cleaned[$key]['settings']['centerPadding']);
        }
        if (empty($cleaned[$key]['settings']['dots'])) {
          unset(
            $cleaned[$key]['settings']['dotsClass'],
            $cleaned[$key]['settings']['appendDots']
          );
        }
      }

      // Make sure to follow JS array.
      $config['responsive'] = $cleaned;
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
  $easings = &drupal_static(__FUNCTION__, NULL);

  if (!isset($easings)) {
    $easings = array(
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
  return $easings;
}

/**
 * Gets a media URL from the rendered iframe already containing all the params.
 *
 * This is quicker than rebuilding media params per media contrib module.
 * Basically, to merge all media audio/video templates into one.
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
 * Defines a list of options available for slick.
 *
 * @see http://kenwheeler.github.io/slick
 */
function slick_get_options() {
  $options = array();

  $options['asNavFor'] = array(
    'title' => t('asNavFor target'),
    'description' => t('Set the slider to be the navigation of other slider (Class or ID Name). Use selector identifier ("." or "#") accordingly. If class, use the provided Wrapper class under General as needed, e.g.: if the main display has class "slick--for", and the thumbnail navigation "slick--nav", place the opposite here as its target. Or use existing classes based on optionsets, e.g.: .slick--optionset--main, or .slick--optionset--main-nav. Overriden per field formatter.'),
    'type' => 'textfield',
    'cast' => 'string',
    'default' => '',
  );

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
    'states' => array('visible' => array(':input[name*="options[settings][autoplay]"]' => array('checked' => TRUE))),
  );

  $options['pauseOnDotsHover'] = array(
    'title' => t('Pause on dots hover'),
    'description' => t('Pauses autoplay when a dot is hovered'),
    'type' => 'checkbox',
    'cast' => 'bool',
    'default' => FALSE,
    'states' => array('visible' => array(':input[name*="options[settings][autoplay]"]' => array('checked' => TRUE))),
  );

  $options['autoplaySpeed'] = array(
    'title' => t('Autoplay speed'),
    'description' => t('Autoplay Speed in milliseconds'),
    'type' => 'textfield',
    'cast' => 'int',
    'default' => 3000,
    'states' => array('visible' => array(':input[name*="options[settings][autoplay]"]' => array('checked' => TRUE))),
  );

  $options['arrows'] = array(
    'title' => t('Arrows'),
    'description' => t('Show prev/next arrows'),
    'type' => 'checkbox',
    'cast' => 'bool',
    'default' => TRUE,
  );

  $options['appendArrows'] = array(
    'title' => t('Append arrows'),
    'description' => t("Change where the navigation arrows are attached (Selector, htmlString, Array, Element, jQuery object). Leave it to default to wrap it within .slick__arrow container, otherwise change its markups accordingly."),
    'type' => 'textfield',
    'cast' => 'string',
    'default' => '$(element)',
    'states' => array('visible' => array(':input[name*="options[settings][arrows]"]' => array('checked' => TRUE))),
  );

  $options['prevArrow'] = array(
    'title' => t('Previous arrow'),
    'description' => t("Customize the previous arrow markups. Make sure to keep the expected class."),
    'type' => 'textfield',
    'cast' => 'string',
    'default' => '<button type="button" data-role="none" class="slick-prev">Previous</button>',
    'states' => array('visible' => array(':input[name*="options[settings][arrows]"]' => array('checked' => TRUE))),
  );

  $options['nextArrow'] = array(
    'title' => t('Next arrow'),
    'description' => t("Customize the next arrow markups. Make sure to keep the expected class"),
    'type' => 'textfield',
    'cast' => 'string',
    'default' => '<button type="button" data-role="none" class="slick-next">Next</button>',
    'states' => array('visible' => array(':input[name*="options[settings][arrows]"]' => array('checked' => TRUE))),
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
    'states' => array('visible' => array(':input[name*="options[settings][centerMode]"]' => array('checked' => TRUE))),
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

  $options['dotsClass'] = array(
    'title' => t('Dot class'),
    'description' => t('Class for slide indicator dots container. Do not prefix with dot. If you change this, edit its CSS accordingly.'),
    'type' => 'textfield',
    'cast' => 'string',
    'default' => 'slick-dots',
    'states' => array('visible' => array(':input[name*="options[settings][dots]"]' => array('checked' => TRUE))),
  );

  $options['appendDots'] = array(
    'title' => t('appendDots'),
    'description' => t('Change where the navigation dots are attached (Selector, htmlString, Array, Element, jQuery object). If you change this, make sure to provide its relevant markup.'),
    'type' => 'textfield',
    'cast' => 'string',
    'default' => '$(element)',
    'states' => array('visible' => array(':input[name*="options[settings][dots]"]' => array('checked' => TRUE))),
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
    'description' => t('Add easing for jQuery animate. Use with <a href="@easing">easing</a> libraries or default easing methods. Install <a href="@jqeasing">jqeasing module</a> for more options.', array('@jqeasing' => '//drupal.org/project/jqeasing', '@easing' => '//gsgd.co.uk/sandbox/jquery/easing/')),
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

  $options['focusOnSelect'] = array(
    'title' => t('Focus on select'),
    'description' => t('Enable focus on selected element (click)'),
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
    'description' => t("Set lazy loading technique. Accepts 'ondemand' or 'progressive'. Note: dummy image is no good for ondemand, and if ondemand fails to generate images, try progressive instead. If you need to share images for Pinterest, leave empty, otherwise Pinterest have no way to read image src."),
    'type' => 'select',
    'cast' => 'string',
    'default' => 'ondemand',
    'options' => drupal_map_assoc(array('ondemand', 'progressive')),
    'empty_option' => t('- None -'),
  );

  $options['rtl'] = array(
    'title' => t('RTL'),
    'description' => t("Change the slider's direction to become right-to-left"),
    'type' => 'checkbox',
    'cast' => 'bool',
    'default' => FALSE,
  );

  $options['slide'] = array(
    'title' => t('Slide element'),
    'description' => t("Element query to use as slide. Make sure to be specific with the slide item class, default to .slick__slide if using Fields formatter, or the provided theme_slick(). Otherwise Slick will wrap all DIV as slide item which is problematic to add extra stuffs like arrows container or thumbnail pagers."),
    'type' => 'textfield',
    'cast' => 'string',
    'default' => 'div',
  );

  $options['slidesToShow'] = array(
    'title' => t('Slides to show'),
    'description' => t('Number of slides to show at a time. If 1, it will behave like slideshow, more than 1 a carousel. Provide more if it is a thumbnail navigation with asNavFor. Only works with odd number slidesToShow counts when using centerMode.'),
    'type' => 'textfield',
    'cast' => 'int',
    'default' => 1,
  );

  $options['slidesToScroll'] = array(
    'title' => t('Slides to scroll'),
    'description' => t('Number of slides to scroll at a time, or steps at each scroll.'),
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
    'states' => array('visible' => array(':input[name*="options[settings][touchMove]"]' => array('checked' => TRUE))),
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

  $options['waitForAnimate'] = array(
    'title' => t('waitForAnimate'),
    'description' => t('Ignores requests to advance the slide while animating.'),
    'type' => 'checkbox',
    'cast' => 'bool',
    'default' => TRUE,
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
      'description' => t('Breakpoint settings'),
      'type' => 'fieldset',
    );

    // Duplicate main settings.
    // Clean up problematic dup options, since they have to be available once
    // for all. JS takes care of the copy on each responsive settings.
    unset(
      $slick_options['asNavFor'],
      $slick_options['slide'],
      $slick_options['lazyLoad'],
      $slick_options['prevArrow'],
      $slick_options['nextArrow'],
      $slick_options['appendArrows'],
      $slick_options['dotsClass'],
      $slick_options['appendDots'],
      $slick_options['rtl']
    );
    foreach ($slick_options as $name => $values) {
      $options[$key]['settings'][$name] = $values;
    }
  }

  return $options;
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
 * Converts hex to RGB.
 *
 * @param string $hex
 *   A string hex value.
 *
 * @return string
 *   A comma separated RGB color string value.
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
      $optionsets[$key] = check_plain($optionset->label);
    }
    asort($optionsets);
  }
  return $optionsets;
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
