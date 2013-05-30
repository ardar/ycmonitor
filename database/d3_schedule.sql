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


CREATE TABLE `yc_monitor_schedule` (
  `ScheduleId` INT(10) UNSIGNED NOT NULL AUTO_INCREMENT,
  `ScheduleName` VARCHAR(50) NOT NULL,
  `BeginTime` INT(11) NOT NULL DEFAULT '0' COMMENT 'timestamp',
  `EndTime` INT(11) DEFAULT '0' COMMENT 'timestamp or times',
  `RepeatType` INT(11) NOT NULL DEFAULT '0' COMMENT '0.once,1.sec,2min,3hour,4day,5week,6month',
  `RepeatParam` INT(10) UNSIGNED DEFAULT '0' COMMENT 'how many units',
  `IsEnabled` TINYINT(1) NOT NULL DEFAULT '0',
  `LastExecTime` INT(11) NOT NULL DEFAULT '0' COMMENT 'timestamp of last executed.',
  `RetriedTimes` INT(11) NOT NULL DEFAULT '0',
  `LastError` INT(11) DEFAULT '0' COMMENT '0:no error , >0:error code',
  `Operation` INT(11) DEFAULT NULL COMMENT 'Query',
  `OperationParam` TEXT,
  PRIMARY KEY (`ScheduleId`)
) ENGINE=INNODB DEFAULT CHARSET=utf8;

/*Data for the table `schedule` */

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;
