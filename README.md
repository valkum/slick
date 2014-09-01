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
- libraries (>=2.x)
- jquery_update with jQuery >= 1.7
- jqeasing, so available at:
  sites/../libraries/easing/jquery.easing.min.js

## Optional integration
Slick supports enhancements and more complex layouts.
- Colorbox
- Picture, to get truly responsive image using art direction technique.
- Media, including media_youtube, media_vimeo, and media_soundcloud.
- Field Collection, to add Overlay image/audio/video over the main image stage.
- Color field module within Field Collection to colorize the slide individually.
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
which supports core and contrib fields: core Image, Media, Field collection.

See README.txt on slick_views.module for more info on Views integration.

## Programmatically
Use renderable arrays, see slick_fields.module.

## Skins
Skins allow swappable layouts like next/prev links, split image and caption, etc.
Make sure to enable slick_fields.module and provide a dedicated slide layout
per field to get more control over caption placements. However a combination of
skins and options may lead to unpredictable layouts, get dirty yourself.

Some default complex layout skins applied to desktop only, adjust for the mobile
accordingly. The provided skins are very basic to support the layouts, it is
not the module job to match your design requirements.

Tips:
----
- Use the Slick API hook_slick_skins_info() to add your own skins.
- Use the provided Wrapper class option to have a unique context as needed,
  useful to build asNavFor aka thumbnail navigation.

Available skins:
---------------
- Full width
  Adds additional wrapper to wrap overlay audio/video and captions properly.
- Boxed
  Added a 0 60px margin to slick-list container and hide neighboring slides.
  An alternative to centerPadding which still reveals neighboring slides.
- Split
  Caption and image/media are split half, and placed side by side.
- Box carousel
  Added box-shadow to the carousel slides, multiple visible slides. Use
  slideToShow option > 2.
- Boxed split
  Caption and image/media are split half, and have edge margin 0 60px.
- Rounded
  This will round the main image display, reasonable for small carousels, maybe
  with a small caption below to make it nice. Use slideToShow option > 2.

## Troubleshooting
When upgrading from Slick v1.3.6 to v1.3.7, try to resave options at:
- admin/config/media/slick
- admin/structure/types/manage/CONTENT_TYPE/display
only if trouble to see the new options, or when options don't apply properly.
This is most likely true specific to the new appendArrows, the old thumbnail
options, or new thumbnail navigation options. Other existing options don't change.
Always clear the cache when updating the module.

More info relevant to each option is available at their form display by hovering
over them, and click a dark question mark.

## Read more

See the project page on drupal.org: http://drupal.org/project/slick.
See the Slick docs at:
- http://kenwheeler.github.io/slick/
- https://github.com/kenwheeler/slick/
