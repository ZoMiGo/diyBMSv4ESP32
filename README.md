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

Here are the new functions described in detail:

1. initializeBMSIDs()
Purpose:
Assigns unique IDs to all BMS modules during system initialization. It ensures that each module has a valid and distinct ID.

Functionality:

Iterates through all modules (default: 10 modules).
Checks the stored ID in the EEPROM for each module.
If an ID is missing (set to 0) or invalid (greater than 100), a new ID is assigned.
Newly assigned IDs are incremented sequentially starting from 1 (lastID).
Updates the EEPROM with the new IDs and ensures the changes are committed.
Example Behavior:

Module 1 has no ID stored (value = 0). Assigns ID 1.
Module 2 has ID 55. Keeps it as-is.
Module 3 has an invalid ID (255). Assigns ID 2.
2. assignMaster()
Purpose:
Identifies and assigns the master BMS module based on the lowest module ID.

Functionality:

Reads all stored IDs from the EEPROM.
Determines the module with the smallest ID.
Logs the ID of the master module.
Use Case:

The master module acts as the coordinator and manages communication with other BMS modules and external systems like the Victron Cerbo GX.
3. checkDuplicateIDs()
Purpose:
Detects and resolves duplicate IDs among BMS modules to ensure unique identification.

Functionality:

Compares the stored IDs of all modules.
If a duplicate ID is detected:
Assigns a new unique ID to the conflicting module using lastID.
Updates the EEPROM with the new ID and commits the changes.
Logs a warning whenever a duplicate ID is corrected.
Example Behavior:

Module 1 and Module 3 both have ID 5.
Module 3 is reassigned a new ID, e.g., 11.
4. setupCANCommunication()
Purpose:
Initializes the CAN communication system for data exchange between BMS modules and the master module.

Functionality:

Configures the CAN driver with default settings:
GPIO pins for CAN RX and TX.
Bit rate set to 125 Kbps.
Accepts all incoming messages (no filters applied).
Starts the CAN driver.
Use Case:

Sets up the CAN bus to facilitate communication between BMS modules and external systems like Victron Cerbo GX.
5. processIncomingCANMessages()
Purpose:
Processes incoming CAN messages received by the system.

Functionality:

Listens for messages on the CAN bus.
Logs the details of each received message:
Message ID.
Data length.
Can be expanded to handle specific message types or respond to requests.
Example Behavior:

Receives a message with ID 0x100 and length 8. Logs the information.
Summary of the New Features
Automatic ID Assignment:

Ensures all modules have unique and valid IDs.
Master-Slave Architecture:

Assigns a master module based on the smallest ID.
Duplicate ID Resolution:

Detects and resolves conflicting IDs automatically.
CAN Communication Setup:

Configures the CAN bus for seamless communication between modules.
CAN Message Processing:

Handles incoming CAN messages, enabling integration with external systems like Victron devices.
These features collectively enhance the robustness and functionality of the BMS system, ensuring reliable operation and efficient communication in a multi-module environment.









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

