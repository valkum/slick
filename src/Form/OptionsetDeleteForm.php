<?php

/**
 * @file
 * Contains \Drupal\slick\Form\OptionsetDeleteForm.
 */

namespace Drupal\slick\Form;

use Drupal\Core\Entity\EntityConfirmFormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Url;

/**
 * Creates a form to delete an image style.
 */
class OptionsetDeleteForm extends EntityConfirmFormBase {

  /**
   * {@inheritdoc}
   */
  public function getQuestion() {
    return $this->t('Are you sure to delete the optionset %style?', array('%style' => $this->entity->label()));
  }

  /**
   * {@inheritdoc}
   */
  public function getConfirmText() {
    return $this->t('Delete');
  }

  /**
   * {@inheritdoc}
   */
  public function getCancelUrl() {
    return new Url('slick.optionset_list');
  }

  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    $this->entity->delete();
    drupal_set_message($this->t('Optionset %name was deleted.', array('%name' => $this->entity->label())));
    $form_state->setRedirectUrl($this->getCancelUrl());
  }

}
