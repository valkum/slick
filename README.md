Drupal module for Slick
==========
Drupal module for Ken Wheeler's Slick carousel. See http://kenwheeler.github.io/slick.

* Fully responsive. Scales with its container.
* Uses CSS3 when available. Fully functional when not.
* Swipe enabled. Or disabled, if you prefer.
* Desktop mouse dragging
* Fully accessible with arrow key navigation
* Autoplay, pagers, arrows, etc...
* Works with Views. (Needs Port)

## Views

Slick works with Views and is available as a style plugin. Select slick as the format. Adjust the settings as needed.

## Programmatically

    <?php
    $render_array =  array(
      '#prefix' => '<h2>Single Item</h2>',
      '#theme' => 'slick',
      '#id' => 'single-item',
      array(
        '#markup' => '<h3>1</h3>',
      ),
      array(
        '#markup' => '<h3>2</h3>',
      ),
      array(
        '#markup' => '<h3>3</h3>',
      ),
      array(
        '#markup' => '<h3>4</h3>',
      ),
      array(
        '#markup' => '<h3>5</h3>',
      ),
      array(
        '#markup' => '<h3>6</h3>',
      ),
      '#settings' => array(),
    );
    Renderer::doRender($render_array);
    ?>

## Read more

See the project page on drupal.org: http://drupal.org/project/slick.
