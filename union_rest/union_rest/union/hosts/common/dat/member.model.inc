<?php
class Member
{
    public $union_db;

    public $alliance_id;
    public $alliance_type;
    public $alliance_department;
    public $cust_id;

    public $is_union_member;

    public function __construct($alliance_id) {
        $this->union_db = DB::link('union');
        $this->alliance_id = (int)$alliance_id;
        if($this->setMemberInfo() == False) {
            $this->is_union_member = False;
        }
    }

   

    public function getAllianceInfoByParams($params=array()){
        $sql = "SELECT TOP 1 *
            FROM AllianceMember WHERE cust_id = '{$params['cust_id']}'";
        if(isset($params['AllianceType'])){
            $sql = $sql . "AND AllianceType = '{$params['AllianceType']}'";
        }
        $info = DB::link('union')->fetchAll($sql);
        return $info;
    }

    public function createAllianceInfoByParams($params){
        $info = DB::link('union')->insert('AllianceMember', $params);
        return $info;
    }

    public function updateAllianceInfoByParams($params,$where){
        $info = DB::link('union')->update('AllianceMember',$params , $where);
        return $info;
    }

    public function getMaxAllianceId(){
        try{
            $id = 0;
            $sql = "SELECT MAX(AllianceId) as id FROM AllianceMember";
            $rec = DB::link('union')->fetch($sql);
                if(count($rec) > 0){
                $id = $rec['id'];
            }
        }catch(Exception $e){
            throw $e;
        }
        return $id + 1;
    }

    public function getPayInfoByParams($params){
        $sql = "SELECT TOP 1 * FROM alliance_info_changes 
            WHERE alliance_id = '{$params['alliance_id']}' 
            AND alliance_type = '{$params['alliance_type']}'
            ORDER BY creation_date DESC";
        $info = DB::link('union')->fetch($sql);
        return $info;
    }

    public function getOldPayInfoByAllianceId($alliance_id){
        $sql = "SELECT TOP 1 * FROM AlliancePayeeInfo WHERE AllianceID = {$alliance_id}";
        return  DB::link('union')->fetch($sql);
    }

    public function putPayInfoByParams($params){

        $info = DB::link('union')->insert('alliance_info_changes', $params);
        return $info;
    }


    //19,16位数字，数字必须为半角
    public function verifyBankCard($data){
        $regex1 = '/^\d{19}$/';
        $regex2 = '/^\d{16}$/';
        $flag1 = preg_match($regex1, $data);
        $flag2 = preg_match($regex2, $data);
        if($flag1 || $flag2){
            return true;
        }
        return false;
    }
    //身份证号码
    public function verifyIDCard($data){
        $regex1 = '/^(\d{6})(\d{4})(\d{2})(\d{3})$/';
        $regex = '/^(\d{6})(\d{4})(\d{2})(\d{2})(\d{3})([0-9]|X)$/';
        $flag2 =  preg_match($regex, $data);
        $flag1 = preg_match($regex1, $data);
        if($flag1 || $flag2){
            return true;
        }
        return false;
    }
    //中文匹配
    public function verifyChineseCharachers($data,$num=20){
        $count = Member::utf8Strlen($data);
        if($count > $num){
            return false;
        }
        $regex = '/^[\x{4e00}-\x{9fa5}]+$/u';
        return preg_match($regex, $data);
    }
    //计算中文字符串长度
    public function utf8Strlen($str=null){
         preg_match_all("/./us", $str, $match);
         return count($match[0]);
    }
    //验证手机号
    public function verifyTelephone($data){

        $regex = '/^(13|14|15|17|18)[0-9]{9}$/';
        return  preg_match($regex, $data);

    }
    //验证验证码是否为四位正整数
    public function verifyCode($data){
        $regex = '/^\d{4}$/';
        return  preg_match($regex, $data);
    }
    public static function sendMsg($phone_no,$event_id,$data){
        $time = time();
        $param= "?msg_to={$phone_no}&event_id={$event_id}&app_from=recommend_books&timestamp=".time().
            "&param={$data}&param_format=txt&response_format=json";
        $url = Core::config("identification.send_msg_api").$param;
        $response = Remote::request($url,0,FALSE,'',FALSE,'',Core::config("identification.send_msg_api_timeout"),TRUE);
        $res = json_decode($response,true);
        if(!$res || !isset($res["response_code"])){
            return false;
        }
        return $res["response_code"];
    }


    public static function getAllianceTypeByCustId($cust_id) {
        $sql = "SELECT TOP 1 * FROM AllianceMember WHERE cust_id = {$cust_id}";
        $alliance_type = DB::link('union')->fetch($sql);
        if(!empty($alliance_type)) {
            return $alliance_type;
        }
        return False;
    }
}
