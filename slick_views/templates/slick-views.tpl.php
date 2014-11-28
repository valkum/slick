<?php

/**
 * @file
 * Default theme implementation for the Slick views template.
 *
 * - $rows: The array of items.
 * - $options: Array of available settings via Views UI.
 */
?>
<div<?php print $attributes; ?>>
  <?php foreach ($rows as $id => $row): ?>
    <?php print render($row); ?>
  <?php endforeach; ?>
</div>
