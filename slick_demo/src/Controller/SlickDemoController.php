<?php

/**
 * @file
 * Contains \Drupal\slick_demo\Controller\SlickDemoController.
 */

namespace Drupal\slick_demo\Controller;

use Drupal\Core\Url;
use Symfony\Component\HttpKernel\Exception\AccessDeniedHttpException;

/**
 * Controller routines for page example routes.
 */
class SlickDemoController {
  /**
   * Constructs a simple page.
   *
   * The router _content callback, maps the path 'examples/page_example/simple'
   * to this method.
   *
   * _content callbacks return a renderable array for the content area of the
   * page. The theme system will later render and surround the content with the
   * appropriate blocks, navigation, and styling.
   */
  public function demo() {
    $render = array(
      '#prefix' => '<div id="slick-demo">',
      '#postfix' => '</div>',
      'single-item' => array(
        '#prefix' => '<h2>Single Item</h2>',
        '#theme' => 'slick',
        '#id' => 'single-item',
        '#items' => array(
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
        ),
      ),

      'multiple-items' => array(
        '#prefix' => '<h2>Multiple Items</h2>',
        '#theme' => 'slick',
        '#id' => 'multiple-items',
        '#items' => array(
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
          array(
            '#markup' => '<h3>7</h3>',
          ),
          array(
            '#markup' => '<h3>8</h3>',
          ),
          array(
            '#markup' => '<h3>9</h3>',
          ),
        ),
        '#settings' => array(
          'infinite' => TRUE,
          'slidesToShow' => 3,
          'slidesToScroll' => 3,
        ),
      ),

      'responsive' => array(
        '#prefix' => '<h2>Responsive Display</h2>',
        '#theme' => 'slick',
        '#id' => 'responsive',
        '#items' => array(
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
        ),
        '#settings' => array(
          'dots' => TRUE,
          'infinite' => FALSE,
          'speed' => 300,
          'slidesToShow' => 4,
          'slidesToScroll' => 4,
          'responsive' => array(
            array(
              'breakpoint' => 1024,
              'settings' => array(
                'slidesToShow' => 3,
                'slidesToScroll' => 3,
                'infinite' => TRUE,
                'dots' => TRUE,
              ),
            ),
            array(
              'breakpoint' => 600,
              'settings' => array(
                'slidesToShow' => 2,
                'slidesToScroll' => 2,
              ),
            ),
            array(
              'breakpoint' => 480,
              'settings' => array(
                'slidesToShow' => 1,
                'slidesToScroll' => 1,
              ),
            ),
          ),
        ),
      ),

      'variable-width' => array(
        '#prefix' => '<h2>Variable Width</h2>',
        '#theme' => 'slick',
        '#id' => 'variable-width',
        '#items' => array(
          array(
            '#markup' => '<div style="width:200px;"><p>200</p></div>',
          ),
          array(
            '#markup' => '<div style="width:175px;"><p>175</p></div>',
          ),
          array(
            '#markup' => '<div style="width:150px;"><p>150</p></div>',
          ),
          array(
            '#markup' => '<div style="width:300px;"><p>300</p></div>',
          ),
          array(
            '#markup' => '<div style="width:225px;"><p>225</p></div>',
          ),
          array(
            '#markup' => '<div style="width:125px;"><p>125</p></div>',
          ),
        ),
        '#settings' => array(
          'dots' => TRUE,
          'infinite' => TRUE,
          'speed' => 300,
          'slidesToShow' => 1,
          'centerMode' => TRUE,
          'variableWidth' => TRUE,
        ),
      ),
      'one-time' => array(
        '#prefix' => '<h2>Adaptive Height</h2>',
        '#theme' => 'slick',
        '#id' => 'one-time',
        '#items' => array(
          array(
            '#markup' => '<div><h3>1</h3></div>',
          ),
          array(
            '#markup' => '<div><h3>2</h3><p>Look ma!</p></div>',
          ),
          array(
            '#markup' => '<div><h3>3</h3><p>Check<br/>this out!</p></div>',
          ),
          array(
            '#markup' => '<div><h3>4</h3><p>Woo!</p></div>',
          ),
        ),
        '#settings' => array(
          'dots' => TRUE,
          'infinite' => TRUE,
          'speed' => 300,
          'slidesToShow' => 1,
          'adaptiveHeight' => TRUE,
        ),
      ),

      'center' => array(
        '#prefix' => '<h2>Center Mode</h2>',
        '#theme' => 'slick',
        '#id' => 'center',
        '#items' => array(
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
        ),
        '#settings' => array(
          'centerMode' => TRUE,
          'centerPadding' => '60px',
          'slidesToShow' => 3,
          'responsive' => array(
            array(
              'breakpoint' => 768,
              'settings' => array(
                'arrows' => FALSE,
                'centerMode' => TRUE,
                'centerPadding' => '40px',
                'slidesToShow' => 3,
              ),
            ),
            array(
              'breakpoint' => 480,
              'settings' => array(
                'arrows' => FALSE,
                'centerMode' => TRUE,
                'centerPadding' => '40px',
                'slidesToShow' => 1,
              ),
            ),
          ),
        ),
      ),

      'lazy loading' => array(),

      'autoplay' => array(
        '#prefix' => '<h2>Autoplay</h2>',
        '#theme' => 'slick',
        '#id' => 'autoplay',
        '#items' => array(
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
        ),
        '#settings' => array(
          'slidesToShow' => 3,
          'slidesToScroll' => 1,
          'autoplay' => TRUE,
          'autoplaySpeed' => 2000,
        ),
      ),

      'fade' => array(
        '#prefix' => '<h2>Fade</h2>',
        '#theme' => 'slick',
        '#id' => 'fade',
        '#items' => array(
          array(
            '#markup' => '<h3><div class="image"><img src="//dummyimage.com/650x400/5081de/ffff00.png"/></div></h3>',
          ),
          array(
            '#markup' => '<h3><div class="image"><img src="//dummyimage.com/650x400/1001de/ff00ff.png"/></div></h3>',
          ),
          array(
            '#markup' => '<h3><div class="image"><img src="//dummyimage.com/650x400/10810e/00ffff.png"/></div></h3>',
          ),
        ),
        '#settings' => array(
          'dots' => TRUE,
          'infinite' => TRUE,
          'speed' => 500,
          'fade' => TRUE,
          'cssEase' => 'linear',
        ),
      ),

      'slider-for' => array(
        '#prefix' => '<h2>Slider Syncing</h2>',
        '#theme' => 'slick',
        '#id' => 'slider-for',
        '#items' => array(
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
        ),
        '#settings' => array(
          'slidesToShow' => 1,
          'slidesToScroll' => 1,
          'arrows' => FALSE,
          'fade' => TRUE,
          'asNavFor' => '#slider-nav',
        ),
      ),
      'slider-nav' => array(
        '#theme' => 'slick',
        '#id' => 'slider-nav',
        '#items' => array(
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
        ),
        '#settings' => array(
          'slidesToShow' => 3,
          'slidesToScroll' => 1,
          'dots' => TRUE,
          'asNavFor' => '#slider-for',
          'centerMode' => TRUE,
          'focusOnSelect' => TRUE,
        ),
      ),
    );

    $render['#attached']['library'][] = 'slick_demo/style';
    return $render;
  }

}
