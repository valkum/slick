/**
 * @file
 */
(function ($) {
  "use strict";

  Drupal.slick = Drupal.slick || {},
    Drupal.slick.callbacks = {};

  Drupal.behaviors.slick = {
    attach: function (context, settings) {

      $('.slick', context).once('slick', function () {
        var t = $(this),
          configs = t.data('config') || {},
          index = t.closest('.slick__slide:not(.slick-cloned)').index(),
          callbacks = Drupal.slick.runCallbacks(t, index) || {},
          customs = {
            customPaging: function (slider, i) {
              return slider.$slides.eq(i).find('.slide__thumbnail').html() || '<button type="button">' + (i + 1) + '</button>';
            },
            prevArrow: $('.slick__arrow--placeholder .slick-prev', t).prop('outerHTML'),
            nextArrow: $('.slick__arrow--placeholder .slick-next', t).prop('outerHTML'),
            onInit: function (slider) {
              // Rebuild arrows and thumbnails based on new options.
              Drupal.theme('slickThumbnails', t);
              Drupal.theme('slickArrows', t);
            }
          };

        // Got no free coffee, customs not inherited by breakpoints.
        // @todo drop if inheritance gets in some day.
        // Latest 1.3.6 is still buggy with responsiveness.
        if (typeof configs.responsive !== 'undefined') {
          $.map(configs.responsive, function (v, i) {
            if (typeof configs.responsive[i].settings !== 'undefined') {
              configs.responsive[i].settings = $.extend(configs.responsive[i].settings, customs);
            }
          });
        }

        // Build the Slick.
        t.slick($.extend(configs, customs, callbacks));

			  // @todo drop when mousewheel does get in.
				// @see https://github.com/kenwheeler/slick/issues/122
				if ($.isFunction($.fn.mousewheel) && configs.mousewheel) {
          t.on('mousewheel', function(e, delta) {
            e.preventDefault();
						var wheelUp = (delta < 0) ? t.slickNext() : t.slickPrev();
          });
        }

        // Arrow down jumper.
        $('.jump-scroll[data-target]').click(function (e) {
          e.preventDefault();
          var t = $(this);
          $('html, body').stop().animate({
            scrollTop: $(t.data('target')).offset().top - (t.data('offset') || 0)
          }, 800, 'easeInOutExpo');
        });
      });
    }
  };

  /**
   * Provide custom callbacks.
   * @see slick.api.js.
   * @see slick.media.js.
   */
  Drupal.slick.runCallbacks = function(slider, index) {
    if (Drupal.slick.callbacks) {
      var methods = $.each(Drupal.slick.callbacks, function(i, callback) {
        var name = {};
        if (typeof callback !== 'undefined' && typeof(callback) === 'function' && slider.$slider) {
         name[callback] = callback.call(this, index);
        }
      });
      return methods;
    }
  };

  /**
   * Theme function to update slick-dots to use thumbnail classes if available.
   */
  Drupal.theme.prototype.slickThumbnails = function (t) {
    // Do not proceed if thumbnails disabled.
    if (!$('.slick__slide:first .slide__thumbnail', t).length) {
      return;
    }
    $('.slick-dots', t)
      .addClass('slick__thumbnail')
      .addClass($('.slick__slide:first .slide__thumbnail--hover', t).length ? 'slick__thumbnail--hover' : '');
  };

  /**
   * Theme function to manipulate arrows and wrap them for easy placement.
   *
   * @todo drop all this trick if any new related option available, e.g.:
   * controlsContainer: '.slick__arrow'.
   */
  Drupal.theme.prototype.slickArrows = function (t) {
    // Do not process if arrows disabled.
    if (!t.find('> .slick-prev').length) {
       return;
    }
    var $down = $('.slick-down', t);

    // Wrap arrows for easy and variant CSS stylings.
    if (!t.find('> .slick__arrow .slick-prev').length) {
      t.find('> .slick-prev, > .slick-next')
        .wrapAll('<nav class="slick__arrow" />');

      // Custom jump down arrow.
      if ($down.length) {
        $('.slick__arrow', t).append($down);
      }

      // Remove placeholder and original arrows since the new one in place.
      t.find('> .slick__arrow--placeholder, > .slick-prev, > .slick-next, > .slick__arrow:empty').remove();
    }
  };

})(jQuery);