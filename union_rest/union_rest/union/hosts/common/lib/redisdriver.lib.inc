<?php
class RedisDriver
{
    public static $instances = array();

    public static function link($conf, $relink = False) {
        if(isset(self::$instances[$conf]) && $relink == false){
            return self::$instances[$conf];
        }
        $config = isset(Conf::$redisConfig[$conf])?Conf::$redisConfig[$conf] :NULL;
        if(is_null($config)){
            throw new Exception("Redis Configure failure, Please Check Conf");
        }
        if(!isset(self::$instances[$conf]) || $relink == True) {
            if(isset(self::$instances[$conf]) && $relink == True && $config['conntype'] == 'pconnect') {
                self::$instances[$conf]->close();
            }
            self::$instances[$conf] = self::connect($config);
        }
        return self::$instances[$conf];
    }

    public static function connect($config) {
        $redis = new RedisExt;
        call_user_func(array($redis, $config['conntype']), $config['host'], $config['port']);
        return $redis;
    }
}

class RedisExt extends Redis
{
    const POSITION_FIRST = 0;
    const POSITION_LAST = -1;

    public function zPush($zset, $score, $element) {
        return $this->zAdd($zset, $score, serialize($element));
    }

    // 模拟实现有序队列
    public function zPop($zset) {
        return $this->zsetPop($zset, self::POSITION_FIRST);
    }

    public function zRevPop($zset) {
        return $this->zsetPop($zset, self::POSITION_LAST);
    }

    // 非原子操作！
    private function zsetPop($zset, $position) {
        $this->watch($zset);

        $element = $this->zRange($zset, $position, $position);

        if (!isset($element[0])) {
            return false;
        }

        if ($this->multi()->zRem($zset, $element[0])->exec()) {
            return unserialize($element[0]);
        }

        return $this->zsetPop($zset, $position);
    }
}
