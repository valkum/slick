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
        var that = this,
          t = $(that),
          defaults = settings.slick || {},
          configs = t.data('config') || {},
          merged = $.extend({}, defaults, configs),
          slides = '.slick__slide:not(.slick-cloned)',
          index = t.closest(slides).index(),
          total = $(slides, that).length,
          $prevArrow = $('.slick__arrow .slick-prev', that),
          $nextArrow = $('.slick__arrow .slick-next', that),
          callbacks = Drupal.slick.runCallbacks(t, index) || {},
          globals = Drupal.slick.globals(that, merged),
          toShow = parseInt(configs.slidesToShow);

        // @todo drop if any fix with total < configs.slidesToShow.
        // @see https://github.com/kenwheeler/slick/issues/497
        if (t.hasClass('slick--display--thumbnail')) {
          if (total < toShow) {
            // Leave odd, otherwise make it odd to work properly.
            configs.slidesToShow = total %2 != 0 ? total : total - 1;
          }
          else if (total == toShow) {
            configs.slidesToShow = total - 1;
          }
        }

        // Populate defaults + globals into breakpoints.
        if (typeof configs.responsive !== 'undefined') {
          $.map(configs.responsive, function (v, i) {
            if (typeof configs.responsive[i].settings !== 'undefined') {
              configs.responsive[i].settings = $.extend({}, defaults, configs.responsive[i].settings, globals);
            }
          });
        }

        // Build the Slick.
        t.slick($.extend(configs, globals, callbacks));

        // @todo drop when appendArrows works on resize.
        // @see https://github.com/kenwheeler/slick/issues/480
        $(window).bind('resize', function () {
          var a = $('.slick__arrow', that);
          if (a.length && $prevArrow.length) {
            a.append($prevArrow).append($nextArrow);
          }
        });

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
   * Provides custom callbacks.
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
   * Declare global options explicitly to copy into responsives.
   */
  Drupal.slick.globals = function(t, merged) {
    var globals = {
      asNavFor: merged.asNavFor,
      slide: merged.slide,
      lazyLoad: merged.lazyLoad,
      dotsClass: merged.dotsClass,
      rtl: merged.rtl,
      appendArrows: merged.appendArrows,
      prevArrow: $('.slick__arrow .slick-prev', t),
      nextArrow: $('.slick__arrow .slick-next', t),
      customPaging: function (slider, i) {
        return slider.$slides.eq(i).find('.slide__thumbnail').html() || '<button type="button" data-role="none">' + (i + 1) + '</button>';
      },
      onInit: function (slider) {
        Drupal.theme('slickThumbnails', t);
        Drupal.slick.setCurrent(t, slider.currentSlide);
      },
      onAfterChange: function (slider, index) {
        Drupal.slick.setCurrent(t, index);
      }
    };
    return globals;
  };

  /**
   * Without centerMode, .slick-active can be as many as visible slides, hence
   * added a specific class.
   */
  Drupal.slick.setCurrent = function(t, index) {
    $('.slide--current', t).removeClass('slide--current');
    $('.slick__slide[index="' + index + '"]', t).addClass('slide--current');
  };

  /**
   * Theme function to update slick-dots to use thumbnail classes if available.
   */
  Drupal.theme.prototype.slickThumbnails = function (t) {
    // Do not proceed if no thumbnails available.
    if (!$('.slick__slide:first .slide__thumbnail', t).length) {
      return;
    }
    var dotClass = $(t).data('config').dotClass || 'slick-dots';
    $('.' + dotClass, t)
      .addClass('slick__thumbnail')
      .addClass($('.slick__slide:first .slide__thumbnail--hover', t).length ? 'slick__thumbnail--hover' : '');

    $('.slick__slide .slide__thumbnail', t).remove();
  };

})(jQuery);