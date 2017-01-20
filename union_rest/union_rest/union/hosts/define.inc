<?php
//error_reporting(E_ALL);
//error_reporting(0);
//error_reporting(E_ALL ^ E_NOTICE);
if(!defined('DEBUG')) define('DEBUG', false);

if(!defined('EXP_LOG_LEVEL')) define('EXP_LOG_LEVEL', 7);
if(!defined('EXP_SHOW_ERROR')) define('EXP_SHOW_ERROR', FALSE);
if(!defined('UNION')) define('UNION', TRUE);
if(!defined('ROOT')) define('ROOT',dirname(__FILE__).DIRECTORY_SEPARATOR);
if(!defined('REALROOT')) define("REALROOT",dirname(ROOT).DIRECTORY_SEPARATOR);
if(!defined('LOGROOT')) define("LOGROOT",REALROOT."logs/");
if(!defined('CONFROOT')) define('CONFROOT',REALROOT."conf/");
if(!defined('SETTINGROOT'))define('SETTINGROOT',REALROOT."setting/");
if(!defined('CACHEROOT')) define('CACHEROOT',REALROOT."cache");

include_once(ROOT . "core/loader.php");
loader::init();
