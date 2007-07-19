#
# Table structure for table `weather`
#

CREATE TABLE `weather` (
  `datetime` datetime NOT NULL default '0000-00-00 00:00:00',
  `temp_in` decimal(4,1) NOT NULL default '0.0',
  `temp_out` decimal(4,1) NOT NULL default '0.0',
  `dewpoint` decimal(4,1) NOT NULL default '0.0',
  `rel_hum_in` tinyint(3) NOT NULL default '0',
  `rel_hum_out` tinyint(3) NOT NULL default '0',
  `wind_speed` decimal(3,1) NOT NULL default '0.0',
  `wind_angle` decimal(4,1) NOT NULL default '0.0',
  `wind_direction` char(3) NOT NULL default '',
  `wind_chill` decimal(4,1) NOT NULL default '0.0',
  `rain_1h` decimal(3,1) NOT NULL default '0.0',
  `rain_24h` decimal(3,1) NOT NULL default '0.0',
  `rain_total` decimal(5,1) NOT NULL default '0.0',
  `rel_pressure` decimal(5,1) NOT NULL default '0.0',
  `tendency` varchar(7) NOT NULL default '',
  `forecast` varchar(6) NOT NULL default '',
  UNIQUE KEY `timestamp` (`timestamp`)
) TYPE=MyISAM;
