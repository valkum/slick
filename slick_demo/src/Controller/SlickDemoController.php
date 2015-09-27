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
      '#prefix' => '<div class="slick--demo">',
      '#postfix' => '</div>',
      'single-item' => array(
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
      ),

      'multiple-#items' => array(
        '#prefix' => '<h2>Multiple Items</h2>',
        '#theme' => 'slick',
        '#id' => 'multiple-#items',
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
        '#options' => array(
          'infinite' => TRUE,
          'slidesToShow' => 3,
          'slidesToScroll' => 3,
        ),
      ),

      'responsive' => array(
        '#prefix' => '<h2>Responsive Display</h2>',
        '#theme' => 'slick',
        '#id' => 'responsive',
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
        '#options' => array(
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
        array(
          '#type' => 'inline_template',
          '#template' => '<div style="width:200px;"><p>200</p></div>',
        ),
        array(
          '#type' => 'inline_template',
          '#template' => '<div style="width:175px;"><p>175</p></div>',
        ),
        array(
          '#type' => 'inline_template',
          '#template' => '<div style="width:150px;"><p>150</p></div>',
        ),
        array(
          '#type' => 'inline_template',
          '#template' => '<div style="width:300px;"><p>300</p></div>',
        ),
        array(
          '#type' => 'inline_template',
          '#template' => '<div style="width:225px;"><p>225</p></div>',
        ),
        array(
          '#type' => 'inline_template',
          '#template' => '<div style="width:125px;"><p>125</p></div>',
        ),
        '#options' => array(
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
        '#options' => array(
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
        '#options' => array(
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

      'lazy loading' => array(
        '#prefix' => '<h2>Lazy Loading</h2>',
        '#theme' => 'slick',
        '#id' => 'lazy',
        array(
          '#type' => 'container',
          '#attributes' => array(
            'class' => array('image'),
          ),
          array(
            '#theme' => 'image',
            '#attributes' => array('data-lazy' => 'http://placeimg.com/200/200/any/grayscale/1.png'),
          ),
        ),
        array(
          '#type' => 'container',
          '#attributes' => array(
            'class' => array('image'),
          ),
          array(
            '#theme' => 'image',
            '#attributes' => array('data-lazy' => 'http://placeimg.com/200/200/any/grayscale/2.png'),
          ),
        ),
        array(
          '#type' => 'container',
          '#attributes' => array(
            'class' => array('image'),
          ),
          array(
            '#theme' => 'image',
            '#attributes' => array('data-lazy' => 'http://placeimg.com/200/200/any/grayscale/3.png'),
          ),
        ),
        array(
          '#type' => 'container',
          '#attributes' => array(
            'class' => array('image'),
          ),
          array(
            '#theme' => 'image',
            '#attributes' => array('data-lazy' => 'http://placeimg.com/200/200/any/grayscale.png'),
          ),
        ),
        array(
          '#type' => 'container',
          '#attributes' => array(
            'class' => array('image'),
          ),
          array(
            '#theme' => 'image',
            '#attributes' => array('data-lazy' => 'http://placeimg.com/200/200/any/grayscale.png'),
          ),
        ),
        '#options' => array(
          'lazyLoad' => 'ondemand',
          'slidesToShow' => 3,
          'slidesToScroll' => 1,
        ),
      ),

      'autoplay' => array(
        '#prefix' => '<h2>Autoplay</h2>',
        '#theme' => 'slick',
        '#id' => 'autoplay',
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
        '#options' => array(
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
        array(
          '#type' => 'container',
          '#attributes' => array(
            'class' => array('image'),
          ),
          array('#markup' => '<h3>'),
          array(
            '#theme' => 'image',
            '#uri' => '//lorempixel.com/650/400/technics/1',
          ),
          array('#markup' => '</h3>'),
        ),
        array(
          '#type' => 'container',
          '#attributes' => array(
            'class' => array('image'),
          ),
          array('#markup' => '<h3>'),
          array(
            '#theme' => 'image',
            '#uri' => '//lorempixel.com/650/400/technics/2',
          ),
          array('#markup' => '</h3>'),
        ),
        array(
          '#type' => 'container',
          '#attributes' => array(
            'class' => array('image'),
          ),
          array('#markup' => '<h3>'),
          array(
            '#theme' => 'image',
            '#uri' => '//lorempixel.com/650/400/technics/3',
          ),
          array('#markup' => '</h3>'),
        ),
        '#options' => array(
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
        '#options' => array(
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
        '#options' => array(
          'slidesToShow' => 3,
          'slidesToScroll' => 1,
          'dots' => TRUE,
          'asNavFor' => '#slider-for',
          'centerMode' => TRUE,
          'focusOnSelect' => TRUE,
        ),
      ),
    );

    $render['#attached']['library'][] = 'slick_demo/slick_demo';
    return $render;
  }

}
