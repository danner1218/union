<?php
class Controllers_Code extends BaseController {

    public function get() {
        header('Content-Type:text/plain; charset=gb2312');
        $cust_id    = isset($this->request['params']['cust_id'])    ? intval($this->request['params']['cust_id'])   : "";
        $mobile     = isset($this->request['params']['mobile'])     ? intval($this->request['params']['mobile'])   : "";
        if(empty($cust_id) || $cust_id == 0 ){
            $this->message('1001','cust_id is empty or type error ');
            return;
        }
        $ismatch = Member::verifyTelephone($mobile);
        if(!$ismatch){
            $this->message('1002','please input right mobile ');
            return;
        }
        $allianceType       = Core::config('core.allianceType');
        $allianceDepartment = Core::config('core.allianceDepartment');
        $type_a             = $allianceType['recommendBooks'];
        $department         = $allianceDepartment['wireless'];

        $params_a   = array('cust_id'=>$cust_id,'AllianceType'=>$type_a);
        $info_all   = Member::getAllianceInfoByParams($params_a);
        $count_arr  = count($info_all);

        if($count_arr == 1){
            $info = $info_all['0'];
        }
        if($count_arr == 0){
            $this->message('1003','this cust_id is invalid ');
            return;
        }
        $exist_mobile = isset($info['Telephone']) ? intval($info['Telephone']) : "";
        if($exist_mobile == $mobile && $info['is_telephone_verified'] == 2){
            $this->message('1004','this mobile has been bind ');
            return;
        }

        //获取配置
        $day_max = Core::config("identification.cust_id_day_max");
        $code_lifetime =  Core::config("identification.code_lifetime");
        $day_lifetime = Core::config("identification.day_lifetime");

        var_dump($code_lifetime);
        //cust_id超过每天发送短信限制
        $day_time = intval(Cache::get(md5($cust_id)));
        if($day_time + 1 > $day_max){
            $this->message('1005','verification code number more than the maximum value '); 
            return;
        }

        //每两分钟只能获取验证码一次
        $has_code = Cache::get(md5($mobile));
        if(!empty($has_code)){
            $this->message('1006','every two minutes can only send a verification code');
            return;
        }
        //发送并设置验证码
        $code = rand(0,9).rand(0,9).rand(0,9).rand(0,9);
        $res = Member::sendMsg($mobile,Core::config("identification.msg_event_id"),$code);
        if($res == 100 ){
            //设置该day获取验证码次数加1
            if(empty($day_time)){
                Cache::set(md5($cust_id),1,$day_lifetime);
            }else{
                Cache::set(md5($cust_id),$day_time + 1,$day_lifetime);
            }
            Cache::set(md5($mobile),$code,$code_lifetime);
            $this->message('200','success');
            return;
        }else{
            $this->message('1007','send verification code failed');
            return;
        }
    }

   /**
     *验证验证码
     */
    public function put(){

        header('Content-Type:text/plain; charset=gb2312');
        $cust_id    = isset($this->request['params']['cust_id'])    ? intval($this->request['params']['cust_id'])   : "";
        $mobile     = isset($this->request['params']['mobile'])     ? intval($this->request['params']['mobile'])    : "";
        $code       = isset($this->request['params']['code'])       ? intval($this->request['params']['code'])      : "";
        if(empty($cust_id) || $cust_id == 0 ){
            $this->message('1001','cust_id is empty or type error ');
            return;
        }

        $ismatch = Member::verifyTelephone($mobile);
        if(!$ismatch){
            $this->message('1002','please input right mobile ');
            return;
        }


        $verify_code = Member::verifyCode($code);
        if(!$verify_code){
            $this->message('1100','please input the four digits ');
            return;
        }
        $allianceType       = Core::config('core.allianceType');
        $allianceDepartment = Core::config('core.allianceDepartment');
        $type_a             = $allianceType['recommendBooks'];
        $department         = $allianceDepartment['wireless'];

        $params_a   = array('cust_id'=>$cust_id,'AllianceType'=>$type_a);
        $info_all   = Member::getAllianceInfoByParams($params_a);
        $count_arr  = count($info_all);
        if($count_arr == 1){
            $info = $info_all['0'];
        }
        if($count_arr == 0){
            $this->message('1003','this cust_id is invalid ');
            return;
        }
        $exist_mobile = isset($info['Telephone']) ? intval($info['Telephone']) : "";
        if($exist_mobile == $mobile && $info['is_telephone_verified'] == 2){
            $this->message('1004','this mobile has been bind ');
            return;
        }

        //获取配置
        $day_max = Core::config("identification.cust_id_day_max");
        $code_lifetime =  Core::config("identification.code_lifetime");
        $day_lifetime = Core::config("identification.day_lifetime");

        //cust_id超过每天发送短信限制
        $day_time = intval(Cache::get(md5($cust_id)));
        if($day_time > $day_max){
            $this->message('1005','verification code number more than the maximum value '); 
            return;
        }

        //验证码是否还有效
        $has_code = Cache::get(md5($mobile));
        if(empty($has_code)){
            $this->message('1006','aptcha has been invalid');
            return;
        }

        //验证验证码
        if($has_code != $code ){
            $this->message('1007','Captcha verify failed ');
            return;
        }

        //更新
        $params = array("Telephone"=>$mobile,"is_telephone_verified"=>2);
        $where  = "cust_id = $cust_id AND AllianceType = $type_a";
        $info   = Member::updateAllianceInfoByParams($params,$where);

        //添加日志
        if($info){
            $this->message('200','successs');
            $msg = "cust_id => $cust_id update success :$mobile ";
        }else{
            $this->message('1101','update cust_id : ' .$cust_id . ' failed ');
            $msg = "cust_id => $cust_id update failed : $mobile";
        }

        Logger::init(false,KLogger::INFO)->logInfo($msg);
        return;
    }

    public function message($code='200',$message='success',$data=array()){
        $this->response       = array(
            'code'      =>intval($code),
            'message'   =>$message,
            'data'      =>$data,
            'cost'      =>$this->get_cost()
            );
        $this->responseStatus = 200;
    }

}
