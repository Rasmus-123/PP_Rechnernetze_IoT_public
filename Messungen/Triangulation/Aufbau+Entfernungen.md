# Triangulation Mess-Infos

RSSI1 und RSSI2 sind TTGOs mit [TriangulationBLE](../../Arduino/TriangulationBLE/TriangulationBLE.ino).

RSSI3 ist ein RasPi der mit [Node-Red](https://github.com/yossihard/node-red-contrib-blebeacon-scanner) misst.

## Aufbau

RSSI1  
|  
|  
|  
RSSI3---------------RSSI2

### Rechteck - Abstände

- RSSI1 <-> RSSI3 = ~2,3m
- RSSI2 <-> RSSI3 = ~3,2m
- RSSI1 <-> RSSI2 = ~3,9m

### Quadrat - Abstände

- RSSI1 <-> RSSI3 = ~2,3m
- RSSI2 <-> RSSI3 = ~2,3m
- RSSI1 <-> RSSI2 = ~3,2m
