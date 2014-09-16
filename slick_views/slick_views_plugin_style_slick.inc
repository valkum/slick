<?php

/**
 * @file
 * slick style plugin for the Views module.
 */

/**
 * Implements a style type plugin for the Views module.
 */
class SlickViewsPluginStyleSlick extends views_plugin_style {

  /**
   * Set default options.
   */
  function option_definition() {
    $options = parent::option_definition();
    $options += array(
      'optionset' => array('default' => 'default'),
      'optionset_thumbnail' => array('default' => ''),
      'skin' => array('default' => ''),
      'skin_thumbnail' => array('default' => ''),
      'asnavfor_main' => array('default' => ''),
      'asnavfor_thumbnail' => array('default' => ''),
      'slide_field_wrapper' => array('default' => ''),
      'slide_title' => array('default' => ''),
      'slide_image' => array('default' => ''),
      'slide_thumbnail' => array('default' => ''),
      'slide_overlay' => array('default' => ''),
      'slide_link' => array('default' => ''),
      'slide_layout' => array('default' => ''),
      'slide_caption' => array('default' => ''),
      'id' => array('default' => ''),
    );

    return $options;
  }

  /**
   * Show a form to edit the style options.
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $slick_module_path = drupal_get_path('module', 'slick');
    $element = array();
    $settings = $this->options;
    $skins = slick_skins(TRUE);
    $image_styles = image_style_options(FALSE, PASS_THROUGH);

    $optionsets = array();
    foreach (slick_optionset_load_all() as $name => $optionset) {
      $optionsets[$name] = check_plain($optionset->label);
    }

    $element['optionset'] = array(
      '#title' => t('Option set main'),
      '#type' => 'select',
      '#options' => $optionsets,
      '#default_value' => $settings['optionset'],
      '#description' => t('Manage optionsets at <a href="@link">Slick carousel admin page</a>.', array('@link' => url('admin/config/media/slick'))),
      '#attributes' => array('class' => array('is-tooltip')),
    );

    $element['skin'] = array(
      '#type' => 'select',
      '#title' => t('Skin main'),
      '#options' => $skins,
      '#default_value' => $settings['skin'],
      '#description' => t('Skins allow swappable layouts like next/prev links, split image and caption, etc. However a combination of skins and options may lead to unpredictable layouts, get dirty yourself.'),
      '#attributes' => array('class' => array('is-tooltip')),
    );

    $element['optionset_thumbnail'] = array(
      '#title' => t('Option set thumbnail'),
      '#type' => 'select',
      '#options' => $optionsets,
      '#empty_option' => t('- None -'),
      '#default_value' => $settings['optionset_thumbnail'],
      '#description' => t('If provided, asNavFor aka thumbnail navigation applies. Provide each optionset a unique asNavFor class at <a href="@link">Slick carousel admin page</a>, or use the provided ID where Slick thumbnail navigation ID is suffixed with "-thumbnail". Leave empty to not use thumbnail navigation.', array('@link' => url('admin/config/media/slick'))),
      '#attributes' => array('class' => array('is-tooltip')),
    );

    $element['skin_thumbnail'] = array(
      '#type' => 'select',
      '#title' => t('Skin thumbnail'),
      '#options' => $skins,
      '#empty_option' => t('- None -'),
      '#default_value' => $settings['skin_thumbnail'],
      '#description' => t('Thumbnail navigation skin. Skins allow swappable layouts like next/prev links, split image and caption, etc. However a combination of skins and options may lead to unpredictable layouts, get dirty yourself. Leave empty to not use thumbnail navigation.'),
      '#attributes' => array('class' => array('is-tooltip')),
    );

    $element['asnavfor_main'] = array(
      '#type' => 'textfield',
      '#title' => t('asNavFor main'),
      '#default_value' => $settings['asnavfor_main'],
      '#description' => t('Valid CSS selector (with "." or "#") to override asNavFor target for the main display, e.g.: #slick-main-thumbnail or .slick--nav. This should address the thumbnail display class or ID attributes. Slick thumbnail navigation ID is suffixed with "-thumbnail".'),
      '#attributes' => array('class' => array('is-tooltip')),
    );

    $element['asnavfor_thumbnail'] = array(
      '#type' => 'textfield',
      '#title' => t('asNavFor thumbnail'),
      '#default_value' => $settings['asnavfor_thumbnail'],
      '#description' => t('Valid CSS selector (with "." or "#") to override asNavFor target for the thumbnail display, e.g.: #slick-main or .slick--for. This should address the main display class or ID attributes.'),
      '#attributes' => array('class' => array('is-tooltip')),
    );

    $element['slide_field_wrapper'] = array(
      '#title' => t('Provide Slick field wrapper'),
      '#type' => 'checkbox',
      '#default_value' => $settings['slide_field_wrapper'],
      '#empty_option' => t('- None -'),
      '#description' => t('If checked, Slick will provides Slick markups for the following fields.'),
      '#attributes' => array('class' => array('is-tooltip')),
      '#field_suffix' => '',
      '#title_display' => 'before',
    );

    $image_options   = array();
    $caption_options = array();
    $layout_options  = array();
    $link_options    = array();
    $title_options   = array();

    $fields = $this->view->display_handler->get_handlers('field');
    foreach ($fields as $field => $handler) {
      $field_info = $handler->field_info;
      $type = $field_info['type'];

      switch ($type) {
        case 'file':
        case 'image':
          $image_options[$field] = $handler->ui_name();
          break;

        case 'list_text':
          $layout_options[$field] = $handler->ui_name();
          break;

        case 'text':
        case 'text_long':
        case 'link_field':
          $link_options[$field] = $handler->ui_name();
          $title_options[$field] = $handler->ui_name();
          break;
      }
      // Content: title is not really a field.
      if (!$title_options && $handler->field == 'title') {
        $title_options[$field] = $handler->ui_name();
      }

      // Caption can be anything to get custom works going.
      $caption_options[$field] = $handler->ui_name();
    }

    // Simple uniform layout.
    $layout_options = $layout_options ? $layout_options : slick_layouts();
    $element['slide_layout'] = array(
      '#title' => t('Slide layout'),
      '#type' => 'select',
      '#default_value' => $settings['slide_layout'],
      '#options' => $layout_options,
      '#empty_option' => t('- None -'),
      '#description' => t('Make sure to choose "key", not "Default" under Formatter. If you provide a dedicate list text field, it will affect individual slide, otherwise it affects the entire slides within the current slick uniformly. Leave empty to DIY. Or use field formatter to allow more control with a dedicated slide layout field.'),
      '#attributes' => array('class' => array('is-tooltip')),
      '#prefix' => '<h3 class="form--slick__title">' . t('Fields') . '</h3>',
    );

    // Title field.
    $element['slide_title'] = array(
      '#title' => t('Slide title field'),
      '#type' => 'select',
      '#empty_option' => t('- None -'),
      '#options' => $title_options,
      '#default_value' => $settings['slide_title'],
      '#description' => t('If provided, it will bre wrapped with H2 and class .slide__title.'),
      '#attributes' => array('class' => array('is-tooltip')),
    );

    // Main image field.
    $element['slide_image'] = array(
      '#title' => t('Main image field'),
      '#type' => 'select',
      '#empty_option' => t('- None -'),
      '#options' => $image_options,
      '#default_value' => $settings['slide_image'],
      '#description' => t('Main background image field'),
      '#attributes' => array('class' => array('is-tooltip')),
    );

    // Thumbnail field relevant to asNavFor.
    $element['slide_thumbnail'] = array(
      '#title' => t('Thumbnail image field'),
      '#type' => 'select',
      '#empty_option' => t('- None -'),
      '#options' => $image_options,
      '#default_value' => $settings['slide_thumbnail'],
      '#description' => t('Oly needed if you provide <em>Option set thumbnail</em> to have thumbnail navigation. Make sure the size is smaller than the main image. It may be the same field as the main image, only different instance/ variant dedicated for thumbnails.'),
      '#attributes' => array('class' => array('is-tooltip')),
    );

    // Field collection overlay.
    $element['slide_overlay'] = array(
      '#title' => t('Overlay image/video field'),
      '#type' => 'select',
      '#empty_option' => t('- None -'),
      '#options' => $image_options,
      '#default_value' => $settings['slide_overlay'],
      '#description' => t('If audio/video, make sure the display is not image display.'),
      '#attributes' => array('class' => array('is-tooltip')),
    );

    // Link field.
    $element['slide_link'] = array(
      '#title' => t('Link field'),
      '#type' => 'select',
      '#empty_option' => t('- None -'),
      '#options' => $link_options,
      '#default_value' => $settings['slide_link'],
      '#description' => t('Link to content: Read more, View Case Study, etc, wrapped with class .slide__link.'),
      '#attributes' => array('class' => array('is-tooltip')),
    );

    // Caption fields.
    $slide_captions = is_array($settings['slide_caption']) ? array_values($settings['slide_caption']) : $settings['slide_caption'];

    $element['slide_caption'] = array(
      '#type' => 'checkboxes',
      '#title' => t('Caption field'),
      '#description' => t("Select a field to be used as the caption. This can also be set manually by adding the '.slide__caption' class to a field."),
      '#options' => $caption_options,
      '#default_value' => $slide_captions,
      '#attributes' => array('class' => array('is-tooltip')),
    );

    // Add empty suffix to style checkboxes like iOS.
    foreach ($element['slide_caption']['#options'] as $key => $value) {
      $element['slide_caption'][$key]['#field_suffix'] = '';
      $element['slide_caption'][$key]['#title_display'] = 'before';
    }

    $element['id'] = array(
      '#type' => 'textfield',
      '#title' => t('Element ID'),
      '#description' => t("Manually define the Slick carousel container ID attribute <em>Ensure you don't display similar ID elements on the same page</em>. If you provide Option set thumbnail and asNavFor, slick thumbnail navigation ID is suffixed with '-thumbnail'."),
      '#size' => 40,
      '#maxlength' => 255,
      '#default_value' => $settings['id'],
      '#attributes' => array('class' => array('is-tooltip')),
    );

    $form['#attached']['css'][] = $slick_module_path . '/css/admin/slick.admin--ui.css';
    $form['#attached']['css'][] = $slick_module_path . '/css/admin/slick.admin--ui--field.css';
    $form['#attached']['js'][]  = $slick_module_path . '/js/slick.admin.ui.js';

    $form['slick'] = array(
      '#type' => 'fieldset',
      '#title' => t('Slick carousel'),
      '#attributes' => array('class' => array('form--slick form--compact form--field has-tooltip')),
      '#description' => t('Leave anything empty/unchecked, except Optionset and Skin, if you are doing custom markups. Otherwise only relevant markups are added like Field formatter ones, and you have to add supported fields to appear here. Views preview works with jQuery >= 1.7.'),
    );

    foreach ($element as $key => $item) {
      $form['slick'][$key] = $item;
      if (!in_array($key, array(
        'optionset',
        'skin',
        'slide_field_wrapper',
        'id',
        ))) {
        $form['slick'][$key]['#states'] = array(
          'visible' => array(
            ':input[name*="[slide_field_wrapper]"]' => array('checked' => TRUE),
          ),
        );
      }
    }
  }

  /**
   * Performs some cleanup tasks on the options array before saving it.
   */
  function options_submit(&$form, &$form_state) {
    $options = &$form_state['values']['style_options'];

    // In some cases, namely when the style settings are overriden for this
    // display, the slick options aren't in a sub array. No idea why. But this
    // prevents a fatal error.
    if (!empty($options['slick'])) {
      // Pull the fieldset values one level up.
      $options += $options['slick'];
      unset($options['slick']);
    }
  }

  /**
   * Render the display in this style.
   */
  function render() {

    // Group the rows according to the grouping field, if specified.
    $sets = $this->render_grouping(
      $this->view->result,
      $this->options['grouping']
    );

    // Render each group separately and concatenate.
    $output = '';
    foreach ($sets as $title => $rows) {
      // Add main image field if so configured.
      if (!empty($this->options['slide_image'])) {
        $slide_image = $this->options['slide_image'];
        $slide_thumbnail = '';
        if (!empty($this->options['slide_thumbnail'])) {
          $slide_thumbnail = $this->options['slide_thumbnail'];
        }
        foreach ($rows as $index => $row) {
          $rows[$index]->image = $this->rendered_fields[$index][$slide_image];
          if ($slide_thumbnail) {
            $rows[$index]->thumbnail = $this->rendered_fields[$index][$slide_thumbnail];
          }
        }
      }

      // Add caption field if so configured.
      if (!empty($this->options['slide_caption'])) {
        $slide_captions = $this->options['slide_caption'];
        $slide_captions = is_array($slide_captions) ? array_filter($slide_captions) : (array) $slide_captions;

        foreach ($rows as $index => $row) {
          $caption_items = array();
          foreach ($slide_captions as $key => $slide_caption) {
            if (empty($this->rendered_fields[$index][$slide_caption])) {
              continue;
            }
            if (array_key_exists($slide_caption, $this->rendered_fields[$index])) {
              $caption_items[$index][$key]['#markup'] = $this->rendered_fields[$index][$slide_caption];
            }
          }
          if ($caption_items) {
            $rows[$index]->caption = $caption_items;
          }
        }
      }

      // Individual caption fields, if any is provided.
      // Add overlay field if so configured.
      if (!empty($this->options['slide_overlay'])) {
        $slide_overlay = $this->options['slide_overlay'];
        foreach ($rows as $index => $row) {
          $rows[$index]->overlay = $this->rendered_fields[$index][$slide_overlay];
        }
      }

      // Add title field if so configured.
      if (!empty($this->options['slide_title'])) {
        $slide_title = $this->options['slide_title'];
        foreach ($rows as $index => $row) {
          $rows[$index]->title = $this->rendered_fields[$index][$slide_title];
        }
      }

      // Add link field if so configured.
      if (!empty($this->options['slide_link'])) {
        $slide_link = $this->options['slide_link'];
        foreach ($rows as $index => $row) {
          $rows[$index]->link = $this->rendered_fields[$index][$slide_link];
        }
      }

      // Add layout field if so configured.
      if (!empty($this->options['slide_layout'])) {
        $slide_layout = $this->options['slide_layout'];
        // Slide layout may be a list field, or builtin layout options.
        if (strpos('field_', $slide_layout) !== FALSE) {
          foreach ($rows as $index => $row) {
            $rows[$index]->layout = $this->rendered_fields[$index][$slide_layout];
          }
        }
        else {
          foreach ($rows as $index => $row) {
            $rows[$index]->layout = $slide_layout;
          }
        }
      }

      $output .= theme($this->theme_functions(),
        array(
          'view' => $this->view,
          'options' => $this->options,
          'rows' => $rows,
        )
      );
    }

    return $output;
  }
}
