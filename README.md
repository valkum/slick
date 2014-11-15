Drupal module for Slick
=======================

Drupal module for Ken Wheeler's Slick carousel.
See http://kenwheeler.github.io/slick.

* Fully responsive. Scales with its container.
* Uses CSS3 when available. Fully functional when not.
* Swipe enabled. Or disabled, if you prefer.
* Desktop mouse dragging.
* Fully accessible with arrow key navigation.
* Autoplay, pagers, arrows, etc...
* Works with Views, core and contrib fields: Image, Media or Field collection.
* Exportable via CTools.

## Versions
Make sure to run update, when upgrading from 7.x-1.x to 7.x-2.x to allow
creating database table to store option sets.

## Requirements
- Slick library:
  * Download archive from https://github.com/kenwheeler/slick/,
  * Extract it as is, so the needed assets available at:
    sites/../libraries/slick/slick/slick.css
    sites/../libraries/slick/slick/slick.min.js
- CTools, for exportable optionsets -- only the main "Chaos tools" is needed.
  If you have Views installed, CTools is already enabled.
  D8 in core: CMI.
- libraries (>=2.x)
  D8: dropped.
- jquery_update with jQuery > 1.7, perhaps 1.8 if trouble with the latest Slick.
  D8: dropped.
- jqeasing, so available at:
  sites/../libraries/easing/jquery.easing.min.js
  This is a fallback for non-supporting browsers.

## Optional integration
Slick supports enhancements and more complex layouts.
- Colorbox
- Picture, to get truly responsive image using art direction technique.
  D8 in core: Responsive image.
- Media, including media_youtube, media_vimeo, and media_soundcloud.
  D8: Media entity, or isfield.
- Field Collection, to add Overlay image/audio/video over the main image stage,
  with additional basic Scald integration for the image/video/audio overlay.
  D8: ?
- Color field module within Field Collection to colorize the slide individually.
  D8 in core: Color field.
- Mousewheel, download from https://github.com/brandonaaron/jquery-mousewheel,
  so it is available at:
  sites/.../libraries/mousewheel/jquery.mousewheel.min.js

See README.txt on slick_fields.module for more info on slide layouts and fields
integration.

## Optionsets
To create your option sets, go to:
"admin/config/media/slick"
These will be available at Manage display field format, and Views UI.

## Views and Fields
Slick works with Views and as field display formatters.
Slick Views is available as a style plugin included at slick_views.module.
Slick Fields is available as a display formatter included at slick_fields.module
which supports core and contrib fields: Image, Media, Field collection.

See README.txt on slick_views.module for more info on Views integration.

## Programmatically
Use renderable arrays, see slick_fields.module.

## Skins
Skins allow swappable layouts like next/prev links, split image and caption, etc.
Make sure to enable slick_fields.module and provide a dedicated slide layout
per field to get more control over caption placements. However a combination of
skins and options may lead to unpredictable layouts, get dirty yourself.

Some default complex layout skins applied to desktop only, adjust for the mobile
accordingly. The provided skins are very basic to support the necessary layouts.
It is not the module job to match your design requirements.

Optional skins:
--------------
- None
  Doesn't load any extra CSS other than the basic styles required by slick.
  Skins defined by sub-modules fallback to those defined at the optionset.
  Re-save existing Optionset to disable the skin at all.
  If you are using individual slide layout, you may have to do the layouts
  yourself.
- 3d back
  Adds 3d view with focal point at back, works best with 3 slidesToShow,
  centerMode, and caption below the slide.
- Classic
  Adds dark background color over white caption, only good for slider (single
  slide visible), not carousel (multiple slides visible), where small captions
  are placed over images, and animated based on their placement.
- Full screen
  Works best with 1 slidesToShow. Use z-index layering > 8 to position elements
  over the slides, and place it at large regions. Currently only works with
  Slick fields, use Views to make it a block. Use block_reference inside FC to
  have more complex contents inside individual slide, and assign it to Slide
  caption fields.
- Full width
  Adds additional wrapper to wrap overlay audio/video and captions properly.
  This is designated for large slider in the header or spanning width to window
  edges at least 1170px width for large monitor.
- Boxed
  Added a 0 60px margin to slick-list container and hide neighboring slides.
  An alternative to centerPadding which still reveals neighboring slides.
- Split
  Caption and image/media are split half, and placed side by side.
- Box carousel
  Added box-shadow to the carousel slides, multiple visible slides. Use
  slidesToShow option > 2.
- Boxed split
  Caption and image/media are split half, and have edge margin 0 60px.
- Rounded, should be named circle
  This will circle the main image display, reasonable for small carousels, maybe
  with a small caption below to make it nice. Use slidesToShow option > 2.
  Expecting square images.

See slick.slick.inc for more info on skins.

Tips:
----
- Use the Slick API hook_slick_skins_info() to add your own skins.
- Use the provided Wrapper class option at Optionset manager to have a unique
  context as needed, useful to build asNavFor aka thumbnail navigation.
- If having JS error with jQuery v1.7, you may need to upgrade it to v1.8.

## Troubleshooting
When upgrading from Slick v1.3.6 to later version, try to resave options at:
- admin/config/media/slick
- admin/structure/types/manage/CONTENT_TYPE/display
- admin/structure/views/view/VIEW
only if trouble to see the new options, or when options don't apply properly.
This is most likely true when the library adds/changes options, or the module 
does something new.
Always clear the cache when updating the module to ensure things are picked up.

More info relevant to each option is available at their form display by hovering
over them, and click a dark question mark.

## Read more
See the project page on drupal.org: http://drupal.org/project/slick.
See the Slick docs at:
- http://kenwheeler.github.io/slick/
- https://github.com/kenwheeler/slick/
