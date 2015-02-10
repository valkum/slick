<?php

/**
 * @file
 * Definition of Drupal\slick\Plugin\views\style\Slick.
 */

namespace Drupal\slick\Plugin\views\style;

use Drupal\views\Plugin\views\style\StylePluginBase;
use Drupal\Core\Form\FormStateInterface;

/**
 * Style plugin to render the view as Slick slideshow.
 *
 * @ingroup views_style_plugins
 *
 * @ViewsStyle(
 *   id = "slick",
 *   title = @Translation("Slick Slider"),
 *   help = @Translation("Display rows as Slick Slider items"),
 *   theme = "slick_view",
 *   display_types = {"normal"}
 * )
 */
class Slick extends StylePluginBase {

  /**
   * Does the style plugin alows to use style plugins.
   *
   * @var bool
   */
  protected $usesRowPlugin = TRUE;

  /**
   * Does the style plugin support custom css class for the rows.
   *
   * @var bool
   */
  protected $usesRowClass = FALSE;

  /**
   * Set default options.
   */
  protected function defineOptions() {
    $options = parent::defineOptions();

    $options = array(
      'optionset' => array('default' => 'default'),
      'optionset_thumbnail' => array('default' => ''),
      'skin' => array('default' => ''),
      'skin_thumbnail' => array('default' => ''),
      'asnavfor_main' => array('default' => ''),
      'asnavfor_thumbnail' => array('default' => ''),
      'grid' => array('default' => ''),
      'grid_medium' => array('default' => ''),
      'grid_small' => array('default' => ''),
      'visible_slides' => array('default' => ''),
      'slide_field_wrapper' => array('default' => FALSE),
      'slide_title' => array('default' => ''),
      'slide_image' => array('default' => ''),
      'slide_thumbnail' => array('default' => ''),
      'slide_overlay' => array('default' => ''),
      'slide_link' => array('default' => ''),
      'slide_layout' => array('default' => ''),
      'slide_caption' => array('default' => array()),
      'id' => array('default' => ''),
    );

    return $options;
  }

  /**
   * Render the given style.
   */
  public function buildOptionsForm(&$form, FormStateInterface $form_state) {
    parent::buildOptionsForm($form, $form_state);

    module_load_include('inc', 'slick', 'includes/slick.admin');

    $optionsets = slick_optionset_options();
    $skins = slick_skins(TRUE);

    $element['optionset'] = array(
      '#title' => t('Option set main'),
      '#type' => 'select',
      '#options' => $optionsets,
      '#description' => t('Manage optionsets at <a href="@link">Slick carousel admin page</a>.', array('@link' => \Drupal::url('slick.optionset_list'))),
      '#attributes' => array('class' => array('is-tooltip')),
    );

    $element['skin'] = array(
      '#type' => 'select',
      '#title' => t('Skin main'),
      '#options' => $skins,
      '#empty_option' => t('- None -'),
      '#description' => t('Skins allow swappable layouts like next/prev links, split image and caption, etc. However a combination of skins and options may lead to unpredictable layouts, get dirty yourself.'),
      '#attributes' => array('class' => array('is-tooltip')),
    );

    $element['optionset_thumbnail'] = array(
      '#title' => t('Option set thumbnail'),
      '#type' => 'select',
      '#options' => $optionsets,
      '#empty_option' => t('- None -'),
      '#description' => t('If provided, asNavFor aka thumbnail navigation applies. Provide each optionset a unique asNavFor class at <a href="@link" target="_blank">Slick carousel admin page</a>, or use the provided ID where Slick thumbnail navigation ID is suffixed with "-thumbnail". Leave empty to not use thumbnail navigation.', array('@link' => \Drupal::url('slick.optionset_list'))),
      '#attributes' => array('class' => array('is-tooltip')),
    );

    $element['skin_thumbnail'] = array(
      '#type' => 'select',
      '#title' => t('Skin thumbnail'),
      '#options' => $skins,
      '#empty_option' => t('- None -'),
      '#description' => t('Thumbnail navigation skin. Leave empty to not use thumbnail navigation.'),
      '#attributes' => array('class' => array('is-tooltip')),
    );

    $element['asnavfor_main'] = array(
      '#type' => 'textfield',
      '#title' => t('asNavFor main'),
      '#description' => t('Valid CSS selector (with "." or "#") to override asNavFor target for the main display, e.g.: #slick-main-thumbnail or .slick--nav. This should address the thumbnail display class or ID attributes. Slick thumbnail navigation ID is suffixed with "-thumbnail".'),
      '#attributes' => array('class' => array('is-tooltip')),
    );

    $element['asnavfor_thumbnail'] = array(
      '#type' => 'textfield',
      '#title' => t('asNavFor thumbnail'),
      '#description' => t('Valid CSS selector (with "." or "#") to override asNavFor target for the thumbnail display, e.g.: #slick-main or .slick--for. This should address the main display class or ID attributes. Also provide thumbnail field below.'),
      '#attributes' => array('class' => array('is-tooltip')),
    );

    $grid_options = array_combine(range(1, 12), range(1, 12));
    $element['grid'] = array(
      '#type' => 'select',
      '#title' => t('Grid large'),
      '#options' => $grid_options,
      '#empty_option' => t('- None -'),
      '#description' => t('The amount of block grid columns for large monitors 64.063em - 90em. Use skin Grid for starter, and plays with slidesToShow to have more grid combination.'),
      '#attributes' => array('class' => array('is-tooltip')),
    );

    $element['grid_medium'] = array(
      '#type' => 'select',
      '#title' => t('Grid medium'),
      '#options' => $grid_options,
      '#empty_option' => t('- None -'),
      '#description' => t('The amount of block grid columns for medium devices 40.063em - 64em.'),
      '#attributes' => array('class' => array('is-tooltip')),
    );

    $element['grid_small'] = array(
      '#type' => 'select',
      '#title' => t('Grid small'),
      '#options' => $grid_options,
      '#empty_option' => t('- None -'),
      '#description' => t('The amount of block grid columns for small devices 0 - 40em.'),
      '#attributes' => array('class' => array('is-tooltip')),
    );

    $element['visible_slides'] = array(
      '#type' => 'select',
      '#title' => t('Visible slides'),
      '#options' => array_combine(range(1, 32), range(1, 32)),
      '#empty_option' => t('- None -'),
      '#description' => t('How many items per slide displayed at a time related to this grid.'),
      '#attributes' => array('class' => array('is-tooltip')),
    );

    $element['slide_field_wrapper'] = array(
      '#title' => t('Provide Slick field wrapper'),
      '#type' => 'checkbox',
      '#empty_option' => t('- None -'),
      '#description' => t('If checked, Slick will provides Slick markups for the following fields.'),
      '#attributes' => array('class' => array('is-tooltip')),
      '#field_suffix' => '',
      '#title_display' => 'before',
    );

    $image_options = array();
    $caption_options = array();
    $layout_options = array();
    $link_options = array();
    $title_options = array();

    $fields = $this->displayHandler->getHandlers('field');
    foreach ($fields as $field => $handler) {

      // Content: title is not really a field.
      if ($handler->field == 'title') {
        $title_options[$field] = $handler->adminLabel();
      }
      else {
        $type = $handler->options['type'];
        switch ($type) {
          case 'file':
          case 'image':
            $image_options[$field] = $handler->adminLabel();
            break;

          case 'list_text':
            $layout_options[$field] = $handler->adminLabel();
            break;

          case 'text':
          case 'text_long':
          case 'link_field':
            $link_options[$field] = $handler->adminLabel();
            $title_options[$field] = $handler->adminLabel();
            break;
        }
      }

      // Caption can be anything to get custom works going.
      $caption_options[$field] = $handler->adminLabel();
    }
    // Title field.
    $element['slide_title'] = array(
      '#title' => t('Slide title'),
      '#type' => 'select',
      '#empty_option' => t('- None -'),
      '#options' => $title_options,
      '#description' => t('If provided, it will bre wrapped with H2 and class .slide__title.'),
      '#attributes' => array('class' => array('is-tooltip')),
    );

    // Main image field.
    $element['slide_image'] = array(
      '#title' => t('Main image field'),
      '#type' => 'select',
      '#empty_option' => t('- None -'),
      '#options' => $image_options,
      '#description' => t('Main image'),
      '#attributes' => array('class' => array('is-tooltip')),
    );

    // Thumbnail field relevant to asNavFor.
    $element['slide_thumbnail'] = array(
      '#title' => t('Thumbnail image'),
      '#type' => 'select',
      '#empty_option' => t('- None -'),
      '#options' => $image_options,
      '#description' => t("Oly needed if you provide <em>Option set thumbnail</em> to have thumbnail navigation. Make sure the size is smaller than the main image. It may be the same field as the main image, only different instance/ variant dedicated for thumbnails. Leave empty if you don't use thumbnail pager."),
      '#attributes' => array('class' => array('is-tooltip')),
    );

    // Field collection overlay.
    $element['slide_overlay'] = array(
      '#title' => t('Overlay image/video'),
      '#type' => 'select',
      '#empty_option' => t('- None -'),
      '#options' => $image_options,
      '#description' => t('If audio/video, make sure the display is not image display.'),
      '#attributes' => array('class' => array('is-tooltip')),
    );

    // Link field.
    $element['slide_link'] = array(
      '#title' => t('Link'),
      '#type' => 'select',
      '#empty_option' => t('- None -'),
      '#options' => $link_options,
      '#description' => t('Link to content: Read more, View Case Study, etc, wrapped with class .slide__link.'),
      '#attributes' => array('class' => array('is-tooltip')),
    );

    $element['slide_caption'] = array(
      '#type' => 'checkboxes',
      '#title' => t('Caption fields'),
      '#description' => t("Select fields for the caption. This can also be set manually by adding the '.slide__caption' class to a field."),
      '#options' => $caption_options,
      '#attributes' => array('class' => array('is-tooltip')),
    );

    $element['slick'] = array(
      '#type' => 'fieldset',
      '#title' => t('Slick carousel'),
      '#attributes' => array('class' => array('form--slick form--compact form--field has-tooltip')),
      '#description' => t('Leave anything empty/unchecked, except Optionset and Skin, if working with custom markups. <br /><small>Otherwise only relevant markups are added like Field formatter ones, and you have to add supported fields to appear here. Views preview works with jQuery > 1.7.</small>'),
    );

    foreach ($element as $key => $item) {
      $form[$key] = $item;
      if (!in_array($key,
        array(
          'optionset',
          'skin',
          'slide_field_wrapper',
          'id',
          'asnavfor_main',
          'asnavfor_thumbnail',
          'grid',
          'grid_medium',
          'grid_small',
          'visible_slides',
        )
      )) {
        $form[$key]['#states'] = array(
          'visible' => array(
            ':input[name*="[slide_field_wrapper]"]' => array('checked' => TRUE),
          ),
        );
      }
      if (in_array($key, array('asnavfor_main', 'asnavfor_thumbnail'))) {
        $form[$key]['#states'] = array(
          'invisible' => array(
            array('select[name*="[optionset_thumbnail]"]' => array('value' => '')),
            array(':input[name*="[slide_field_wrapper]"]' => array('checked' => FALSE)),
          ),
        );
      }
      if (in_array($key, array('grid_medium', 'grid_small', 'visible_slides'))) {
        $form[$key]['#states'] = array(
          'invisible' => array(
            array('select[name$="[grid]"]' => array('value' => '')),
          ),
        );
      }
    }
  }

}
