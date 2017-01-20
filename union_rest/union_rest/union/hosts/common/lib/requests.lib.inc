<?php
class Requests
{
    public static function text($url, $timeout = 5, $connect_timeout = 5) {
        $curl = curl_init($url);
        curl_setopt($curl, CURLOPT_RETURNTRANSFER, true);
        curl_setopt($curl, CURLOPT_CONNECTTIMEOUT, $connect_timeout);
        curl_setopt($curl, CURLOPT_TIMEOUT, $timeout);
        $res  = curl_exec($curl);
        return $res;
    }
}
