<?php
class Core
{
    // Configuration
    public static $configuration;

    /**
     * Get a config item or group. Core::config('main.test')
     *
     * @param   string   item name
     * @param   boolean  
     * @return  mixed
     */
    public static function config($key, $reload = False) {

        // Get the group name from the key
        $sep   = explode('.', $key, 2);
        $conf  = $sep[1];
        $group = $sep[0];

        if(!isset(self::$configuration[$group]) || $reload == True) {
            // Load the configuration group
            self::$configuration[$group] = self::config_load($group);
        }

        return self::$configuration[$group][$conf];
    }

    /**
     * Load Config
     *
     * @return array();
     */
    public static function config_load($name) {

        // Return the config
        return include ROOT . 'load/' . $name . '.inc';
    }

}
