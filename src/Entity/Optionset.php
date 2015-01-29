<?php
/**
 * @file
 * Containes \Drupal\slick\Entity\Optionset.
 */

namespace Drupal\slick\Entity;

use \Drupal\Core\Config\Entity\ConfigEntityBase;
use \Drupal\Core\Entity\EntityStorageInterface;
use \Drupal\slick\OptionsetInterface;

/**
 * Defines the slick optionset entity class.
 *
 * @ConfigEntityType(
 *   id = "slick_optionset",
 *   label = @Translation("Optionset"),
 *   module = "slick",
 *   handlers = {
 *     "storage" = "Drupal\slick\OptionsetStorage",
 *     "list_builder" = "Drupal\slick\OptionsetListBuilder",
 *     "form" = {
 *       "edit" = "Drupal\slick\Form\OptionsetEditForm",
 *       "add" = "Drupal\slick\Form\OptionsetEditForm",
 *       "delete" = "Drupal\slick\Form\OptionsetDeleteForm"
 *     }
 *   },
 *   config_prefix = "optionset",
 *   admin_permission = "administer themes",
 *   translateable = FALSE,
 *   entity_keys = {
 *     "id" = "id",
 *     "label" = "label"
 *   },
 *   links = {
 *     "edit-form" = "entity.slick_optionset.edit_form",
 *     "delete-form" = "entity.slick_optionset.delete_form"
 *   }
 * )
 */
class Optionset extends ConfigEntityBase implements OptionsetInterface {
  /**
   * The machinen-readable of this option set.
   *
   * @var string
   */
  public $id;
  /**
   * The human-readable of this option set.
   *
   * @var string
   */
  public $label;
  /**
   * The number of defined breakpoints.
   *
   * @var int
   */
  public $breakpoints;
  /**
   * The The slick skin.
   *
   * @var string
   */
  public $skin;
  /**
   * The UUID of this rule.
   *
   * @var array
   */
  public $options;

}
