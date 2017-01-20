<?php
return array(
    'union' => array(
        'dsn' => 'dblib:host='.DBConf::$unionDB['server'].';dbname='.DBConf::$unionDB['database'].';charset='.DBSetting::$unionDB['charset'],
        'host' => DBConf::$unionDB['server'],
        'database' => DBConf::$unionDB['database'],
        'username' => DBConf::$unionDB['username'],
        'password' => DBConf::$unionDB['password'],
        'options' => DBSetting::$unionDB['options'],
    ),
);
