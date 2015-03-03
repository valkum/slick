/**
 * @file
 * Slick loading file.
 */

(function ($, Drupal, drupalSettings) {
  "use strict";

  Drupal.behaviors.slick = {
    attach: function(context) {
      $('.slick', context).once('slick', function() {
        var t = $(this),
            configs = t.data('config') || {};

        t.slick(configs);
      });
    }
  };

})(jQuery, Drupal, drupalSettings);
