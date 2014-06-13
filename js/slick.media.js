/**
 * @file
 */
(function ($) {
  "use strict";

  // Use document as media can be anywhere, inside colorbox, or unreliable
  // different selectors.
  var $document = $(document);

  Drupal.behaviors.slickMedia = {
    attach: function (context, settings) {

      var $slider = $('.slick', context),
        player = '.media--switch--player';

      $(player, context).once('slick-media', function () {
        var t = $(this);

        // Remove SRC attributes to avoid direct autoplay, if enabled.
        t.find('iframe').attr('src', '');

        $document.on('click.media-play', '.media-icon--play', function (e) {
          e.preventDefault();
          var t = $(this),
            iframe = t.closest(player).find('iframe'),
            url = iframe.data('lazy'),
            media = iframe.data('media');

            // Soundcloud needs internet, so fails on disconnected local.
            if (url === '') {
              return false;
            }
            // Provide autoplay, if not enabled.
            if (media.scheme === 'soundcloud') {
              if (url.indexOf('auto_play') < 0 || url.indexOf('auto_play') === false) {
                url = url + '&amp;auto_play=true';
              }
            }
            else if (url.indexOf('autoplay') < 0 || url.indexOf('autoplay') === 0) {
              url = url + '&amp;autoplay=1';
            }

          // Reset other videos to avoid multiple videos playing.
          $(player)
            .removeClass('is-playing')
            .find('iframe').attr('src', '');

          $slider.removeClass('is-paused');
          t.closest('.slick').addClass('is-paused');

          t.closest(player)
            .addClass('is-playing')
            .find('iframe').attr('src', url);

          t.closest('.slick').slickPause();
        })

        .on('click.media-close', '.media-icon--close', function (e) {
          e.preventDefault();
          var t = $(this);
          t.closest(player).removeClass('is-playing').find('iframe').attr('src', '');
          $('.slick').removeClass('is-paused');
        })

        .on('click.media-close-other', '.slick__arrow button, .slick > button, .slick-dots button', function (e) {
          e.preventDefault();
          var t = $(this);
          t.closest('.slick').find(player).removeClass('is-playing');

          if (t.closest('.slick').hasClass('is-paused')) {
            $(player, context).removeClass('is-playing').find('iframe').attr('src', '');
          }
          $('.slick').removeClass('is-paused');
        });

      });
    }
  };

})(jQuery);