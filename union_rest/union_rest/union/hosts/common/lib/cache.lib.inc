<?php
class Cache
{
    private static $obj = null;

    public static function init() {
        $mem_conf = isset(Conf::$cacheConfig['memcached'])?Conf::$cacheConfig['memcached']:NULL;
        if($mem_conf) {
            self::$obj = new Memcached();
            self::$obj->addServer($mem_conf['host'], $mem_conf['port']);
        }
    }

    public static function set($key, $value, $expire = 300) {
        if(self::$obj === null) {
            self::init();
        }
        if(self::$obj) {
            return self::$obj->set($key, $value, $expire);
        }
        return FALSE;
    }

    public static function get($key) {
        if(self::$obj === null) {
            self::init();
        }
        if(self::$obj) {
            return self::$obj->get($key);
        }
        return FALSE;
    }

    public static function delete($key) {
        if(self::$obj === null) {
            self::init();
        }
        if(self::$obj) {
            return self::$obj->delete($key);
        }
        return FALSE;
    }

    public static function close() {
        if(self::$obj) {
            self::$obj->close();
        }
    }
}
