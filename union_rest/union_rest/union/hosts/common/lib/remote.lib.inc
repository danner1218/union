<?php defined('UNION') OR die('Acess deny!');
/**
 * Cookie
 */
class Remote_Core {

	public static function request($url, $limit = 0, $post = '', $cookie = '', $bysocket = FALSE, $ip = '', $timeout = 15, $block = TRUE) {
        if($post){
            if (!filter_var($url, FILTER_VALIDATE_URL, FILTER_FLAG_HOST_REQUIRED))
            {
                return FALSE;
            }
        }
    // Get the hostname and path
		$url = parse_url($url);
        !isset($url['host'])    && $url['host'] = '';
	    !isset($url['path'])    && $url['path'] = '';
	    !isset($url['query'])   && $url['query'] = '';
	    !isset($url['port'])    && $url['port'] = '';
        $host = $url['host'];
	    $path = $url['path'] ? $url['path'].($url['query'] ? '?'.$url['query'] : '') : '/';
	    $port = !empty($url['port']) ? $url['port'] : 80;

		// Open a remote connection
		$fp = fsockopen($host, $port, $errno, $errstr, $timeout);

		if (!is_resource($fp))
			return FALSE;

        stream_set_blocking($fp, $block);
		stream_set_timeout($fp, $timeout);

		// Set CRLF
		$CRLF = "\r\n";

		//request head
        if ($post) {
            $out = 'POST ' . $path . ' HTTP/1.0'.$CRLF;
            $out .= 'Accept: */*'.$CRLF;
            $out .= 'Accept-Language: zh-cn'.$CRLF;
            $out .= 'Content-Type: application/x-www-form-urlencoded'.$CRLF;
            $out .= 'User-Agent: ' . $_SERVER['HTTP_USER_AGENT'] . $CRLF;
            $out .= 'Host: ' . $host . $CRLF;
            $out .= 'Content-Length: ' . strlen($post) . $CRLF;
            $out .= 'Connection: Close' . $CRLF;
            $out .= 'Cache-Control: no-cache' . $CRLF;
            $cookie && $out .= 'Cookie: ' . $cookie . $CRLF;
            // Send one more CRLF to terminate the headers
            $out .= $CRLF . $post;
        } else {
            $out = 'GET ' . $path . ' HTTP/1.0' . $CRLF;
            $out .= 'Accept: */*'.$CRLF;
            $out .= 'Accept-Language: zh-cn'.$CRLF;
            $out .= 'User-Agent: ' . $_SERVER['HTTP_USER_AGENT'] . $CRLF;
            $out .= 'Host: ' . $host . $CRLF;
            $out .= 'Connection: Close' . $CRLF;
            $cookie && $out .= 'Cookie: ' . $cookie . $CRLF;
            // Send one more CRLF to terminate the headers
            $out .= $CRLF;
        }
		fwrite($fp, $out);
        $status = stream_get_meta_data($fp);

        if(!$status['timed_out']) {
            while (!feof($fp)) {
				if(($header = fgets($fp)) && ($header == "\r\n" ||  $header == "\n")) {
					break;
				}
			}

			$stop = false;
            $response = '';
			while(!feof($fp) && !$stop) {
				$data = fread($fp, ($limit == 0 || $limit > 8192 ? 8192 : $limit));
				$response .= $data;
				if($limit) {
					$limit -= strlen($data);
					$stop = $limit <= 0;
				}
			}
        }

		// Close the connection
		fclose($fp);
		return isset($response) ? $response : FALSE;
	}

} // End remote

class Remote extends Remote_Core {}
