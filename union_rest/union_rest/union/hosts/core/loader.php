<?php
class loader {

    public static $loader;

    public static function init() {
        if (self::$loader == NULL)
            self::$loader = new self();

        return self::$loader;
    }

    public function __construct() {
        spl_autoload_register(array($this,'lib'));
        spl_autoload_register(array($this,'cdat'));
        spl_autoload_register(array($this,'cmodel'));
        spl_autoload_register(array($this,'core'));
        spl_autoload_register(array($this,'conf'));
        spl_autoload_register(array($this,'setting'));
    }

    public function lib($class) {
        set_include_path(get_include_path().PATH_SEPARATOR.ROOT.'common/lib');
        spl_autoload_extensions('.lib.inc');
        spl_autoload($class);
    }

    public function cdat($class) {
        set_include_path(get_include_path().PATH_SEPARATOR.ROOT.'common/dat');
        spl_autoload_extensions('.dat.inc');
        spl_autoload($class);
    }

    public function cmodel($class) {
        set_include_path(get_include_path().PATH_SEPARATOR.ROOT.'common/dat');
        spl_autoload_extensions('.model.inc');
        spl_autoload($class);
    }

    public function core($class) {
        set_include_path(get_include_path().PATH_SEPARATOR.ROOT.'core/');
        spl_autoload_extensions('.core.inc');
        spl_autoload($class);
    }

    public function conf($class){
        set_include_path(get_include_path().PATH_SEPARATOR.REALROOT.'conf');
        if(strpos($class,'Conf')!==false){
            require_once($class.".inc");
        }
    }

    public function setting($class){
        set_include_path(get_include_path().PATH_SEPARATOR.REALROOT.'setting');
        if(strpos($class,'Setting')!==false){
            require_once($class.".inc");
        }
    }
}
