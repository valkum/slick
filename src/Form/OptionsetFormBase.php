<?php
/**
 * @file
 * Contains \Drupal\slick\OptionsetFormController.
 */

namespace Drupal\slick\Form;

use Drupal\Core\Entity\EntityStorageInterface;
use Drupal\Core\Entity\EntityForm;
use Drupal\Core\Form\FormStateInterface;
use Symfony\Component\DependencyInjection\ContainerInterface;

/**
 * Base form for optionset add and edit forms.
 */
abstract class OptionsetFormBase extends EntityForm {
  /**
   * The entity being used by this form.
   *
   * @var \Drupal\image\ImageStyleInterface
   */
  protected $entity;

  /**
   * The image style entity storage.
   *
   * @var \Drupal\Core\Entity\EntityStorageInterface
   */
  protected $optionsetStorage;

  /**
   * Constructs a base class for image style add and edit forms.
   *
   * @param \Drupal\Core\Entity\EntityStorageInterface $optionset_storage
   *   The image style entity storage.
   */
  public function __construct(EntityStorageInterface $optionset_storage) {
    $this->optionsetStorage = $optionset_storage;
  }

  /**
   * {@inheritdoc}
   */
  public static function create(ContainerInterface $container) {
    return new static(
      $container->get('entity.manager')->getStorage('slick_optionset')
    );
  }

  /**
   * {@inheritdoc}
   */
  public function form(array $form, FormStateInterface $form_state) {
    // @todo: JS and CSS needs fix for D8.
    // $form['#attached']['library'][] = 'slick/slick.admin';

    $form['#attributes']['class'][] = 'no-js';
    $form['#attributes']['class'][] = 'form--slick';
    $form['#attributes']['class'][] = 'form--compact';
    $form['#attributes']['class'][] = 'form--optionset';
    $form['#attributes']['class'][] = 'clearfix';

    $form['label'] = array(
      '#type' => 'textfield',
      '#title' => $this->t('Optionset name'),
      '#default_value' => $this->entity->label(),
      '#required' => TRUE,
      '#attributes' => array('is-tooltip'),
    );
    $form['id'] = array(
      '#type' => 'machine_name',
      '#machine_name' => array(
        'exists' => array($this->optionsetStorage, 'load'),
      ),
      '#default_value' => $this->entity->id(),
      '#required' => TRUE,
      '#attributes' => array('is-tooltip'),
    );

    // Options.
    // Skins. We don't provide skin_thumbnail as each optionset may be deployed
    // as main or thumbnail navigation.
    // @todo: Add README link.
    $skins = slick_skins(TRUE);
    $form['skin'] = array(
      '#type' => 'select',
      '#title' => t('Skin'),
      '#options' => $skins,
      '#default_value' => $optionset->skin,
      '#empty_option' => t('- None -'),
      '#description' => t('Skins allow swappable layouts like next/prev links, split image and caption, etc. Make sure to provide a dedicated slide layout per field. However a combination of skins and options may lead to unpredictable layouts, get dirty yourself. See main <a href="#">README</a> for details on Skins. Keep it simple for thumbnail navigation skin.'),
      '#attributes' => array('class' => array('is-tooltip')),
    );
    $form['breakpoints'] = array(
      '#title' => t('Breakpoints'),
      '#type' => 'textfield',
      '#description' => t('The number of breakpoints added to Responsive display, max 9. This is not Breakpoint Width (480px, etc).'),
      '#default_value' => $form_state->getValue('breakpoints', $optionset->breakpoints),
      '#suffix' => '</div>',
      '#ajax' => array(
        'callback' => 'slick_add_breakpoints_ajax_callback',
        'wrapper' => 'breakpoints-ajax-wrapper',
        'event' => 'blur',
      ),
      '#attributes' => array('class' => array('is-tooltip')),
      '#maxlength' => 1,
    );

    $form['settings'] = array(
      '#type' => 'vertical_tabs',
      '#tree' => TRUE,
    );

    $settings = $this->entity->settings;

    // Main options.
    $slick_options = $this->getBasicOptions();
    $form['settings'] = array(
      '#title' => t('Settings'),
      '#type' => 'fieldset',
      '#collapsible' => FALSE,
      '#tree' => TRUE,
      '#attributes' => array('class' => array('fieldset--settings', 'has-tooltip')),
    );

    foreach ($slick_options as $name => $values) {
      $form['settings'][$name] = array(
        '#title' => $values['title'],
        '#description' => $values['description'],
        '#type' => $values['type'],
        '#default_value' => isset($settings[$name]) ? $settings[$name] : $values['default'],
        '#attributes' => array('class' => array('is-tooltip')),
      );

      if (isset($values['field_suffix'])) {
        $form['settings'][$name]['#field_suffix'] = $values['field_suffix'];
      }

      if ($values['type'] == 'textfield') {
        $form['settings'][$name]['#size'] = 20;
        $form['settings'][$name]['#maxlength'] = 255;
      }

      if (!isset($values['field_suffix']) && $values['cast'] == 'bool') {
        $form['settings'][$name]['#field_suffix'] = '';
        $form['settings'][$name]['#title_display'] = 'before';
      }

      if ($values['cast'] == 'int') {
        $form['settings'][$name]['#maxlength'] = 60;
        $form['settings'][$name]['#attributes']['class'][] = 'form-text--int';
      }

      if (isset($values['states'])) {
        $form['settings'][$name]['#states'] = $values['states'];
      }

      if (isset($values['options'])) {
        $form['settings'][$name]['#options'] = $values['options'];
      }

      if (isset($values['empty_option'])) {
        $form['settings'][$name]['#empty_option'] = $values['empty_option'];
      }

      // Expand textfield for easy edit.
      if (in_array($name, array('prevArrow', 'nextArrow'))) {
        $form['settings'][$name]['#attributes']['class'][] = 'js-expandable';
      }
    }


    return parent::form($form, $form_state);
  }

  /**
   * {@inheritdoc}
   */
  public function save(array $form, FormStateInterface $form_state) {
    parent::save($form, $form_state);
    $form_state->setRedirectUrl($this->entity->urlInfo('edit-form'));
  }


  public function getBasicOptions() {
    $options = &drupal_static(__FUNCTION__, NULL);

    if (!isset($options)) {
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
        'description' => t('Enables tabbing and arrow key navigation.'),
        'type' => 'checkbox',
        'cast' => 'bool',
        'default' => TRUE,
      );

      $options['adaptiveHeight'] = array(
        'title' => t('Adaptive height'),
        'description' => t('Enables adaptive height for single slide horizontal carousels.'),
        'type' => 'checkbox',
        'cast' => 'bool',
        'default' => FALSE,
      );

      $options['autoplay'] = array(
        'title' => t('Autoplay'),
        'description' => t('Enables autoplay.'),
        'type' => 'checkbox',
        'cast' => 'bool',
        'default' => FALSE,
      );

      $options['autoplaySpeed'] = array(
        'title' => t('Autoplay speed'),
        'description' => t('Autoplay speed in milliseconds.'),
        'type' => 'textfield',
        'cast' => 'int',
        'default' => 3000,
        'states' => array('visible' => array(':input[name*="options[settings][autoplay]"]' => array('checked' => TRUE))),
      );

      $options['pauseOnHover'] = array(
        'title' => t('Pause on hover'),
        'description' => t('Pause autoplay on hover.'),
        'type' => 'checkbox',
        'cast' => 'bool',
        'default' => TRUE,
        'states' => array('visible' => array(':input[name*="options[settings][autoplay]"]' => array('checked' => TRUE))),
      );

      $options['pauseOnDotsHover'] = array(
        'title' => t('Pause on dots hover'),
        'description' => t('Pauses autoplay when a dot is hovered.'),
        'type' => 'checkbox',
        'cast' => 'bool',
        'default' => FALSE,
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
        'description' => t("Customize the next arrow markups. Make sure to keep the expected class."),
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
        'description' => t('Side padding when in center mode (px or %).'),
        'type' => 'textfield',
        'cast' => 'string',
        'default' => '50px',
        'states' => array('visible' => array(':input[name*="options[settings][centerMode]"]' => array('checked' => TRUE))),
      );

      $options['dots'] = array(
        'title' => t('Dots'),
        'description' => t('Show dot indicators.'),
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
        'title' => t('Append dots'),
        'description' => t('Change where the navigation dots are attached (Selector, htmlString, Array, Element, jQuery object). If you change this, make sure to provide its relevant markup.'),
        'type' => 'textfield',
        'cast' => 'string',
        'default' => '$(element)',
        'states' => array('visible' => array(':input[name*="options[settings][dots]"]' => array('checked' => TRUE))),
      );

      $options['draggable'] = array(
        'title' => t('Draggable'),
        'description' => t('Enable mouse dragging.'),
        'type' => 'checkbox',
        'cast' => 'bool',
        'default' => TRUE,
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
        'description' => t('Enable focus on selected element (click).'),
        'type' => 'checkbox',
        'cast' => 'bool',
        'default' => FALSE,
      );

      $options['infinite'] = array(
        'title' => t('Infinite'),
        'description' => t('Infinite loop sliding.'),
        'type' => 'checkbox',
        'cast' => 'bool',
        'default' => TRUE,
      );

      $options['initialSlide'] = array(
        'title' => t('Initial slide'),
        'description' => t('Slide to start on.'),
        'type' => 'textfield',
        'cast' => 'int',
        'default' => 0,
      );

      $options['lazyLoad'] = array(
        'title' => t('Lazy load'),
        'description' => t("Note: dummy image is no good for ondemand. If ondemand fails to generate images, try progressive instead. Or use <a href='@url' target='_blank'>imageinfo_cache</a>. To share images for Pinterest, leave empty, otherwise no way to read actual image src.", array('@url' => '//www.drupal.org/project/imageinfo_cache')),
        'type' => 'select',
        'cast' => 'string',
        'default' => 'ondemand',
        'options' => array_combine(array('ondemand', 'progressive'), array('ondemand', 'progressive')),
        'empty_option' => t('- None -'),
      );

      $options['respondTo'] = array(
        'title' => t('Respond to'),
        'description' => t("Width that responsive object responds to. Can be 'window', 'slider' or 'min' (the smaller of the two)."),
        'type' => 'select',
        'cast' => 'string',
        'default' => 'window',
        'options' => array_combine(
          array('window', 'slider', 'min'),
          array('window', 'slider', 'min')
        ),
      );

      $options['rtl'] = array(
        'title' => t('RTL'),
        'description' => t("Change the slider's direction to become right-to-left."),
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
        'description' => t('Slide/Fade animation speed in milliseconds.'),
        'type' => 'textfield',
        'cast' => 'int',
        'default' => 500,
        'field_suffix' => 'ms',
      );

      $options['swipe'] = array(
        'title' => t('Swipe'),
        'description' => t('Enable swiping.'),
        'type' => 'checkbox',
        'cast' => 'bool',
        'default' => TRUE,
      );

      $options['swipeToSlide'] = array(
        'title' => t('Swipe to slide'),
        'description' => t('Allow users to drag or swipe directly to a slide irrespective of slidesToScroll.'),
        'type' => 'checkbox',
        'cast' => 'bool',
        'default' => FALSE,
        'states' => array('visible' => array(':input[name*="options[settings][swipe]"]' => array('checked' => TRUE))),
      );

      $options['touchMove'] = array(
        'title' => t('Touch move'),
        'description' => t('Enable slide motion with touch.'),
        'type' => 'checkbox',
        'cast' => 'bool',
        'default' => TRUE,
      );

      $options['touchThreshold'] = array(
        'title' => t('Touch threshold'),
        'description' => t('Swipe distance threshold.'),
        'type' => 'textfield',
        'cast' => 'int',
        'default' => 5,
        'states' => array('visible' => array(':input[name*="options[settings][touchMove]"]' => array('checked' => TRUE))),
      );

      $options['useCSS'] = array(
        'title' => t('Use CSS'),
        'description' => t('Enable/Disable CSS Transitions.'),
        'type' => 'checkbox',
        'cast' => 'bool',
        'default' => TRUE,
      );

      $options['cssEase'] = array(
        'title' => t('CSS ease'),
        'description' => t('CSS3 animation easing. <a href="@ceaser">Learn</a> <a href="@bezier">more</a>.', array('@ceaser' => '//matthewlein.com/ceaser/', '@bezier' => '//cubic-bezier.com')),
        'type' => 'textfield',
        'cast' => 'string',
        'default' => 'ease',
        'states' => array('visible' => array(':input[name*="options[settings][useCSS]"]' => array('checked' => TRUE))),
      );

      $options['cssEaseOverride'] = array(
        'title' => t('CSS ease override'),
        'description' => t('If provided, this will override the CSS ease with the pre-defined CSS easings based on <a href="@ceaser">CSS Easing Animation Tool</a>. Leave it empty to use your own CSS ease.', array('@ceaser' => '//matthewlein.com/ceaser/')),
        'type' => 'select',
        'cast' => 'string',
        'default' => '',
        'options' => _slick_css_easing_options(),
        'empty_option' => t('- None -'),
        'states' => array('visible' => array(':input[name*="options[settings][useCSS]"]' => array('checked' => TRUE))),
      );

      $options['easing'] = array(
        'title' => t('jQuery easing'),
        'description' => t('Add easing for jQuery animate as fallback. Use with <a href="@easing">easing</a> libraries or default easing methods. Optionally install <a href="@jqeasing">jqeasing module</a>. This will be ignored and replaced by CSS ease for supporting browsers, or effective if useCSS is disabled.', array('@jqeasing' => '//drupal.org/project/jqeasing', '@easing' => '//gsgd.co.uk/sandbox/jquery/easing/')),
        'type' => 'select',
        'cast' => 'string',
        'default' => 'linear',
        'options' => _slick_easing_options(),
        'empty_option' => t('- None -'),
      );

      $options['variableWidth'] = array(
        'title' => t('variableWidth'),
        'description' => t('Disables automatic slide width calculation.'),
        'type' => 'checkbox',
        'cast' => 'bool',
        'default' => FALSE,
      );

      $options['vertical'] = array(
        'title' => t('Vertical'),
        'description' => t('Vertical slide direction.'),
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
    }
    return $options;
  }
}
