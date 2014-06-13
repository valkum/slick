/**
 * @file
 */
(function ($) {
  "use strict";

  Drupal.slick = Drupal.slick || {};

  Drupal.behaviors.slick = {
    attach: function (context, settings) {

      $('.slick', context).once('slick', function () {
        var t = $(this),
          thumbnails = t.find('.slick__thumbnail').length,
          configs = t.data('config') || {},
          customs = {
            customPaging: function (slider, i) {
              var $thumbs = t.find('.slick__thumbnail li');
              return $thumbs.eq(i).html() || '<button type="button">' + (i + 1) + '</button>';
            }
          };

        t.slick($.extend({}, configs, customs));

        if (thumbnails) {
          var $dots = t.find('.slick-dots');
          t.find('.slick__thumbnail').remove();
          $dots.addClass('slick__thumbnail');
          if (t.find('.slick__thumbnail button').length) {
            $dots.addClass('slick__thumbnail--hover');
          }
        }

        // Wrap the arrows for easy placement.
        Drupal.slick.wrapArrows(t);

        // Arrow down jumper.
        $('.jump-scroll').click(function (e) {
          e.preventDefault();
          var t = $(this);
          $('html, body').stop().animate({
            scrollTop: $(t.data('target') || t.attr('href')).offset().top - (t.data('offset') || 0)
          }, 800, 'easeInOutExpo');
        });
      });
    }
  };

  // @todo drop by https://github.com/kenwheeler/slick/pull/273
  Drupal.slick.wrapArrows = function (t) {
    if (!$('.slick__arrow .slick-prev', t).length) {
      $('.slick__arrow', t).append(t.find('> button'));
    }
  };

})(jQuery);