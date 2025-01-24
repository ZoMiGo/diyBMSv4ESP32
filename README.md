Mit den vorgenommenen Änderungen hat das BMS-System jetzt mehrere zusätzliche Funktionen und Verbesserungen. Hier sind die wichtigsten Punkte und wie das System auf doppelte ID-Vergaben reagiert:

Neue Funktionen des BMS
	1.	Automatische ID-Vergabe:
	•	Jedes BMS-Modul erhält automatisch eine eindeutige ID, die aus dem EEPROM geladen oder generiert wird.
	•	IDs werden ab 1 aufsteigend vergeben.
	2.	Master-Slave-Erkennung:
	•	Das BMS mit der niedrigsten ID wird automatisch als Master festgelegt.
	•	Der Master-BMS kommuniziert mit dem Victron Cerbo GX und koordiniert die Datenerfassung der Slaves.
	3.	Erkennung und Vermeidung doppelter IDs:
	•	Während der Initialisierung prüft das System, ob mehrere BMS-Module dieselbe ID haben.
	•	Wenn eine doppelte ID erkannt wird, wird dem betroffenen Modul automatisch eine neue, eindeutige ID zugewiesen.
	4.	CAN-Kommunikation:
	•	Der Master-BMS fordert regelmäßig Daten von den Slaves über den CAN-Bus an.
	•	Die gesammelten Daten werden anschließend an das Victron Cerbo GX weitergeleitet.

Was geschieht bei doppelter ID-Vergabe?

1. Während der Initialisierung:
	•	Das System prüft alle vergebenen IDs in der Liste der BMS-Module (bmsList).
	•	Wenn zwei Module dieselbe ID haben, wird dem betroffenen Modul eine neue ID zugewiesen.
	•	Die neue ID wird automatisch generiert und gespeichert.

Beispiel:
	•	Wenn Modul 3 und Modul 5 beide die ID 2 haben, erhält Modul 5 eine neue ID, z. B. 11 (je nach EEPROM-Zählerstand).

2. Nach der Initialisierung oder während des Betriebs:
	•	Das System geht davon aus, dass doppelte IDs ausgeschlossen sind, da die Funktion checkDuplicateIDs() während der Initialisierung aufgerufen wurde.
	•	Sollte trotzdem ein Fehler auftreten (z. B. durch Hardwareprobleme oder manuelle Änderungen der IDs), könnten Kommunikationsprobleme entstehen, da der Master möglicherweise nicht weiß, welches Modul angesprochen wird.

Kann es noch zu doppelten IDs kommen?

Normalerweise nein, weil:
	•	Die Funktion checkDuplicateIDs() überprüft alle vergebenen IDs und korrigiert sie direkt.
	•	Neue IDs werden automatisch durch die Funktion generateBMSID() generiert, wobei sichergestellt wird, dass jede ID eindeutig ist.

Mögliche Ausnahmefälle:
	•	Wenn jemand manuell IDs in der Software oder auf der Hardware falsch einstellt, ohne die Überprüfung durchzuführen, könnte es vorübergehend zu Problemen kommen.
	•	Solche Fälle sind aber unwahrscheinlich, da die Überprüfung direkt nach der Initialisierung erfolgt.

Zusammenfassung der Funktionen:
	1.	Vermeidung von doppelten IDs:
	•	Während der Initialisierung werden doppelte IDs erkannt und behoben.
	2.	Master-Slave-Architektur:
	•	Der Master-BMS koordiniert die Kommunikation und sendet Daten an das Victron Cerbo GX.
	•	Slaves antworten nur auf Anfragen des Masters.
	3.	CAN-Datenkommunikation:
	•	Effiziente Kommunikation zwischen Master, Slaves und Victron-Geräten.
	4.	Erweiterbarkeit:
	•	Das System unterstützt bis zu 10 BMS-Module und kann problemlos angepasst werden.
	5.	Fehlertoleranz:
	•	Wenn doppelte IDs vorhanden sind, werden diese automatisch korrigiert, ohne dass manuell eingegriffen werden muss.

# diyBMS v4

Version 4 of the diyBMS.  Do-it-yourself battery management system for Lithium ion battery packs and cells

If you are looking for version 3 of this project take a look here https://github.com/stuartpittaway/diyBMS

THIS CODE IS FOR THE NEW CONTROLLER (AFTER FEB 2021) AND ESP32 DEVKIT-C

# Support the project

If you find the BMS useful, please consider buying me a beer, check out [Patreon](https://www.patreon.com/StuartP) for more information.

You can also send beer tokens via Paypal - [https://paypal.me/stuart2222](https://paypal.me/stuart2222)

Any donations go towards the on going development and prototype costs of the project.

# Videos on how to use and build

https://www.youtube.com/stuartpittaway

### Video on how to program the devices
https://youtu.be/wTqDMg_Ql98

### Video on how to order from JLCPCB
https://youtu.be/E1OS0ZOmOT8


# How to use the code

Instructions for programming/flashing the hardware can be [found here](ProgrammingHardware.md)

# Help

If you need help, ask over at the [forum](https://community.openenergymonitor.org/t/diybms-v4)

If you discover a bug or want to make a feature suggestion, open a Github issue

# Hardware

Hardware for this code is in a seperate repository, and consists of a controller (you need 1 of these) and modules (one per series cell in your battery)

https://github.com/stuartpittaway/diyBMSv4



# WARNING

This is a DIY product/solution so don’t use this for safety critical systems or in any situation where there could be a risk to life.  

There is no warranty, it may not work as expected or at all.

The use of this project is done so entirely at your own risk.  It may involve electrical voltages which could kill - if in doubt, seek help.

The use of this project may not be compliant with local laws or regulations - if in doubt, seek help.


# How to compile the code yourself

The code uses [PlatformIO](https://platformio.org/) to build the code.  There isn't any need to compile the code if you simply want to use it, see "How to use the code" above.

If you want to make changes, fix bugs or poke around, use platformio editor to open the workspace named "diybms_workspace.code-workspace"


# License

This work is licensed under a Creative Commons Attribution-NonCommercial-ShareAlike 2.0 UK: England & Wales License.

https://creativecommons.org/licenses/by-nc-sa/2.0/uk/

You are free to:
* Share — copy and redistribute the material in any medium or format
* Adapt — remix, transform, and build upon the material
The licensor cannot revoke these freedoms as long as you follow the license terms.

Under the following terms:
* Attribution — You must give appropriate credit, provide a link to the license, and indicate if changes were made. You may do so in any reasonable manner, but not in any way that suggests the licensor endorses you or your use.
* Non-Commercial — You may not use the material for commercial purposes.
* ShareAlike — If you remix, transform, or build upon the material, you must distribute your contributions under the same license as the original.
* No additional restrictions — You may not apply legal terms or technological measures that legally restrict others from doing anything the license permits.

Notices:
You do not have to comply with the license for elements of the material in the public domain or where your use is permitted by an applicable exception or limitation.

No warranties are given. The license may not give you all of the permissions necessary for your intended use. For example, other rights such as publicity, privacy, or moral rights may limit how you use the material.

