<?php
/**
 * Created by PhpStorm.
 * User: Administrator
 * Date: 2016/12/27
 * Time: 11:28
 */


class BaseController extends RestController
{
    public $ipAddress;
    public $startTime;

    public function __construct($request){
        parent::__construct($request);
        $this->startTime = microtime(true);
    }
    public function get_cost(){
        return (microtime(true) - $this->startTime);
    }

    public function checkAuth()
    {
        $this->request['server'] = $_SERVER;
        if(!$this->filterInnerAddress($this->getIpAddress())) {
            throw new Exception('Unauthorized', 401);
        }
        return True;
    }


    /**
     * Fetch the IP Address.
     *
     * @return string|null
     */
    private function getIpAddress()
    {
        if ($this->ipAddress !== NULL) return $this->ipAddress;
        // Server keys that could contain the client IP address
        $keys = array('HTTP_X_FORWARDED_FOR', 'HTTP_CLIENT_IP', 'REMOTE_ADDR');
        foreach ($keys as $key) {
            if (isset($this->request['server'][$key])) {
                $this->ipAddress = $this->request['server'][$key];
                break;
            }
        }

        if ($comma = strrpos($this->ipAddress, ',') !== FALSE) {
            $this->ipAddress = substr($this->ipAddress, $comma + 1);
        }

        return $this->ipAddress;
    }

    /**
     * check address is inner ip,only think about IPV4
     * @param $address
     * @return bool
     */
    private function filterInnerAddress($address)
    {
        $validateIp = (bool) filter_var($address, FILTER_VALIDATE_IP);
        if($validateIp) {
            return !(bool) filter_var($address, FILTER_VALIDATE_IP, FILTER_FLAG_NO_PRIV_RANGE);
        }
        return false;
    }

}
