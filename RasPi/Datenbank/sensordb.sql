-- phpMyAdmin SQL Dump
-- version 5.0.2
-- https://www.phpmyadmin.net/
--
-- Host: 127.0.0.1:3307
-- Erstellungszeit: 11. Okt 2021 um 14:25
-- Server-Version: 10.3.29-MariaDB-0+deb10u1
-- PHP-Version: 7.4.9

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
START TRANSACTION;
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- Datenbank: `sensordb`
--
CREATE DATABASE IF NOT EXISTS `sensordb` DEFAULT CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci;
USE `sensordb`;

-- --------------------------------------------------------

--
-- Tabellenstruktur für Tabelle `email-recipients`
--

CREATE TABLE `email-recipients` (
  `ID` int(11) NOT NULL,
  `Address` varchar(50) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- --------------------------------------------------------

--
-- Tabellenstruktur für Tabelle `lora`
--

CREATE TABLE `lora` (
  `ID` int(11) NOT NULL,
  `Sensor` int(11) NOT NULL,
  `Timestamp` datetime NOT NULL,
  `Value` int(11) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- --------------------------------------------------------

--
-- Tabellenstruktur für Tabelle `pings`
--

CREATE TABLE `pings` (
  `ID` int(11) NOT NULL,
  `Sensor` int(11) NOT NULL,
  `Timestamp` datetime NOT NULL,
  `Status` varchar(64) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- --------------------------------------------------------

--
-- Tabellenstruktur für Tabelle `rssi`
--

CREATE TABLE `rssi` (
  `ID` int(11) NOT NULL,
  `Sensor` int(11) NOT NULL,
  `Timestamp` datetime NOT NULL,
  `Value` int(11) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- --------------------------------------------------------

--
-- Tabellenstruktur für Tabelle `schranken-unterbrechungen`
--

CREATE TABLE `schranken-unterbrechungen` (
  `ID` int(11) NOT NULL,
  `Sensor` int(11) NOT NULL,
  `Timestamp` datetime NOT NULL,
  `Value` varchar(50) NOT NULL COMMENT '???'
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- --------------------------------------------------------

--
-- Tabellenstruktur für Tabelle `sensoren`
--

CREATE TABLE `sensoren` (
  `ID` int(11) NOT NULL,
  `Identifier` varchar(50) NOT NULL,
  `DisplayName` varchar(50) DEFAULT NULL,
  `Type` varchar(50) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- --------------------------------------------------------

--
-- Tabellenstruktur für Tabelle `telegram`
--

CREATE TABLE `telegram` (
  `ID` int(11) NOT NULL,
  `Address` varchar(50) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- --------------------------------------------------------

--
-- Tabellenstruktur für Tabelle `triangulation`
--

CREATE TABLE `triangulation` (
  `ID` int(11) NOT NULL,
  `Timestamp` datetime NOT NULL,
  `rssi1` int(11) NOT NULL,
  `rssi2` int(11) NOT NULL,
  `rssi3` int(11) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

--
-- Indizes der exportierten Tabellen
--

--
-- Indizes für die Tabelle `email-recipients`
--
ALTER TABLE `email-recipients`
  ADD PRIMARY KEY (`ID`);

--
-- Indizes für die Tabelle `lora`
--
ALTER TABLE `lora`
  ADD PRIMARY KEY (`ID`),
  ADD KEY `Sensor-LoRa` (`Sensor`);

--
-- Indizes für die Tabelle `pings`
--
ALTER TABLE `pings`
  ADD PRIMARY KEY (`ID`),
  ADD KEY `Sensor` (`Sensor`);

--
-- Indizes für die Tabelle `rssi`
--
ALTER TABLE `rssi`
  ADD PRIMARY KEY (`ID`),
  ADD KEY `Sensor-RSSI` (`Sensor`);

--
-- Indizes für die Tabelle `schranken-unterbrechungen`
--
ALTER TABLE `schranken-unterbrechungen`
  ADD PRIMARY KEY (`ID`),
  ADD KEY `Sensor-Schranke` (`Sensor`);

--
-- Indizes für die Tabelle `sensoren`
--
ALTER TABLE `sensoren`
  ADD PRIMARY KEY (`ID`),
  ADD UNIQUE KEY `Identifier_UNIQUE` (`Identifier`) USING BTREE;

--
-- Indizes für die Tabelle `telegram`
--
ALTER TABLE `telegram`
  ADD PRIMARY KEY (`ID`);

--
-- Indizes für die Tabelle `triangulation`
--
ALTER TABLE `triangulation`
  ADD PRIMARY KEY (`ID`);

--
-- AUTO_INCREMENT für exportierte Tabellen
--

--
-- AUTO_INCREMENT für Tabelle `email-recipients`
--
ALTER TABLE `email-recipients`
  MODIFY `ID` int(11) NOT NULL AUTO_INCREMENT;

--
-- AUTO_INCREMENT für Tabelle `lora`
--
ALTER TABLE `lora`
  MODIFY `ID` int(11) NOT NULL AUTO_INCREMENT;

--
-- AUTO_INCREMENT für Tabelle `pings`
--
ALTER TABLE `pings`
  MODIFY `ID` int(11) NOT NULL AUTO_INCREMENT;

--
-- AUTO_INCREMENT für Tabelle `rssi`
--
ALTER TABLE `rssi`
  MODIFY `ID` int(11) NOT NULL AUTO_INCREMENT;

--
-- AUTO_INCREMENT für Tabelle `schranken-unterbrechungen`
--
ALTER TABLE `schranken-unterbrechungen`
  MODIFY `ID` int(11) NOT NULL AUTO_INCREMENT;

--
-- AUTO_INCREMENT für Tabelle `sensoren`
--
ALTER TABLE `sensoren`
  MODIFY `ID` int(11) NOT NULL AUTO_INCREMENT;

--
-- AUTO_INCREMENT für Tabelle `telegram`
--
ALTER TABLE `telegram`
  MODIFY `ID` int(11) NOT NULL AUTO_INCREMENT;

--
-- AUTO_INCREMENT für Tabelle `triangulation`
--
ALTER TABLE `triangulation`
  MODIFY `ID` int(11) NOT NULL AUTO_INCREMENT;

--
-- Constraints der exportierten Tabellen
--

--
-- Constraints der Tabelle `pings`
--
ALTER TABLE `pings`
  ADD CONSTRAINT `Sensor-Ping` FOREIGN KEY (`Sensor`) REFERENCES `sensoren` (`ID`) ON DELETE CASCADE ON UPDATE CASCADE;


--
-- Constraints der Tabelle `lora`
--
ALTER TABLE `lora`
  ADD CONSTRAINT `Sensor-LoRa` FOREIGN KEY (`Sensor`) REFERENCES `sensoren` (`ID`) ON DELETE CASCADE ON UPDATE CASCADE;
--
-- Constraints der Tabelle `schranken-unterbrechungen`
--
ALTER TABLE `schranken-unterbrechungen`
  ADD CONSTRAINT `Sensor-Schranke` FOREIGN KEY (`Sensor`) REFERENCES `sensoren` (`ID`) ON DELETE CASCADE ON UPDATE CASCADE;
COMMIT;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
