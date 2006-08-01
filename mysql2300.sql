# phpMyAdmin SQL Dump
# version 2.5.4
# http://www.phpmyadmin.net
#
# Host: localhost
# Generation Time: Feb 21, 2004 at 02:04 AM
# Server version: 4.0.16
# PHP Version: 4.3.3
# 
# Database : `weather`
# 

# --------------------------------------------------------

#
# Table structure for table `weather`
#

CREATE TABLE `weather` (
  `timestamp` bigint(14) NOT NULL default '0',
  `rec_date` date NOT NULL default '0000-00-00',
  `rec_time` time NOT NULL default '00:00:00',
  `temp_in` decimal(3,1) NOT NULL default '0.0',
  `temp_out` decimal(3,1) NOT NULL default '0.0',
  `dewpoint` decimal(3,1) NOT NULL default '0.0',
  `rel_hum_in` tinyint(3) NOT NULL default '0',
  `rel_hum_out` tinyint(3) NOT NULL default '0',
  `windspeed` decimal(3,1) NOT NULL default '0.0',
  `wind_angle` decimal(3,1) NOT NULL default '0.0',
  `wind_direction` char(3) NOT NULL default '',
  `wind_chill` decimal(3,1) NOT NULL default '0.0',
  `rain_1h` decimal(3,1) NOT NULL default '0.0',
  `rain_24h` decimal(3,1) NOT NULL default '0.0',
  `rain_total` decimal(4,1) NOT NULL default '0.0',
  `rel_pressure` decimal(4,1) NOT NULL default '0.0',
  `tendency` varchar(7) NOT NULL default '',
  `forecast` varchar(6) NOT NULL default '',
  UNIQUE KEY `timestamp` (`timestamp`)
) TYPE=MyISAM;
