/**
 * @file
 * JS Callbacks provided by the Slick module.
 *
 * Modules and themes may implement any of the Slick methods to interact with
 * the Slick using Drupal.slick.callbacks namespace.
 *
 * Available methods:
 * - onBeforeChange(this, index): Before slide change callback.
 * - onAfterChange(this, index): After slide change callback.
 * - onInit(this): When Slick initializes for the first time callback.
 * - onReInit(this): Every time Slick (re-)initializes callback.
 *
 * @param object slider
 *   A fully qualified current Slick object, already checked before a callback
 *   run, so safe to directly operate on this. If you have several slides on the
 *   same page, the callback applies to all. If you need to operate with a
 *   specific Slick, target its relevant ID instead.
 *
 * @param int index
 *   The index, or position of the current active slide.
 *
 * @see slick.load.js
 * @see slick.media.js
 * @see https://github.com/kenwheeler/slick
 */

/**
 * E.g.: Applies a myMethod() to all Slick onAfterChange, by dragging here.
 */
Drupal.slick.callbacks.onAfterChange = function (slider, index) {
  slider.$slides[index].myMethod();
};

/**
 * E.g.: Applies a css border to all Slick onBeforeChange.
 */
Drupal.slick.callbacks.onBeforeChange = function (slider, index) {
  $('#' + slider.$slider.attr('id')).css({'border': '10px solid red'});
};

/**
 * E.g.: Applies a css border to a specific #my-slick onInit.
 */
Drupal.slick.callbacks.onInit = function (slider) {
  $('#my-slick').css({'border': '10px solid red'});
};

/**
 * E.g.: Applies a css border to all Slick onReInit.
 */
Drupal.slick.callbacks.onReInit = function (slider) {
  $('#' + slider.$slider.attr('id')).css({'border': '10px solid red'});
};
