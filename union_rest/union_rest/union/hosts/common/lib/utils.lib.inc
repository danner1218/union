<?php
class Utils
{
    /**
     * 传入start_time和end_time，将其分为*分钟一段，调用回调函数执行
     *
     * @param   datetime   start_date
     * @param   datetime   end_date
     * @param   string     callback function name
     * @param   int        interval 
     * @return  boolean
     */
    public static function timeSlice($callback, $start_date, $end_date, $args = array(), $interval = 600) {
        $start_time = strtotime($start_date);
        $end_time   = strtotime($end_date);
        if(!is_callable($callback) || $start_time == False || $end_time == False) {
            return False;
        }

        $current_time = $start_time;
        while(True) {
            $next_time = $current_time + $interval;
            if($next_time >= $end_time) {
                $next_time = $end_time;
            }
            $current_date = date("Y-m-d H:i:s", $current_time);
            $next_date    = date("Y-m-d H:i:s", $next_time);
            call_user_func($callback, $current_date, $next_date, $args);
            $current_time = $next_time;
            if($current_time == $end_time) {
                break;
            }
        }
    }

    /**
     * 复制mssql数据表(通数据库)
     *
     * @param   string   from [db].[dbo].[table]
     * @param   string   to [db].[dbo].[table]
     * @param   string   type:1 只拷贝表结构 type:2 拷贝表结构和表数据
     * @return  null
     */
    public static function mssqlCopy($table, $from, $to, $type = 1) {
        $ext = "";
        if($type == 1) {
            $ext = " WHERE 1=0 ";
        }
        $sql  = "SELECT * INTO {$to} FROM {$from} {$ext} ";
        DB::link($table)->query($sql);
    }

}
