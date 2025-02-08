DIYBMS v4 Modular – ESP32 BMS System

⚠️ WARNUNG
	•	Dies ist ein DIY-Projekt. Nicht für sicherheitskritische Anwendungen geeignet!
	•	Keine Garantie – Nutzung auf eigene Gefahr!
	•	Hochspannung kann tödlich sein! Falls du dir unsicher bist, hole dir professionelle Hilfe.
	•	Die Nutzung dieses Projekts könnte gegen lokale Vorschriften verstoßen. Informiere dich vorher!

📌 Projektübersicht

DIYBMS v4 ist eine Open-Source-Lösung zur Überwachung und Steuerung von Lithium-Ionen-Batterien.
Mit einem ESP32 als BMS-Controller und einzelnen Modulen für jede Zelle verwaltet es Spannung, Temperatur, Ladezustand (SOC) und Kommunikation mit externen Geräten wie Victron Cerbo GX.

	Vorherige Versionen:
Falls du nach DIYBMS v3 suchst: Hier geht’s zum Repository

🆕 Neue Funktionen & Verbesserungen

Diese Version bringt viele neue Features für ein stabiles, sicheres und effizientes BMS:

1️⃣ Automatische ID-Vergabe an Module
	•	Die BMS-Module erhalten beim Start automatisch eine eindeutige ID.
	•	IDs werden im EEPROM gespeichert und beim Neustart wiederhergestellt.
	•	Falls eine ID fehlt oder fehlerhaft ist, wird sie automatisch neu vergeben.

2️⃣ Master-Slave-System
	•	Das Modul mit der niedrigsten ID wird automatisch als Master bestimmt.
	•	Alle anderen Module sind Slaves und melden ihre Werte an den Master.
	•	Die Master- / Slave-Rolle wird auf dem Display und im Web-Interface angezeigt.

3️⃣ Erkennung & Korrektur doppelter IDs
	•	Falls zwei Module die gleiche ID haben, wird das Problem erkannt und automatisch behoben.
	•	Eine neue, eindeutige ID wird zugewiesen und gespeichert.

4️⃣ Integration mit Victron Cerbo GX über CAN-Bus
	•	Datenübertragung per CAN-Bus an Victron-Geräte.
	•	Spannung, SOC, Strom & Ladezustand direkt in Victron VRM & GX-Geräten sichtbar.

5️⃣ Erweiterte SOC-Berechnung
	•	Verbesserte Ladezustandsberechnung (SOC) basierend auf Zellspannungen und Kapazität.
	•	Dynamische Anpassung der SOC-Werte basierend auf realen Messungen.

6️⃣ WiFi & Web-Interface
	•	WiFi-Setup über das TFT-Display möglich!
	•	IP-Adresse wird auf dem Display angezeigt.
	•	Komplette Web-Oberfläche zur Überwachung und Konfiguration.

🔧 Hardware-Anforderungen
	•	ESP32 DevKit-C (Controller)
	•	BMS-Module (1 pro Zelle) – Verbindet sich mit dem ESP32
	•	TFT-Display (320x240 SPI LCD) für lokale Anzeige
	•	CAN-Bus-Adapter für Victron-Kompatibilität

🚀 Installation & Einrichtung

1️⃣ Flashen des ESP32
	•	Code mit PlatformIO (VS Code) kompilieren und auf den ESP32 hochladen.

2️⃣ WiFi konfigurieren
	•	WiFi-Daten direkt über das TFT-Display eingeben.
	•	Alternativ per Web-Oberfläche nach dem ersten Start.

3️⃣ BMS-Module anschließen
	•	Die Module verbinden sich automatisch mit dem ESP32.
	•	Master wird automatisch gewählt.

4️⃣ Victron-Geräte verbinden (optional)
	•	Falls du Victron-Produkte nutzt, CAN-Bus anschließen und Daten automatisch übertragen lassen.

💡 Wie benutze ich das BMS?
	•	Live-Daten auf dem Display und im Web-Interface ansehen (Spannung, Strom, SOC, Temperatur).
	•	Warnmeldungen & Fehler auf dem Display erkennen (Übertemperatur, Zellspannung zu hoch/niedrig).
	•	Lade- & Entladegrenzen konfigurieren (Schutz vor Tiefentladung/Überladung).
	•	Daten per MQTT, CAN-Bus oder Web-API abrufen.

🎥 Videos & Tutorials
	•	📺 Einführung & Aufbau: YouTube
	•	🔌 Wie programmiere ich den ESP32? Hier klicken
	•	🛒 Wie bestelle ich PCBs von JLCPCB? Video-Anleitung

⚙️ Wie kompiliere ich den Code selbst?
	•	Der Code nutzt PlatformIO für die Kompilierung.
	•	Lade das Repository herunter und öffne die Workspace-Datei in PlatformIO.
	•	Kein Code-Kompilieren nötig, wenn du nur das BMS nutzen willst.

🤝 Unterstütze das Projekt!

Falls dir das DIYBMS hilft, kannst du mich unterstützen:

☕ Patreon: Unterstütze mich monatlich für neue Entwicklungen.
🍻 Paypal-Spende: Hilf mir, die Entwicklungskosten zu decken.

📜 Lizenz & Rechtliches

🔹 Lizenz: Creative Commons Attribution-NonCommercial-ShareAlike 2.0 UK
🔹 Keine gewerbliche Nutzung erlaubt!
🔹 Du kannst den Code modifizieren, aber musst ihn Open-Source halten.

📄 Lizenzdetails: Hier nachlesen

❗ Letzte Warnung
	•	KEINE GARANTIE!
	•	Nutzung auf eigene Gefahr!
	•	Gefährliche Spannungen möglich!
	•	Nicht für sicherheitskritische Anwendungen!

Falls du unsicher bist, hole dir professionelle Hilfe.

