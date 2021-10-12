# Setup und so

~Rasmus

## Raspberry Pi

### OS Installation

[Quelle](https://www.tomshardware.com/reviews/raspberry-pi-headless-setup-how-to,6028.html)

### WLAN Access Point

[Quelle](https://www.raspberrypi.org/documentation/computers/configuration.html#setting-up-a-routed-wireless-access-point)

### Change Hostname

`sudo raspi-config`  
`1 - System Options`  
`S4 - Hostname`  

## Datenbank

### Installation

[Quelle](https://raspberrytips.com/install-mariadb-raspberry-pi/)

`sudo apt install mariadb-server`

`sudo mysql_secure_installation`

### Neuer Nutzer

#### Admin

`CREATE USER '<username>'@'localhost' IDENTIFIED BY '<password>';`  
`GRANT ALL PRIVILEGES ON *.* TO '<username>'@'localhost' <WITH GRANT OPTION>;`

Da der `root` Nutzer nur mit sudo genutzt werden kann.

Für unsere Node-Red Konfiguration: `admin` mit `password`

### Remote Access

[Quelle](https://stackoverflow.com/a/23270907/12545504)

#### SSH Tunnel

`ssh -NL 3307:localhost:3306 pi@raspberrypi.local`

localhost:3307 leitet auf localhost:3306 des RasPi um.

#### PHPMyAdmin Config

In `config.inc.php` anhängen:  

```php
/*
 * Server on RasPi
 */
$i++;
$cfg['Servers'][$i]['verbose']       = 'RasPi';
$cfg['Servers'][$i]['host']          = '127.0.0.1';
$cfg['Servers'][$i]['port']          = '3307';
$cfg['Servers'][$i]['connect_type']  = 'tcp';
$cfg['Servers'][$i]['extension']     = 'mysqli';
$cfg['Servers'][$i]['compress']      = FALSE;
$cfg['Servers'][$i]['auth_type']     = 'config';
$cfg['Servers'][$i]['user']          = 'admin';
$cfg['Servers'][$i]['password']      = 'password';
```

### Datenbank aufsetzen

Code aus `RasPi/Datenbank/sensordb.sql` in Datenbank ausführen.

### Zeitzonen

`sudo raspi-config`
Zeitzone einstellen.

`mysql_tzinfo_to_sql /usr/share/zoneinfo | sudo mysql -u root -p mysql`

## Website

`mkdir ~/logs`

Have `~/scripts` with the Scripts

Edit `Webserver/appsettings.json`

Update ConnectionStrings.SensorDB based on your Database.
Decide on Authentication Options.

### SSL

Ohne Zertifikat einfach die https-Adresse in `appsettings.json` weglassen.

### Run

In Terminal  
`./Webserver`

In the Background  
`nohup ./Webserver > log.txt 2>&1`

On Startup  
`crontab -e`  
`@reboot bash /home/pi/start_server.sh`

### Script Permissions

`sudo visudo`  

```sudoers
ALL ALL=NOPASSWD: /home/pi/scripts/change_wifi_network.sh
ALL ALL=NOPASSWD: /home/pi/scripts/reboot.sh
ALL ALL=NOPASSWD: /home/pi/scripts/getSSID.sh
ALL ALL=NOPASSWD: /home/pi/scripts/getPSK.sh
```

### Port 80 Redirect

Add to `/etc/rc.local`

`iptables -t nat -I PREROUTING -p tcp --dport 80 -j REDIRECT --to-ports 5000`

Where 5000 = The http Port in `appsettings.json`.  
Do the same with `--dport 443` and the https-port if you want to use https.  

### Fix Windows Line-Endings

`sed -i -e 's/\r$//' scriptname.sh`  
`sed -i -e 's/^M$//' scriptname.sh`

## Mehr Installationen

### Mosquitto

`sudo apt install mosquitto mosquitto-clients`  
`sudo systemctl enable mosquitto.service`

### Node-Red

`bash <(curl -sL https://raw.githubusercontent.com/node-red/linux-installers/master/deb/update-nodejs-and-nodered)`  
`sudo systemctl enable nodered.service`
