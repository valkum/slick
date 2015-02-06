<?php

/**
 * @file
 * Contains \Drupal\slick\Form\OptionsetEditForm.
 */

namespace Drupal\slick\Form;

use Drupal\Core\Entity\EntityStorageInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Url;
use Drupal\Component\Utility\String;
use Symfony\Component\DependencyInjection\ContainerInterface;

/**
 * Controller for image style addition forms.
 */
class OptionsetEditForm extends OptionsetFormBase {

  /**
   * Constructs an OptionsetEditForm object.
   *
   * @param \Drupal\Core\Entity\EntityStorageInterface $optionset_storage
   *   The storage.
   */
  public function __construct(EntityStorageInterface $optionset_storage) {
    parent::__construct($optionset_storage);
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
    $form['#title'] = $this->t('Edit optionset %name', array('%name' => $this->entity->label()));
    $form['#tree'] = TRUE;
    return parent::form($form, $form_state);
  }

  /**
   * {@inheritdoc}
   */
  public function save(array $form, FormStateInterface $form_state) {
    parent::save($form, $form_state);
    drupal_set_message($this->t('Changes to the optionset have been saved.'));
  }

  /**
   * {@inheritdoc}
   */
  public function actions(array $form, FormStateInterface $form_state) {
    $actions = parent::actions($form, $form_state);
    $actions['submit']['#value'] = $this->t('Update optionset');

    return $actions;
  }
}
