<?php


if(!ini_get('register_globals')) foreach($_REQUEST as $k_123=>$v_123) ${$k_123} = $v_123;

$site = 'http://mantichora.net';
$site_domain = 'mantichora.net';
$site_path = '/customers/mantichora.net/mantichora.net/httpd.www';
$site_company = 'Mantichora.net';
$site_name = 'Mantichora.net';
$site_user = 'Admin';
$site_email = 'admin@mantichora.net';
$site_langs = 'en,sv';
$site_theme = 'mantichora';
$site_time = 0;

$mysql_addr = 'localhost';
$mysql_usr = 'mantichora_net';
$mysql_pwd = 'Lcyna2vP';
$mysql_db = 'mantichora_net';


header('Content-Type: text/plain; charset=utf-8'); 

$db = 0;
function db_open() {
	global $gl_str,$db,$mysql_addr,$mysql_usr,$mysql_pwd,$mysql_db;
	if($db || (($db=mysql_connect($mysql_addr,$mysql_usr,$mysql_pwd)) &&
		mysql_select_db($mysql_db))) return true;
	die($gl_str['db_err_connect']);
}

function &db_query($query) {
	global $gl_str,$db_result;
	$db_result = mysql_query($query) or die($gl_str['db_err_query']."<br>\n<p>".$query."</p>\nError:".mysql_error());
	return $db_result;
}


?>
