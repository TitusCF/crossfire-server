<?php
/*
 * database server type values:
 * 0: ODBC      (not supported)
 * 1: postgres  (supported)
 * 2: MySql     (not supported)
 */
$databaseservertype=1;/*Only this one supported for now*/
$databasename="crossfire";
$databaseuser="crossfire";
/*
 * Comment any of the following line to
 * forget the associated argument in database connection
 */
// $databasepass="";
// $databasehost="";
// $databaseport="";


function database_connect ()
{
    global $databasehandle;
    global $databasename;
    global $databaseuser;
    global $databasepass;
    global $databasehost;
    global $databaseport;
    global $databaseservertype;
    if ($databasehandle) return;
    switch ($databaseservertype){
        case 0:
            break;
        case 1:
            $string="dbname=$databasename user=$databaseuser";
            if (isset($databasepass)) $string.=" password=$databasepass";
            if (isset($databasehost)) $string.=" host=$databasehost";
            if (isset($databaseport)) $string.=" port=$databaseport";
            $databasehandle=pg_connect($string);
            break;
        case 2:
            $databasehandle=mysql_connect($databasehost, $databaseuser, $databasepass);
            mysql_select_db($databasename, $databasehandle);
            break;
    }
}
function database_exec ($query)
{
    global $databasehandle;
    global $databaseservertype;
    if (!isset ($databasehandle)) database_connect();
    switch ($databaseservertype){
        case 0:
            return 0;
        case 1:
            return pg_exec ($databasehandle,$query);
        case 2:
            return mysql_query ($query, $databasehandle); 
    }
}
function database_numrows ($handle)
{
    global $databaseservertype;
    switch ($databaseservertype){
        case 0:
            return 0;
        case 1:
            return pg_numrows ($handle);
        case 2:
            return mysql_num_rows ($handle);
    }
}
function database_fetch_array ($handle,$row)
{
    global $databaseservertype;
    switch ($databaseservertype){
        case 0:
            return 0;
        case 1:
            return pg_fetch_row ($handle,$row);
        case 2:
            mysql_data_seek ($handle,$row);
            return mysql_fetch_row($handle);
    }
}
function database_disconnect ()
{
    global $databasehandle;
    global $databaseservertype;
    switch ($databaseservertype){
        case 0:
            break;
        case 1:
            unset ($databasehandle);
            break;
        case 2:
            mysql_close ($databasehandle);
            unset ($databasehandle);
            break;
    }
}
function database_strtotime ($datestring)
{
    global $databaseservertype;
    switch ($databaseservertype){
        case 0:
            break;
        case 1:
            list ($year,$month,$day,$hour,$minutes,$second,$gmt)=
                sscanf ($datestring,"%d-%d-%d %d:%d:%d%s");
            if ($gmt!="")
              $good_time=("$year-$month-$day $hour:$minutes:$second ${gmt}00");
            else
              $good_time=("$year-$month-$day $hour:$minutes:$second");
            return strtotime ($good_time);
            break;
        case 2:
            list ($year,$month,$day,$hour,$minutes,$second)=
                sscanf ($datestring,"%4d%2d%2d%2d%2d%2d");
              $good_time=("$year-$month-$day $hour:$minutes:$second");
            return strtotime ($good_time);
            break;
    }
}
function server_active()
{
    $EVENT_SERVER_PING=2044;
    $SERVER_PING_LATENCY=60;
    database_connect();
    $database_result=database_exec ("select moment from server_events where event=$EVENT_SERVER_PING order by moment desc");
    list ($moment) =database_fetch_array($database_result,0);
    $when =database_strtotime($moment);
    return (abs($when-time())<($SERVER_PING_LATENCY*2));
}
?>

