/*
SQLyog Ultimate v8.32 
MySQL - 5.1.62-community : Database - d3
*********************************************************************
*/

/*!40101 SET NAMES utf8 */;

/*!40101 SET SQL_MODE=''*/;

/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;
/*Table structure for table `schedule` */

DROP TABLE IF EXISTS `schedule`;

CREATE TABLE `schedule` (
  `ScheduleId` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `ScheduleName` varchar(50) NOT NULL,
  `BeginTime` int(11) NOT NULL DEFAULT '0' COMMENT 'timestamp',
  `EndTime` int(11) DEFAULT '0' COMMENT 'timestamp or times',
  `RepeatType` int(11) NOT NULL DEFAULT '0' COMMENT '0.once,1.sec,2min,3hour,4day,5week,6month',
  `RepeatParam` int(10) unsigned DEFAULT '0' COMMENT 'how many units',
  `IsEnabled` tinyint(1) NOT NULL DEFAULT '0',
  `LastExecTime` int(11) NOT NULL DEFAULT '0' COMMENT 'timestamp of last executed.',
  `RetriedTimes` int(11) NOT NULL DEFAULT '0',
  `LastError` int(11) DEFAULT '0' COMMENT '0:no error , >0:error code',
  `Operation` int(11) DEFAULT NULL COMMENT 'Query',
  `OperationParam` text,
  PRIMARY KEY (`ScheduleId`)
) ENGINE=InnoDB DEFAULT CHARSET=gbk;

/*Data for the table `schedule` */

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;
