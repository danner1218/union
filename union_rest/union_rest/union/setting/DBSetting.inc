<?php
class DBSetting
{
    public static $unionDB = array(
        'charset'=>'GBK',
        'options'=>array(
            PDO::ATTR_ERRMODE => PDO::ERRMODE_EXCEPTION,
            PDO::ATTR_DEFAULT_FETCH_MODE => PDO::FETCH_ASSOC
        ),
    );
} 
