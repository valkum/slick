<?php

/**
 * @file
 * Default theme implementation for the Slick carousel template.
 *
 * - $items: The array of items.
 * - $settings: A cherry-picked settings to avoid bloated ones.
 * - $thumbnails: The array of thumbnails if available.
 */
?>
<div<?php print $attributes; ?>>

  <?php foreach ($items as $delta => $item): ?>
    <?php print render($item); ?>
  <?php endforeach; ?>

  <?php if (count($items) > 1): ?>
    <div class="slick__arrow">
      <?php if ($settings['has_arrow_down']): ?>
        <?php
          $is_target = $settings['arrow_down_target'] ? ' data-target="#' . $settings['arrow_down_target'] . '"' : '';
          $is_offset = $settings['arrow_down_offset'] ? ' data-offset="' . $settings['arrow_down_offset'] . '"' : '';
        ?>
        <button class="slick-down jump-scroll"<?php print $is_target . $is_offset; ?>></button>
      <?php endif; ?>
    </div>

    <?php if ($thumbnails): ?>
      <ul class="slick__thumbnail">
      <?php foreach ($thumbnails as $key => $thumbnail): ?>
        <li><?php print $thumbnail; ?></li>
      <?php endforeach; ?>
      </ul>
    <?php endif; ?>
  <?php endif; ?>

</div>
