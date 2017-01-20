<?php
class MssqlDriver
{
    public static $instances = array();

    public static function link($conf, $relink = False) {
        if(is_string($conf)) {
            $db_key = $conf;
            $conf   = Core::config("db.{$conf}");
        } else {
            $db_key = md5((string)$conf);
        }
        if(!isset(self::$instances[$db_key]) || $relink == True) {
            self::$instances[$db_key] = new MssqlDataBase($conf);
        }
        return self::$instances[$db_key];
    }
}
class MssqlDataBase
{
    public $connection = null;

    public function __construct($conf) {
        // Connect to MSSQL and select the database
        $link = mssql_connect($conf['host'], $conf['username'], $conf['password']);
        mssql_select_db($conf['database'], $link);
        $this->connection = $link;
    }

    public function selectPage($sql, $page, $pagesize) {
        $r = mssql_query($sql, $this->connection);
        if($r){
            $total_page = ceil(mssql_num_rows($r)/$pagesize);
            if(!$page || $page<=0) $page=1;
            if($page>$total_page) $page=$total_page;
            $begin = ($page-1) * $pagesize;
            $re_arr=array();
            $index=0;
            if($pagesize>0) mssql_data_seek($r,$begin);
            while($result_arr=@mssql_fetch_array($r, MSSQL_ASSOC)){
                if($pagesize>0) {
                    if($index>$pagesize-1) break;
                }
                $re_arr[$index]=$result_arr;
                $index++;
            }
            return  $re_arr;
        } else {
            return  false;
        }
    }
}
