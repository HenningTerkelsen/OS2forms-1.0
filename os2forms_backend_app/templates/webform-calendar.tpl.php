<?php

/* 
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

 $idKey = str_replace('_', '-', $component['form_key']); 
?>
<input type="text" id="edit-submitted-<?php print $idKey ?>" class="form-control form-text  <?php print implode(' ', $calendar_classes); ?>" alt="<?php print t('Open popup calendar'); ?>" title="<?php print t('Open popup calendar'); ?>" size="60"/>