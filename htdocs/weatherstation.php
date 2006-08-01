<?
/*	weatherstation.php  - part of the open2300 package
 *
 *	Version 1.3
 *
 *	Control WS2300 weather station
 *
 *	Copyright 2003,2004, Kenneth Lavrsen
 *	This program is published under the GNU Public license
 */
exec("/usr/local/bin/fetch2300",$fetcharray);
foreach ($fetcharray as $value)
{
	list($parameter,$parvalue)=explode(" ", $value);
	$ws["$parameter"]=$parvalue;
}
$forecastpic= strtolower($ws["Forecast"]) . ".jpg";
$tendencypic= strtolower($ws["Tendency"]) . ".png";

?>

<html>

<head>
<title>Kenneth's Weather Station, Glostrup, Denmark</title>
<META HTTP-EQUIV="Expires" CONTENT="Tue, 01 Jan 1980 1:00:00 GMT">  
<META HTTP-EQUIV="Pragma" CONTENT="no-cache">  
<META HTTP-EQUIV="Refresh" CONTENT="60">  
</head>

<body bgcolor="honeydew" text="black" link="blue" vlink="purple" alink="red">

<h1 align="center">Kenneth's Weather Station, Glostrup, Denmark</h1>
<table border="3" width="700" align="center" bgcolor="ivory">
  <tr>
    <td width="100%">
      <table border="0" cellpadding="0" cellspacing="0" width=100%">
        <tr>

            <td width="55%" valign="top">
                <table border="1" width="100%">
                    <tr>
                        <td width="50%" height="170" valign="top">
                            <center><b><?=$ws["Time"]?>&nbsp;&nbsp;<?=$ws["Date"]?></b></center><br>
                            <img src="<?=$forecastpic?>" align="left">
                            <br>Forecast: <?=$ws["Forecast"]?><br><br>
                            Tendency: <?=$ws["Tendency"]?> <img src="<?=$tendencypic?>" align="middle">
                        </td>
                         </td>
                        <td width="50%" height="170" valign="top">
                            Relative Pressure
                            <p align="center"><font size="5"><?=number_format($ws["RP"],1,'.','')?> hPa</font></p>
                            <p><font size="2">Minimum: <?=number_format($ws["RPmin"],1,'.','')?> hPa</font><br>
                               <font size="2"><center><?=$ws["TRPmin"]?> <?=$ws["DRPmin"]?></center></font>
                            <p><font size="2">Maximum: <?=number_format($ws["RPmax"],1,'.','')?> hPa</font><br>
                               <font size="2"><center><?=$ws["TRPmax"]?> <?=$ws["DRPmax"]?></center></font>
                        </td>
                    <tr>
                </table>
                <table border="1" width="100%">
                    <tr>
                        <td width="50%" height="170" valign="top">
                            <p>Wind Speed</p>
                            <p align="center"><font size="5"><?=$ws["WS"]?> m/s</font></p>
                            <p><font size="2">Minimum: <?=$ws["WSmin"]?> m/s</font><br>
                               <font size="2"><center><?=$ws["TWSmin"]?> <?=$ws["DWSmin"]?></center></font>
                            <p><font size="2">Maximum: <?=$ws["WSmax"]?> m/s</font><br>
                               <font size="2"><center><?=$ws["TWSmax"]?> <?=$ws["DWSmax"]?></center></font>
                         </td>
                        <td width="50%" height="170" valign="top">
                            <p>Wind Direction</p>
                            <p align="center"><font size="5"><?=$ws["DIRtext"]?></font><br><font size="5"><?=$ws["DIR0"]?> &deg</font></p>
                            <p><font size="2">Last 5 directions<br>
                               <center><?=$ws["DIR1"]?> &deg, <?=$ws["DIR2"]?> &deg, <?=$ws["DIR3"]?> &deg,<br>
                               <?=$ws["DIR4"]?> &deg, <?=$ws["DIR5"]?> &deg</center></font>
                        </td>
                    <tr>
                </table>
                <table border="1" width="100%">
                    <tr>
                        <td width="33%" height="190" valign="top">
                            <p>Rain Total</p>
                            <p align="center"><font size="5"><?=number_format($ws["Rtot"],1,'.','')?> mm</font></p>
                            <p><br><font size="2">Since:</font><br>
                                   <font size="2"><center><?=$ws["TRtot"]?> <?=$ws["DRtot"]?></center></font>
                         </td>
                        <td width="33%" height="190" valign="top">
                            <p>Rain last 24 hours</p>
                            <p align="center"><font size="5"><?=number_format($ws["R24h"],1,'.','')?> mm</font></p>
                            <p><br><font size="2">Maximum: <?=number_format($ws["R24hmax"],1,'.','')?> mm</font><br>
                                   <font size="2"><center><?=$ws["TR24hmax"]?> <?=$ws["DR24hmax"]?></center></font>
                         </td>
                         <td width="33%" height="190" valign="top">
                            <p>Rain last 1 hour</p>
                            <p align="center"><font size="5"><?=number_format($ws["R1h"],1,'.','')?> mm</font></p>
                            <p><br><font size="2">Maximum: <?=number_format($ws["R1hmax"],1,'.','')?> mm</font><br>
                                   <font size="2"><center><?=$ws["TR1hmax"]?> <?=$ws["DR1hmax"]?></center></font>
                        </td>
                    <tr>
                </table>

            </td>
            <td width="45%" valign="top">
                <table border="1" width="100%">
                    <tr>
                        <td width="50%" height="170" valign="top">
                            <p>Temperature, Indoor</p>
                            <p align="center"><font size="5"><?=$ws["Ti"]?> &deg;C</font></p>
                            <p><font size="2">Minimum: <?=$ws["Timin"]?> &deg;C</font><br>
                               <font size="2"><center><?=$ws["TTimin"]?> <?=$ws["DTimin"]?></center></font>
                            <p><font size="2">Maximum: <?=$ws["Timax"]?> &deg;C</font><br>
                               <font size="2"><center><?=$ws["TTimax"]?> <?=$ws["DTimax"]?></center></font>
                         </td>
                        <td width="50%" height="170" valign="top">
                            <p>Temperature, Outdoor</p>
                            <p align="center"><font size="5"><?=$ws["To"]?> &deg;C</font></p>
                            <p><font size="2">Minimum: <?=$ws["Tomin"]?> &deg;C</font><br>
                               <font size="2"><center><?=$ws["TTomin"]?> <?=$ws["DTomin"]?></center></font>
                            <p><font size="2">Maximum: <?=$ws["Tomax"]?> &deg;C</font><br>
                               <font size="2"><center><?=$ws["TTomax"]?> <?=$ws["DTomax"]?></center></font>
                        </td>
                    <tr>
                </table>
                <table border="1" width="100%">
                    <tr>
                        <td width="50%" height="170" valign="top">
                            <p>Dewpoint</p>
                            <p align="center"><font size="5"><?=$ws["DP"]?> &deg;C</font></p>
                            <p><font size="2">Minimum: <?=$ws["DPmin"]?> &deg;C</font><br>
                               <font size="2"><center><?=$ws["TDPmin"]?> <?=$ws["DDPmin"]?></center></font>
                            <p><font size="2">Maximum: <?=$ws["DPmax"]?> &deg;C</font><br>
                               <font size="2"><center><?=$ws["TDPmax"]?> <?=$ws["DDPmax"]?></center></font>
                        </td>
                        <td width="50%" height="170" valign="top">
                            <p>Windchill</p>
                            <p align="center"><font size="5"><?=$ws["WC"]?> &deg;C</font></p>
                            <p><font size="2">Minimum: <?=$ws["WCmin"]?> &deg;C</font><br>
                               <font size="2"><center><?=$ws["TWCmin"]?> <?=$ws["DWCmin"]?></center></font>
                            <p><font size="2">Maximum: <?=$ws["WCmax"]?> &deg;C</font><br>
                               <font size="2"><center><?=$ws["TWCmax"]?> <?=$ws["DWCmax"]?></center></font>
                          </td>
                    <tr>
                </table>
                <table border="1" width="100%">
                    <tr>
                        <td width="50%" height="190" valign="top">
                            <p>Relative Humidity, Indoor</p>
                            <p align="center"><font size="5"><?=$ws["RHi"]?> %</font></p>
                            <p><font size="2">Minimum: <?=$ws["RHimin"]?> %</font><br>
                               <font size="2"><center><?=$ws["TRHimin"]?> <?=$ws["DRHimin"]?></center></font>
                            <p><font size="2">Maximum: <?=$ws["RHimax"]?> %</font><br>
                               <font size="2"><center><?=$ws["TRHimax"]?> <?=$ws["DRHimax"]?></center></font>
                         </td>
                        <td width="50%" height="190" valign="top">
                            <p>Relative Humidity, Outdoor</p>
                            <p align="center"><font size="5"><?=$ws["RHo"]?> %</font></p>
                            <p><font size="2">Minimum: <?=$ws["RHomin"]?> %</font><br>
                               <font size="2"><center><?=$ws["TRHomin"]?> <?=$ws["DRHomin"]?></center></font>
                            <p><font size="2">Maximum: <?=$ws["RHomax"]?> %</font><br>
                               <font size="2"><center><?=$ws["TRHomax"]?> <?=$ws["DRHomax"]?></center></font>
                        </td>
                    <tr>
                </table>
            </td>

        </tr>
      </table>
    </td>
  </tr>
</table>
<p align="center">Weather Data are loaded directly from Weather Station which gives a 5-30 seconds delay</p>
<p>
<p align="center"><a href="index.htm">[Weather Index]</a> <a href="weathergraphs.php">[Weather Graphs]</a> 
<a href="phpweather/index.php?icao=EKCH&language=en">[World Weather]</a> <a href="/">[Kenneth's Homepage]</a></p>
</body>

</html>
