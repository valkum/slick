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

    $form['label'] = array(
      '#type' => 'textfield',
      '#title' => $this->t('Optionset name'),
      '#default_value' => $this->entity->label(),
      '#required' => TRUE,
    );
    $form['id'] = array(
      '#type' => 'machine_name',
      '#machine_name' => array(
        'exists' => array($this->optionsetStorage, 'load'),
      ),
      '#default_value' => $this->entity->id(),
      '#required' => TRUE,
    );

    return parent::form($form, $form_state);
  }

  /**
   * {@inheritdoc}
   */
  public function save(array $form, FormStateInterface $form_state) {
    parent::save($form, $form_state);
    $form_state->setRedirectUrl($this->entity->urlInfo('edit-form'));
  }
}
