<?php
class MongoDriver
{
    public static $instances = array();
    public static $allConnections = array();
    public static $selfObject = false;

    public static function link($conf, $relink = False) {
        if(isset(self::$instances[$conf]) && $relink == false){
            return self::$instances[$conf];
        }
        $staticName = $conf.'MongoDB';
        if(isset(DBConf::$$staticName)){ //
            $staicConfig = DBConf::$$staticName;
            if(empty($staicConfig['username'])){
                $config['dsn'] = "mongodb://".$staicConfig['server'];
            }else{
                $config['dsn'] = "mongodb://".$staicConfig['username'].':'.$staicConfig['password'].'@'.$staicConfig['server'].'/'.$staicConfig['database'];
            }
            $config['database'] = $staicConfig['database'];
        }
        if(!isset($config)){
            throw new Exception("Mongo Configure failure");
        }
        if (isset(self::$instances[$conf]) && $relink == True) {
            self::$instances[$conf]->close();
        }
        self::$instances[$conf] = self::connect($config, $conf);
        return self::$instances[$conf];
    }
    public static function connect($config, $linkName = '') {
        if(!self::$selfObject) {                 // instantiation only first time
            self::$selfObject = new MongoDriver();
        }
        $con = new MongoClient($config['dsn']); // Connect to Mongo Server
        if($linkName != '') {
            self::$allConnections[$linkName] = $con;
        }
        $db = $con->selectDB($config['database']); // Connect to Database
        return $db;
    }

    public function __destruct() {
        if(!empty(self::$instances) && !empty(self::$allConnections)) {
            foreach(self::$allConnections as $linkName => $connection) {
                $connection->close();
                if(isset(self::$instances[$linkName])) self::$instances[$linkName] = null;
            }
        }
    }
}
