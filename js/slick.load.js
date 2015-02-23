/**
 * @file
 * Slick loading file.
 */

(function ($, Drupal, window) {
  "use strict";

  Drupal.behaviors.slick = {
    attach: function(context, settings) {
      $('.slick', context).once('slick', function() {
        var t = $(this),
            configs = t.data('config') || {},
            merged = $.extend({}, settings.slick, configs);

        t.slick(merged);
      });
    }
  };

})(jQuery, Drupal, this);
