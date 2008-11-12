<?php
/* sendToHost
 * ~~~~~~~~~~
 * Params:
 *   $host      - Just the hostname.  
 *                No http:// or /path/to/file.html portions
 *   $method    - get or post, case-insensitive
 *   $path      - The /path/to/file.html part
 *   $data      - The query string, without initial question mark
 *   $useragent - If true, 'MSIE' will be sent as
 *                the User-Agent (optional)
 *
 * Examples:
 *   sendToHost('www.google.com','get','/search','q=php_imlib');
 *   sendToHost('www.example.com','post','/some_script.cgi',
 *              'param=First+Param&second=Second+param');
 */
function sendToHost($host,$method,$path,$data,$useragent=0) {
  // Supply a default method of GET if the one passed was empty
  if (empty($method)) {
    $method = 'GET';
  }

  $method = strtoupper($method);
  $fp = fsockopen($host, 80);
  if ($method == 'GET') {
    $path .= '?' . $data;
  }
  fputs($fp, "$method $path HTTP/1.1\n");
  fputs($fp, "Host: $host\n");
  fputs($fp, "Content-type: application/x-www-form-urlencoded\n");
  fputs($fp, "Content-length: " . strlen($data) . "\n");
  if ($useragent) {
    fputs($fp, "User-Agent: MSIE\n");
  }
  fputs($fp, "Connection: close\n\n");
  if ($method == 'POST') {
    fputs($fp, $data);
  }

  while (!feof($fp)) {
    $buf .= fgets($fp,128);
  }
  fclose($fp);
  return $buf;
}
?>