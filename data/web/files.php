<?php

header('Content-Type: text/plain; charset=utf-8'); 

if(!ini_get('register_globals')) foreach($_REQUEST as $k_123=>$v_123) ${$k_123} = $v_123;

$s = stat('./files.txt');
$t2 = $s['mtime'];
$t3 = time();
if($t>($t3-$t2)) {
	$files = explode("\n",file_get_contents('./files.txt'));
	foreach($files as $f) {
		$f = explode("\t",$f,2);
		$t2 = $f[0]*1;
		$f = $f[1];
		if($t>($t3-$t2)) echo $f."\n";
	}
}


?>