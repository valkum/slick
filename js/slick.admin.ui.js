/**
 * @file
 */
(function($) {
  "use strict";

  // Supports for jQuery < 1.6 admin pages.
  if (typeof $.fn.prop !== 'function') {
    $.fn.prop = function(name, value){
      if (typeof value === 'undefined') {
        return this.attr(name);
      } else {
        return this.attr(name, value);
      }
    };
  }

  Drupal.behaviors.slickAdmin = {
    attach: function(context, settings) {

    $('.form--slick', context).removeClass('no-js');

      if (!$('html').hasClass('js')) {
        $('html').addClass('js');
      }

      $('.fieldset-legend-prefix', context).removeClass('element-invisible');

      $('.is-tooltip', context).each(function () {
        if (!$(this).closest('.form-item').find('.hint').length) {
          $(this).closest('.form-item:not(.form-checkboxes .form-type-checkbox)').append('<span class="hint">?</span>');
        }
      });

      $('.form--slick .form-item > .form-checkbox', context).click(function () {
        var t = $(this);
        if (t.prop('checked')) {
          t.addClass('on');
        } else {
          t.removeClass('on');
        }
      });

      $('.form--slick .form-item > .hint', context).hover(function () {
        $(this).closest('.form-item').addClass('hover');
      },
      function () {
        $(this).closest('.form-item').removeClass('hover');
      });

      $('.form-item > .hint', context).click(function () {
        $('.form--slick .form-item.selected').removeClass('selected');
        $(this).parent().toggleClass('selected');
      });

      $('.form-item > .description', context).click(function () {
        $(this).parent().removeClass('selected');
      });

    }
  };

})(jQuery);