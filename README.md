# PP_Rechnernetze_IoT_public
Projekt im Rahmen des Programmierpraktikums Rechnernetze im Sommersemster 2021 an der UOS.

Ziel: Aktivitätsüberwachung alleinlebender Senioren. Falls die Person zuhause ist und über längere Zeit (z.B. 3h) keine Aktivität stattfindet, werden Bezugspersonen benachrichtigt.
Versuche: IRObstaclesensor zur Überwachung einer Zimmertüt (-> Reichweite zu gering), Ultraschallsensor zur Überwachung von Zimmertüren (funktioniert), PIR-Bewegungsmelder: Aktivitätserfassung in Aufenthaltsbereichen wie bspw. einer Sitzecke. Lora: TTGO t-Beam als Sender und Empfänger. Erwartung: mit einer Person im Raum ist der RSSI-Wert weniger stabil, sodass die Anwesenheit erkannt werden kann (-> funktioniert nur eingeschränkt, daher nicht nutzbar). Bluetooth: Analog zu Lora. Keine Veränderungen erkennbar. BLE: Beacon sowohl zu Anwesenheitserkennung (funktioniert), als auch zur vereinfachten Triangulation, also Erkennung, ob die Position des Beacon deutlich verändert wurde.
Fazit: Kombination aus PIR- und Ultraschallsensor und Beacon ist sinnvoll.
Die Daten (Pings und Aktivitätserfassungen) werden in einer Datenbank gespeichert, die letzte Bewegung je Sensor ist auf der lokalen Website einsehbar. Auf der Website können außerdem Emailadressen und Telegram IDs eingegeben werden um Benachrichtigungen bei Inaktivität oder Funktionsstörungen zu erhalten. 

Literatur Poster:
  Ärzteblatt (2018): Unfallchirurgen und Geriater warnen vor Sturzgefahr im Alter. URL: https://www.aerzteblatt.de/nachrichten/98013/Unfallchirurgen-und-Geriater-warnen-vor-Sturzgefahr-im-Alter
  BMBF (o.J.a): Viele Erkrankungen werden mit dem Alter häufig. URL: https://www.gesundheitsforschung-bmbf.de/de/viele-erkrankungen-werden-mit-dem-alter-haufig-6786.php
  BMBF (o.J.b): Demenz: Wenn Vergesslichkeit zur Krankheit wird. URL: https://www.gesundheitsforschung-bmbf.de/de/demenz-wenn-vergesslichkeit-zur-krankheit-wird-5948.php
  DESTATIS (2018): 
  Zahl der Woche Nr. 49 vom 4. Dezember 2018. URL: https://www.destatis.de/DE/Presse/Pressemitteilungen/Zahl-der-Woche/2018/PD18_49_p002.html
  Statista(2009): Was sind Ihre wichtigsten Wünsche für das Alter? URL: https://de.statista.com/statistik/daten/studie/5418/umfrage/wuensche-fuer-das-alter/
