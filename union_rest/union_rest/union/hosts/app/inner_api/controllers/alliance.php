<?php
class Controllers_Alliance extends BaseController {

    public function get() {
        header('Content-Type:text/plain; charset=gb2312');
        $cust_id    = isset($this->request['params']['cust_id'])    ? intval($this->request['params']['cust_id'])   : "";
        $create     = isset($this->request['params']['create'])     ? intval($this->request['params']['create'])    : "";
        $format     = isset($this->request['params']['format'])     ? $this->request['params']['format']            : "json";
        $type       = isset($this->request['params']['type'])       ? intval($this->request['params']['type'])      : 1;
        if(empty($cust_id) || $cust_id == 0 ){
            $this->message('1001','cust_id is empty or type error ');
            return;
        }
        if($create != 1 ){
            $create =  0;
        }
        if($format != 'json'){
            $format = 'json';
        }
        if($type != 1 && $type !=2 && $type !=3){
            $this->message('1008','[type] type error,please input 1 , 2 or 3');
            return;
        }

        $allianceType       = Core::config('core.allianceType');
        $allianceDepartment = Core::config('core.allianceDepartment');
        $type_a             = $allianceType['recommendBooks'];
        $department         = $allianceDepartment['wireless'];

        switch($type){
            case 1:
                $params_a   = array('cust_id'=>$cust_id);
                $info_all   = Member::getAllianceInfoByParams($params_a);
                $count_arr  = count($info_all);

                if($count_arr == 1){
                    $info = $info_all['0'];
                }
                if($count_arr == 0){
                    $info = false;
                }
                if( !$info && $create == 1){
                    //新建一个
                    $alliance_id    = Member::getMaxAllianceId();
                    $params         = array(
                                            'AllianceId'            =>$alliance_id ,
                                            'cust_id'               =>$cust_id,
                                            'loginEmail'            =>'',
                                            'UserStatus'            => 0,
                                            'AllianceType'          => $type_a,
                                            'is_verify'             =>2,
                                            'alliance_department'   => $department
                                        );
                    $c_info         = Member::createAllianceInfoByParams($params);

                    //添加日志
                    if(!$c_info){
                        $str = "create failed ";
                    }else{
                        $str = "create success ";
                    }
                    $msg = "cust_id =>$cust_id  $str : alliace_id =>$alliance_id, 'UserStatus' => 0,
                        'AllianceType' =>$type_a ,'is_verify' =>2, 'alliance_department' => $department";

                    if(!$c_info){
                        $this->message("1003","create new alliance failed ,please try again ");
                        Logger::init(false,KLogger::INFO)->logInfo($msg);
                        return;
                    }

                    Logger::init(false,KLogger::INFO)->logInfo($msg);
                    $params_a       = array('cust_id'=>$cust_id,'AllianceType'=>$type_a);
                    $info           = Member::getAllianceInfoByParams($params_a);
                    $info           = $info['0'];
                }

                //cust_id 对应的账户信息
                if($info){
                    $data           = $this->formatInfo($info);
                    $this->message("200","Success",$data);
                    return;
                }else{
                    $this->message("1004","this cust_id has no basic infomation ");
                    return;
                }
                break;
            case 2:
            case 3:
                $params_a           = array('cust_id'=>$cust_id,'AllianceType'=>$type_a);
                $info               = Member::getAllianceInfoByParams($params_a);
                $info               = $info['0'];
                $alliance_id        = isset($info['AllianceId']) ? $info['AllianceId'] : "" ;
                if(empty($alliance_id)){
                    $this->message('1005','this cust_id invalid');
                    return;
                }

                $params['alliance_id']      = $alliance_id;
                $params['alliance_type']    = $type_a;
                if($type == 2){
                    $info_c                 = Member::getPayInfoByParams($params);
                    if(!$info_c){
                        $this->message('1006','this cust_id has no bank card infomation');
                        return;
                    }
                    $data            = $this->formatAccoutInfo($info_c);
                }else {
                    $info_c                 = Member::getOldPayInfoByAllianceId($alliance_id);
                    if(!$info_c){
                        $this->message('1007','this cust_id has no old  bank card infomation');
                        return;
                    }
                    $data            = $this->formatOldAccoutInfo($info_c);
                }

                $data['cust_id'] = $cust_id;
                $this->message('200','success',$data);
                return;
                break;
            default:
                break;
        }
    }

   /**
     * 更新信息
     */
    public function put(){

        header('Content-Type:text/plain; charset=gb2312');
        $type       = isset($this->request['params']['type'])       ? intval($this->request['params']['type'])      : 1;
        $cust_id    = isset($this->request['params']['cust_id'])    ? intval($this->request['params']['cust_id'])   : "";
        $format     = isset($this->request['params']['format'])     ? $this->request['params']['format']            : "json";

        if($type != 1 && $type !=2 ){
            $this->message('1008','[type] type error or empty');
            return;
        }
        if(empty($cust_id) || $cust_id == 0 ){
            $this->message('1001','cust_id is empty or type error ');
            return;
        }
        if($format != 'json'){
            $format = 'json';
        }
        $a_type = Core::config('core.allianceType');
        $alliance_type = $a_type['recommendBooks'];
        $params_g   = array('cust_id' => $cust_id,'AllianceType' => $alliance_type);
        $info_g     = Member::getAllianceInfoByParams($params_g);
        $info_g     = $info_g['0'];
        if(!$info_g){
            $this->message('1100','cust_id is invalid ');
            return;
        }
        //手机验证通过,才能修改身份信息，身份证验证通过之后才能修改银行卡信息
        if($info_g['is_telephone_verified'] != 2){
            $this->message('1110','please verify the mobile phone');
            return;
        }
        switch ($type){
            case 1: //base info
                $certificate_id = isset($this->request['params']['certificate_id'])     ? $this->request['params']['certificate_id']  : "";
                $contact_name   = isset($this->request['params']['contact_name'])       ? $this->request['params']['contact_name']    : "";
                $params         = array('certificate_id'=>$certificate_id , 'contact_name'=>$contact_name );

                foreach($params as $key => $param){
                    if(empty($param)){
                        $this->message('1002',$key . ' is empty');
                        return;
                    }
                }
                //验证
                $flag_v = Member::verifyIDCard($certificate_id);
                if(!$flag_v){
                    $this->message('1003','certificate_id verify failed');
                    return;
                }
                $flag_n = Member::verifyChineseCharachers($contact_name,10);
                if(!$flag_n){
                    $this->message('1004','contact_name verify failed');
                    return;
                }

                $params_p   = array('CertificateId'=>$certificate_id , 'ContactName'=>$contact_name,'loginEmail'=>$contact_name);
                $where      = "cust_id = $cust_id AND AllianceType = $alliance_type";
                $info_p     = Member::updateAllianceInfoByParams($params_p,$where);
                $temp_msg   = "'CertificateId'=>$certificate_id , 'ContactName'=>$contact_name,'loginEmail'=>$contact_name";

                //添加日志
                if($info_p){
                    $this->message('200','successs');
                    $msg = "cust_id => $cust_id update success : $temp_msg";
                    Logger::init(false,KLogger::INFO)->logInfo($msg);
                    return;
                }else{
                    $this->message('1101','update cust_id : ' .$cust_id . ' failed ');
                    $msg = "cust_id => $cust_id update failed : $temp_msg";
                    Logger::init(false,KLogger::INFO)->logInfo($msg);
                    return;
                }
                break;
            case 2://accout info
                $date = intval(date("d"));
                if($date < 16){
                    $this->message("1230","Can't modify the card information before 16th every month");
                    return;
                }

                $alliance_id      = isset($info_g['AllianceId'])                    ? $info_g['AllianceId'] : "";
                $province         = isset($this->request['params']['province'])     ? $this->request['params']['province'] : "";
                $city             = isset($this->request['params']['city'])         ? $this->request['params']['city'] : "";
                $card_type        = isset($this->request['params']['card_type'])    ? $this->request['params']['card_type'] : "";
                $credit_card_no   = isset($this->request['params']['credit_card_no']) ? $this->request['params']['credit_card_no'] : "";
                $address          = isset($this->request['params']['address'])      ? $this->request['params']['address'] : "";
                $payee_name       = isset($this->request['params']['payee_name'])   ? $this->request['params']['payee_name'] : "";

                //银行卡审核中不能修改银行卡信息
                //获取最新的一条待审核记录
                $params = array('alliance_id'=>$alliance_id,'alliance_type'=>$alliance_type);
                $last_account = Member::getPayInfoByParams($params);
                if(isset($last_account['is_verify']) && $last_account['is_verify'] == 0){
                    $this->message('1231','bank card under review ');
                    return;
                }

                if($info_g['is_idcard_verified'] != 2){
                    $this->message('1232','please verify certificate_id');
                    return;
                }
                //yan zheng
                $params_y         = array(
                                    'alliance_id'   =>$alliance_id ,
                                    'province'      => $province,
                                    'city'          =>$city, 
                                    'card_type'     => $card_type,
                                    'credit_card_no'=>$credit_card_no ,
                                    'address'       => $address, 
                                    'payee_name'    => $payee_name
                                    );
                //验证参数的有效性
                $verify_ret = $this->verifyUpdateParams($params_y);
                if(!empty($verify_ret)){
                    return $this->message($verify_ret[0],$verify_ret[1]);
                }

                $params     = array(
                                    'alliance_id'=>$alliance_id ,
                                    'alliance_type'=>$alliance_type,
                                    'province_name' => iconv('utf-8','gbk',$province),
                                    'city_name' =>iconv('utf-8','gbk',$city), 
                                    'bank_id' => $card_type,
                                    'credit_card' =>$credit_card_no ,
                                    'subbranch' => iconv('utf-8','gbk',$address), 
                                    'account_owner' => iconv('utf-8','gbk',$payee_name),
                                    'site_name' =>'--',
                                    'site_address'=>'--',
                                    'creation_date'=>date('Y-m-d H:i:s'),
                                    'last_modified_date'=>date('Y-m-d H:i:s'),
                                    'is_verify' =>0,
                                );
                $info_p = Member::putPayInfoByParams($params);
                if($info_p){
                    $str    = "inset sucess ";
                    $flag   = 1;
                }else{
                    $str    = "inset failed ";
                }

                $msg = "cust_id =>$cust_id | alliace_id =>$alliance_id  $str  :
                        'province_name' => $province,
                        'city_name' =>$city, 
                        'bank_id' => $card_type,
                        'credit_card' =>$credit_card_no,
                        'subbranch' => $address, 
                        'account_owner' => $payee_name,
                        'is_verify'=>0";

                //添加日志
                if($flag == 1){
                    $this->message('200','successs');
                    Logger::init(false,KLogger::INFO)->logInfo($msg);
                    return;
                }else{
                    $this->message('1111','insert  account info  failed ');
                    Logger::init(false,KLogger::INFO)->logInfo($msg);
                    return;
                }
                break;
            default:
                break;
        }
    }

    public function formatInfo($info){
        $array = array();
        $array['alliance_id']     = empty($info['AllianceId'])      ? '' : intval($info['AllianceId']);
        $array['cust_id']         = empty($info['cust_id'])         ? '' : intval($info['cust_id']);
        $array['contact_name']    = empty($info['loginEmail'])      ? '' : $info['loginEmail'];
        $array['user_status']     = empty($info['UserStatus'])      ? 0  : intval($info['UserStatus']);
        $array['certificate_id']  = empty($info['certificateId'])   ? '' : substr_replace($info['certificateId'],'********',6,8);
        $array['is_verify']       = empty($info['is_verify'])       ? 0  : intval($info['is_verify']);
        $array['telephone']       = empty($info['Telephone'])       ? 0  : substr_replace($info['Telephone'],'****',3,4);
        $array['is_telephone_verified'] = empty($info['is_telephone_verified']) ? 0  : intval($info['is_telephone_verified']);
        $array['is_idcard_verified']    = empty($info['is_idcard_verified'])    ? 0 : intval($info['is_idcard_verified']);
        return $array;
    }

    public function formatAccoutInfo($info){
        $arr = array();
        $arr['alliance_id']     = isset($info['alliance_id'])   ? intval($info['alliance_id']) : "";
        $arr['alliance_type']   = isset($info['alliance_type']) ? intval($info['alliance_type']) : "";
        $arr['payee_name']      = isset($info['account_owner']) ? iconv('gbk','utf-8',$info['account_owner']) : "";
        $arr['address']         = isset($info['subbranch'])     ? iconv('gbk','utf-8',$info['subbranch']) : "";
        $arr['credit_card_no']  = isset($info['credit_card'])   ? iconv('gbk','utf-8',
                                    substr_replace($info['credit_card'],'************',0,12)) : "";
        $arr['province']        = isset($info['province_name']) ? iconv('gbk','utf-8',$info['province_name']) : "";
        $arr['card_type']       = isset($info['bank_id']) ? intval($info['bank_id']) : "";
        $arr['city']            = isset($info['city_name']) ? iconv('gbk','utf-8',$info['city_name']) : "";
        $arr['is_verify']       = isset($info['is_verify']) ? intval($info['is_verify']) : "";
        return $arr;

    }

    public function formatOldAccoutInfo($info){
        $arr = array();
        $arr['alliance_id']     = isset($info['AllianceID'])    ? intval($info['AllianceID']) : "";
        $arr['payee_name']      = isset($info['Payee_name'])    ? iconv('gbk','utf-8',$info['Payee_name']) : "";
        $arr['address']         = isset($info['Address'])       ? iconv('gbk','utf-8',$info['Address']) : "";
        $arr['credit_card_no']  = isset($info['CreditCardNO'])  ? iconv('gbk','utf-8',
                                    substr_replace($info['CreditCardNO'],'************',0,12)) : "";
        $arr['province']        = isset($info['province'])      ? iconv('gbk','utf-8',$info['province']) : "";
        $arr['card_type']       = isset($info['CardType'])      ? intval($info['CardType']) : "";
        $arr['city']            = isset($info['city'])          ? iconv('gbk','utf-8',$info['city']) : "";
        return $arr;

    }

    //更新银行卡信息的验证
    public function verifyUpdateParams($params_y){

        //验证是否为空
        foreach($params_y as $k => $v){
            if(empty($v)){
                return array('1200',$k . ' is empty');
            }
        }
        $credit_card_no = $params_y['credit_card_no'];
        $card_type = $params_y['card_type'];
        //验证中文字符长度及是否是中文
        unset($params_y['alliance_id'],$params_y['card_type'],$params_y['credit_card_no']);
        foreach($params_y as $m => $data){
            $num = 20;
            if($m == "payee_name"){
                $num = 15;
            }
            $flag_y = Member::verifyChineseCharachers($data,$num);
            if(!$flag_y){
                return array('1201' ,$m . ' verify failed ');
            }
        }
        //验证CreditCardNo
        $flag_n = Member::verifyBankCard($credit_card_no);
        if(!$flag_n){
            return array('1202' ,'credit_card_no verify failed ');
        }
        //CardType
        if($card_type < 0 || $card_type > 14 ){
            return array('1203' ,'card_type verify failed ');
        }
        return array();
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
