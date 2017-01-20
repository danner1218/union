<?php
defined('UNION') OR die('Acess deny!');

class DBConf
{
    //---开发配置
    public static $unionDB = array(
        'database' => '[union]',
        'username' => 'writeuser',
        'password' => 'ddbackend',
        'server' => '127.0.0.1:1433',
        'dbtype' => 'mssql',
    );

    //--开发mongo配置
    public static $unionMongoDB=array(
        'database' => 'union',
        'username' => 'ddunion',
        'password' => '4RLeESoll4',
        'server' => '127.0.0.1:15218',
        'dbtype' => 'mongo',
    );


}
?>
