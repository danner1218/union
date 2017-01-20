<?php
function __autoload($object) {
    $object = strtolower($object);
    if(file_exists(ROOT."common/dat/{$object}.dat.inc")) {
        require_once(ROOT."common/dat/{$object}.dat.inc");
    } elseif(file_exists(ROOT."common/dat/{$object}.model.inc")) {
        require_once(ROOT."common/dat/{$object}.model.inc");
    } elseif(file_exists(ROOT."common/lib/{$object}.lib.inc")) {
        require_once(ROOT."common/lib/{$object}.lib.inc");
    } elseif(file_exists(ROOT."core/{$object}.core.inc")) {
        require_once(ROOT."core/{$object}.core.inc");
    }
}
