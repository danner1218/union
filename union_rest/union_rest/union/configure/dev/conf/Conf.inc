<?php
defined('UNION') OR die('Acess deny!');
class Conf
{
      //--开发环境
    public static $sendMsgApi = 'http://127.0.0.1/v2/smsQueueApi.php';
    public static $RedisConfig = array(
        'union' => array(
            'host' => '127.0.0.1',
            'port' => 6379,
            'conntype' => 'connect',
        )
    );
    public static $cacheConfig = array(
        'memcached' => array(
            'host' => '127.0.0.1',
            'port' => 11211
        )
    );
}
