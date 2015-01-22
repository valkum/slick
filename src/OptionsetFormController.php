<?php
/**
 * @file
 * Contains \Drupal\slick\OptionsetFormController
 */

namespace Drupal\slick;

use Drupal\Core\Entity\EntityInterface;
use Drupal\Core\Entity\EntityFormController;

class OptionsetFormController extends EntityFormController {

  /**
   * {@inheritoc}
   */
  public function form(array $form, array &$form_state) {
    $form['actions']['submit']['#value'] = t('Create new rule');

    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function save(array $form, array &$form_state) {
    $entity = $this->entity;
  }
}
