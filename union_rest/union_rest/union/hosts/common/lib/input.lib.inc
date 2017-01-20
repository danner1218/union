<?php
/**
 * Input library.
 *
 */
class Input_Core {

    // Enable or disable automatic XSS cleaning
    protected $use_xss_clean = FALSE;

    // Are magic quotes enabled?
    protected $magic_quotes_gpc = FALSE;

    // IP address of current user
    public $ip_address;

    // Input singleton
    protected static $instance;

    /**
     * Retrieve a singleton instance of Input. This will always be the first
     * created instance of this class.
     *
     * @return  object
     */
    public static function instance() {
        if (Input::$instance === NULL) {
            // Create a new instance
            return new Input;
        }

        return Input::$instance;
    }

    /**
     * Sanitizes global GET, POST and COOKIE data. Also takes care of
     * magic_quotes and register_globals, if they have been enabled.
     *
     * @return  void
     */
    public function __construct() {
        // Use XSS clean?
        $this->use_xss_clean = (bool) Core::config('core.global_xss_filtering');

        if (Input::$instance === NULL) {
            // magic_quotes_runtime is enabled
            if (get_magic_quotes_runtime()) {
                set_magic_quotes_runtime(0);
                //exp_log('debug', 'Should disable magic_quotes_runtime');
            }

            // magic_quotes_gpc is enabled
            if (get_magic_quotes_gpc()) {
                $this->magic_quotes_gpc = TRUE;
                //exp_log('debug', 'Should disable magic_quotes_gpc!');
            }

            // register_globals is enabled
            if (ini_get('register_globals')) {
                if (isset($_REQUEST['GLOBALS'])) {
                    // Prevent GLOBALS override attacks
                    exit('Global variable overload attack.');
                }

                // Destroy the REQUEST global
                $_REQUEST = array();

                // These globals are standard and should not be removed
                $preserve = array('GLOBALS', '_REQUEST', '_GET', '_POST', '_FILES', '_COOKIE', '_SERVER', '_ENV', '_SESSION');

                // This loop has the same effect as disabling register_globals
                foreach (array_diff(array_keys($GLOBALS), $preserve) as $key) {
                    global $$key;
                    $$key = NULL;

                    // Unset the global variable
                    unset($GLOBALS[$key], $$key);
                }

                // Warn the developer about register globals
                //exp_log('debug', 'Should Disable register_globals!');
            }

            if (is_array($_GET)) {
                foreach ($_GET as $key => $val) {
                    // Sanitize $_GET
                    $_GET[$this->clean_input_keys($key)] = $this->clean_input_data($val);
                }
            } else {
                $_GET = array();
            }

            if (is_array($_POST)) {
                foreach ($_POST as $key => $val) {
                    // Sanitize $_POST
                    $_POST[$this->clean_input_keys($key)] = $this->clean_input_data($val);
                }
            } else {
                $_POST = array();
            }

            if (is_array($_COOKIE)) {
                foreach ($_COOKIE as $key => $val) {
                    // Ignore special attributes in RFC2109 compliant cookies
                    if ($key == '$Version' || $key == '$Path' || $key == '$Domain')
                        continue;

                    // Sanitize $_COOKIE
                    $_COOKIE[$this->clean_input_keys($key)] = $this->clean_input_data($val);
                }
            } else {
                $_COOKIE = array();
            }
			
			if (is_array($_REQUEST)) {
                foreach ($_REQUEST as $key => $val) {
                    // Sanitize $_POST
                    $_REQUEST[$this->clean_input_keys($key)] = $this->clean_input_data($val);
                }
            } else {
                $_REQUEST = array();
            }

            // Create a singleton
            Input::$instance = $this;

            //exp_log('debug', 'Global GET, POST and COOKIE data sanitized');
        }
    }

    /**
     * Fetch an item from the $_GET array.
     *
     * @param   string   key to find
     * @param   mixed    default value
     * @param   boolean  XSS clean the value
     * @return  mixed
     */
    public function get($key = array(), $default = NULL, $xss_clean = FALSE) {
        return $this->search_array($_GET, $key, $default, $xss_clean);
    }

    /**
     * Fetch an item from the $_POST array.
     *
     * @param   string   key to find
     * @param   mixed    default value
     * @param   boolean  XSS clean the value
     * @return  mixed
     */
    public function post($key = array(), $default = NULL, $xss_clean = FALSE) {
        return $this->search_array($_POST, $key, $default, $xss_clean);
    }

    /**
     * Fetch an item from the $_COOKIE array.
     *
     * @param   string   key to find
     * @param   mixed    default value
     * @param   boolean  XSS clean the value
     * @return  mixed
     */
    public function cookie($key = array(), $default = NULL, $xss_clean = FALSE) {
        return $this->search_array($_COOKIE, $key, $default, $xss_clean);
    }

    /**
     * Fetch an item from the $_SERVER array.
     *
     * @param   string   key to find
     * @param   mixed    default value
     * @param   boolean  XSS clean the value
     * @return  mixed
     */
    public function server($key = array(), $default = NULL, $xss_clean = FALSE) {
        return $this->search_array($_SERVER, $key, $default, $xss_clean);
    }

    /**
     * Fetch an item from a global array.
     *
     * @param   array    array to search
     * @param   string   key to find
     * @param   mixed    default value
     * @param   boolean  XSS clean the value
     * @return  mixed
     */
    protected function search_array($array, $key, $default = NULL, $xss_clean = FALSE) {
        if ($key === array())
            return $array;

        if (!isset($array[$key]))
            return trim($default);

        // Get the value
        $value = $array[$key];

        if ($this->use_xss_clean === FALSE && $xss_clean === TRUE) {
            // XSS clean the value
            $value = $this->xss_clean($value);
        }

        return trim($value);
    }

    /**
     * Fetch the IP Address.
     *
     * @return string
     */
    public function ip_address() {
        if ($this->ip_address !== NULL)
            return $this->ip_address;

        // Server keys that could contain the client IP address
        $keys = array('HTTP_X_FORWARDED_FOR', 'HTTP_CLIENT_IP', 'REMOTE_ADDR');

        foreach ($keys as $key) {
            if ($ip = $this->server($key)) {
                $this->ip_address = $ip;

                // An IP address has been found
                break;
            }
        }

        if ($comma = strrpos($this->ip_address, ',') !== FALSE) {
            $this->ip_address = substr($this->ip_address, $comma + 1);
        }

        if (!valid::ip($this->ip_address)) {
            // Use an empty IP
            $this->ip_address = '0.0.0.0';
        }

        return $this->ip_address;
    }
    protected $_xss_hash = '';
    protected $never_allowed_str = array(
                                        'document.cookie'   =>  '[removed]',
                                        'document.write'    =>  '[removed]',
                                        '.parentNode'       =>  '[removed]',
                                        '.innerHTML'        =>  '[removed]',
                                        'window.location'   =>  '[removed]',
                                        '-moz-binding'      =>  '[removed]',
                                        '<!--'              =>  '&lt;!--',
                                        '-->'               =>  '--&gt;',
                                        '<![CDATA['         =>  '&lt;![CDATA['
    );
    /* never allowed, regex replacement */
    protected $never_allowed_regex = array(
                                    "javascript\s*:"            => '[removed]',
                                    "expression\s*(\(|&\#40;)"  => '[removed]', // CSS and IE
                                    "vbscript\s*:"              => '[removed]', // IE, surprise!
                                    "Redirect\s+302"            => '[removed]'
    );
    protected function remove_invisible_characters($str, $url_encoded = TRUE){
        $non_displayables = array();
        // every control character except newline (dec 10)
        // carriage return (dec 13), and horizontal tab (dec 09)
        if ($url_encoded){
            $non_displayables[] = '/%0[0-8bcef]/';  // url encoded 00-08, 11, 12, 14, 15
            $non_displayables[] = '/%1[0-9a-f]/';   // url encoded 16-31
        }
        $non_displayables[] = '/[\x00-\x08\x0B\x0C\x0E-\x1F\x7F]+/S';   // 00-08, 11, 12, 14-31, 127
        do {
            $str = preg_replace($non_displayables, '', $str, -1, $count);
        }
        while ($count);
        return $str;
    }
    /**
    * Random Hash for protecting URLs
    *
    * @return      string
    */
    protected function xss_hash() {
        if ($this->_xss_hash == ''){
            $this->_xss_hash = md5(time() + mt_rand(0, 1999999999));
        }
        return $this->_xss_hash;
    }
    /**
    * Validate URL entities
    *
    * Called by xss_clean()
    *
    * @param       string  
    * @return      string
    */
    protected function validate_entities($str){
        /*
        * Protect GET variables in URLs
        */
        $str = preg_replace('|\&([a-z\_0-9\-]+)\=([a-z\_0-9\-]+)|i', $this->xss_hash()."\\1=\\2", $str);

        /*
        * Validate standard character entities
        *
        * Add a semicolon if missing.  We do this to enable
        * the conversion of entities to ASCII later.
        *
        */
        $str = preg_replace('#(&\#?[0-9a-z]{2,})([\x00-\x20])*;?#i', "\\1;\\2", $str);

        /*
        * Validate UTF16 two byte encoding (x00)
        *
        * Just as above, adds a semicolon if missing.
        *
        */
        $str = preg_replace('#(&\#x?)([0-9A-F]+);?#i',"\\1\\2;",$str);

        /*
        * Un-Protect GET variables in URLs
        */
        $str = str_replace($this->xss_hash(), '&', $str);
       
        return $str;
    }
    /**
     * Attribute Conversion
     *
     * Used as a callback for XSS Clean
     *
     * @param   array
     * @return  string
     */
    protected function convert_attribute($match){
        return str_replace(array('>', '<', '\\'), array('&gt;', '&lt;', '\\\\'), $match[0]);
    }

    /**
     * HTML Entity Decode Callback
     *
     * Used as a callback for XSS Clean
     *
     * @param   array
     * @return  string
     */
    protected function decode_entity($match){
        return $this->entity_decode($match[0], strtoupper('GBK'));
    }

    /**
     * HTML Entities Decode
     *
     * This function is a replacement for html_entity_decode()
     *
     * The reason we are not using html_entity_decode() by itself is because
     * while it is not technically correct to leave out the semicolon
     * at the end of an entity most browsers will still interpret the entity
     * correctly.  html_entity_decode() does not convert entities without
     * semicolons, so we are left with our own little solution here. Bummer.
     *
     * @param   string
     * @param   string
     * @return  string
     */
    protected function entity_decode($str, $charset='UTF-8'){
        if (stristr($str, '&') === FALSE) {
            return $str;
        }

        $str = html_entity_decode($str, ENT_COMPAT, $charset);
        $str = preg_replace('~&#x(0*[0-9a-f]{2,5})~ei', 'chr(hexdec("\\1"))', $str);
        return preg_replace('~&#([0-9]{2,4})~e', 'chr(\\1)', $str);
    }

    /**
         * Do Never Allowed
         *
         * A utility function for xss_clean()
         *
         * @param       string
         * @return      string
         */
    protected function do_never_allowed($str){
        foreach ($this->never_allowed_str as $key => $val){
            $str = str_replace($key, $val, $str);
        }

        foreach ($this->never_allowed_regex as $key => $val){
            $str = preg_replace("#".$key."#i", $val, $str);
        }
        return $str;
    }

    /**
     * Compact Exploded Words
     *
     * Callback function for xss_clean() to remove whitespace from
     * things like j a v a s c r i p t
     *
     * @param       type
     * @return      type
     */
    protected function compact_exploded_words($matches){
        return preg_replace('/\s+/s', '', $matches[1]).$matches[2];
    }

    /**
     * JS Link Removal
     *
     * Callback function for xss_clean() to sanitize links
     * This limits the PCRE backtracks, making it more performance friendly
     * and prevents PREG_BACKTRACK_LIMIT_ERROR from being triggered in
     * PHP 5.2+ on link-heavy strings
     *
     * @param       array
     * @return      string
     */
    protected function js_link_removal($match){
        $attributes = $this->filter_attributes(str_replace(array('<', '>'), '', $match[1]));
        return str_replace($match[1], preg_replace("#href=.*?(alert\(|alert&\#40;|javascript\:|livescript\:|mocha\:|charset\=|window\.|document\.|\.cookie|<script|<xss|base64\s*,)#si", "", $attributes), $match[0]);
    }

    /**
     * JS Image Removal
     *
     * Callback function for xss_clean() to sanitize image tags
     * This limits the PCRE backtracks, making it more performance friendly
     * and prevents PREG_BACKTRACK_LIMIT_ERROR from being triggered in
     * PHP 5.2+ on image tag heavy strings
     *
     * @param       array
     * @return      string
     */
    protected function js_img_removal($match){
        $attributes = $this->filter_attributes(str_replace(array('<', '>'), '', $match[1]));
        return str_replace($match[1], preg_replace("#src=.*?(alert\(|alert&\#40;|javascript\:|livescript\:|mocha\:|charset\=|window\.|document\.|\.cookie|<script|<xss|base64\s*,)#si", "", $attributes), $match[0]);
    }

    /**
     * Filter Attributes
     *
     * Filters tag attributes for consistency and safety
     *
     * @param       string
     * @return      string
     */
    protected function filter_attributes($str){
        $out = '';
        if (preg_match_all('#\s*[a-z\-]+\s*=\s*(\042|\047)([^\\1]*?)\\1#is', $str, $matches)){
            foreach ($matches[0] as $match){
                $out .= preg_replace("#/\*.*?\*/#s", '', $match);
            }
        }
        return $out;
    }

    /*
     * Remove Evil HTML Attributes (like evenhandlers and style)
     *
     * It removes the evil attribute and either:
     *      - Everything up until a space
     *              For example, everything between the pipes:
     *              <a |style=document.write('hello');alert('world');| class=link>
     *      - Everything inside the quotes 
     *              For example, everything between the pipes:
     *              <a |style="document.write('hello'); alert('world');"| class="link">
     *
     * @param string $str The string to check
     * @param boolean $is_image TRUE if this is an image
     * @return string The string with the evil attributes removed
     */
    protected function remove_evil_attributes($str, $is_image){
        // All javascript event handlers (e.g. onload, onclick, onmouseover), style, and xmlns
        $evil_attributes = array('on\w*', 'style', 'xmlns');
        if ($is_image === TRUE){
            /*
             * Adobe Photoshop puts XML metadata into JFIF images, 
             * including namespacing, so we have to allow this for images.
             */
            unset($evil_attributes[array_search('xmlns', $evil_attributes)]);
        }
        do {
                $str = preg_replace(
                "#<(/?[^><]+?)([^A-Za-z\-])(".implode('|', $evil_attributes).")(\s*=\s*)([\"][^>]*?[\"]|[\'][^>]*?[\']|[^>]*?)([\s><])([><]*)#i",
                "<$1$6",
                $str, -1, $count
                );
        } while ($count);
        return $str;
    }

    /**
     * Sanitize Naughty HTML
     *
     * Callback function for xss_clean() to remove naughty HTML elements
     *
     * @param       array
     * @return      string
     */
    protected function sanitize_naughty_html($matches){
        // encode opening brace
        $str = '&lt;'.$matches[1].$matches[2].$matches[3];
        // encode captured opening or closing brace to prevent recursive vectors
        $str .= str_replace(array('>', '<'), array('&gt;', '&lt;'), $matches[4]);
        return $str;
    }

    public  function xss_clean($str) {
        if (is_array($str)) {
            while (list($key) = each($str)){
                $str[$key] = $this->xss_clean($str[$key]);
            }
            return $str;
        }
        /*
         * Remove Invisible Characters
         */
        $str = $this->remove_invisible_characters($str);

        // Validate Entities in URLs
        $str = $this->validate_entities($str);

        /*
         * URL Decode
         *
         * Just in case stuff like this is submitted:
         *
         * <a href="http://%77%77%77%2E%67%6F%6F%67%6C%65%2E%63%6F%6D">Google</a>
         *
         * Note: Use rawurldecode() so it does not remove plus signs
         *
         */
        $str = rawurldecode($str);

        /*
         * Convert character entities to ASCII
         *
         * This permits our tests below to work reliably.
         * We only convert entities that are within tags since
         * these are the ones that will pose security problems.
         *
         */

        $str = preg_replace_callback("/[a-z]+=([\'\"]).*?\\1/si", array($this, 'convert_attribute'), $str);
        
        $str = preg_replace_callback("/<\w+.*?(?=>|<|$)/si", array($this, 'decode_entity'), $str);

        /*
         * Remove Invisible Characters Again!
         */
        $str = $this->remove_invisible_characters($str);

        /*
         * Convert all tabs to spaces
         *
         * This prevents strings like this: ja  vascript
         * NOTE: we deal with spaces between characters later.
         * NOTE: preg_replace was found to be amazingly slow here on 
         * large blocks of data, so we use str_replace.
         */

        if (strpos($str, "\t") !== FALSE){
            $str = str_replace("\t", ' ', $str);
        }
        /*
         * Capture converted string for later comparison
         */
        $converted_string = $str;

        // Remove Strings that are never alloweublicd
        $str = $this->do_never_allowed($str);

        /*
         * Makes PHP tags safe
         *
         * Note: XML tags are inadvertently replaced too:
         *
         * <?xml
         *
         * But it doesn't seem to pose a problem.
         */
        if ($is_image === TRUE){
            // Images have a tendency to have the PHP short opening and 
            // closing tags every so often so we skip those and only 
            // do the long opening tags.
            $str = preg_replace('/<\?(php)/i', "&lt;?\\1", $str);
        } else {
            $str = str_replace(array('<?', '?'.'>'),  array('&lt;?', '?&gt;'), $str);
        }

        /*
         * Compact any exploded words
         *
         * This corrects words like:  j a v a s c r i p t
         * These words are compacted back to their correct state.
         */
        $words = array('javascript', 'expression', 'vbscript', 'script','applet', 'alert', 'document', 'write', 'cookie', 'window');
        foreach ($words as $word){
            $temp = '';
            $wordlen = strlen($word);
            for ($i = 0; $i < $wordlen; $i++){
                    $temp .= substr($word, $i, 1)."\s*";
            }
            // We only want to do this when it is followed by a non-word character
            // That way valid stuff like "dealer to" does not become "dealerto"
            $str = preg_replace_callback('#('.substr($temp, 0, -3).')(\W)#is', array($this, 'compact_exploded_words'), $str);
        }

        /*
         * Remove disallowed Javascript in links or img tags
         * We used to do some version comparisons and use of stripos for PHP5, 
         * but it is dog slow compared to these simplified non-capturing 
         * preg_match(), especially if the pattern exists in the string
         */
        do {
            $original = $str;
            if (preg_match("/<a/i", $str)){
                $str = preg_replace_callback("#<a\s+([^>]*?)(>|$)#si", array($this, 'js_link_removal'), $str);
            }
            if (preg_match("/<img/i", $str)){
                $str = preg_replace_callback("#<img\s+([^>]*?)(\s?/?>|$)#si", array($this, 'js_img_removal'), $str);
            }
            if (preg_match("/script/i", $str) OR preg_match("/xss/i", $str)){
                $str = preg_replace("#<(/*)(script|xss)(.*?)\>#si", '[removed]', $str);
            }
        }
        while($original != $str);
        unset($original);
        // Remove evil attributes such as style, onclick and xmlns
        $str = $this->remove_evil_attributes($str, $is_image);
        /*
         * Sanitize naughty HTML elements
         *
         * If a tag containing any of the words in the list
         * below is found, the tag gets converted to entities.
         *
         * So this: <blink>
         * Becomes: &lt;blink&gt;
         */
        $naughty = 'alert|applet|audio|basefont|base|behavior|bgsound|blink|body|embed|expression|form|frameset|frame|head|html|ilayer|iframe|input|isindex|layer|link|meta|object|plaintext|style|script|textarea|title|video|xml|xss';
        $str = preg_replace_callback('#<(/*\s*)('.$naughty.')([^><]*)([><]*)#is', array($this, 'sanitize_naughty_html'), $str);

        /*
         * Sanitize naughty scripting elements
         *
         * Similar to above, only instead of looking for
         * tags it looks for PHP and JavaScript commands
         * that are disallowed.  Rather than removing the
         * code, it simply converts the parenthesis to entities
         * rendering the code un-executable.
         *
         * For example: eval('some code')
         * Becomes:             eval&#40;'some code'&#41;
         */
        $str = preg_replace('#(alert|cmd|passthru|eval|exec|expression|system|fopen|fsockopen|file|file_get_contents|readfile|unlink)(\s*)\((.*?)\)#si', "\\1\\2&#40;\\3&#41;", $str);

        // Final clean up
        // This adds a bit of extra precaution in case
        // something got through the above filters
        $str = $this->do_never_allowed($str);
        if ($is_image === TRUE){
            return ($str == $converted_string) ? TRUE: FALSE;
        }
        return $str;
    }


    /**
     * This is a helper method. It enforces W3C specifications for allowed
     * key name strings, to prevent malicious exploitation.
     *
     * @param   string  string to clean
     * @return  string
     */
    public function clean_input_keys($str) {
        $chars = PCRE_UNICODE_PROPERTIES ? '\pL' : 'a-zA-Z';

        if (!preg_match('#^['.$chars.'0-9:_.-]++$#uD', $str)) {
            exit('Disallowed key characters in global data.');
        }

        return $str;
    }

    /**
     * This is a helper method. It escapes data and forces all newline
     * characters to "\n".
     *
     * @param   unknown_type  string to clean
     * @return  string
     */
    public function clean_input_data($str) {
        if (is_array($str)) {
            $new_array = array();
            foreach ($str as $key => $val) {
                // Recursion!
                $new_array[$this->clean_input_keys($key)] = $this->clean_input_data($val);
            }
            return $new_array;
        }

        if ($this->magic_quotes_gpc === TRUE) {
            // Remove annoying magic quotes
            $str = stripslashes($str);
        }

        if ($this->use_xss_clean === TRUE) {
            $str = $this->xss_clean($str);
        }

        if (strpos($str, "\r") !== FALSE) {
            // Standardize newlines
            $str = str_replace(array("\r\n", "\r"), "\n", $str);
        }

        return str_replace('\'', '\'\'', $str);
    }

} // End Input Class

class Input extends Input_Core {}
