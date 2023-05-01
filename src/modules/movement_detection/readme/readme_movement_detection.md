[TOP]: #location "Go to the top of the page"

[Signal-Slot-Interface]: ../../../../readme/readme_signal_slot.md "SW-IRQ based communication system used in the rpi-control firmware"

### Section

Readme | [Changelog](../../../../changelog.md)

### Location
[frmwrk](../../../../README.md) / [modules](../../readme_modules.md) / MOVEMENT-DETECTION

<br>

### Content

<details>
<summary> Click to open</summary>

[Brief](#brief)\
[Features](#features)\
[Solution Strategy](#solution-strategy)\
[Structure](#structure)\
&nbsp;&nbsp;&nbsp;&nbsp;[Context](#context)\
[Runtime](#runtime)\
&nbsp;&nbsp;&nbsp;&nbsp;[State-Machine](#state-machine)\
[Integration](#integration)\
&nbsp;&nbsp;&nbsp;&nbsp;[Makefile](#makefile)\
[Usage](#usage)\
&nbsp;&nbsp;&nbsp;&nbsp;[Initialization](#initialization)

</details>

<br>

# MOVEMENT-DETECTION

## Brief
[[TOP]]

The movement-detection module can recognize moving objects in front of the system.
E.g. via a PIR sensor. If an object was recognized a signal is generated and send via the systems messages bus.

## Features
[[TOP]]

... comming soon ...

## Requirements
[[TOP]]

[REQ_MOVEMENT_01_DECT_MOVMENT]: #Requirements "Requirement that movement-detection needs to be fulfilled"
[REQ_MOVEMENT_02_VERIFY_DETECTION]: #REQ_MOVEMENT_02_VERIFY_DETECTION "Requirement that verify of detected movement is necessary"
[REQ_MOVEMENT_03_SENSOR_INDEPENDENCE]: #REQ_MOVEMENT_03_SENSOR_INDEPENDENCE "Requirement that movement-detection is independent of the HW-sensor"
[REQ_MOVEMENT_04_USE_SYSTEM_MSG_BUS]: #REQ_MOVEMENT_04_USE_SYSTEM_MSG_BUS "Requirement that the current system-wide message bus is used for sending signals and information"

| ID | Title | Description | *Status |
|----|-------|-------------|--------|
| [REQ_MOVEMENT_01_DECT_MOVMENT] | Detect Movement | The movement-detection module must detect movement in front of the system.  | DEFINED |
| [REQ_MOVEMENT_02_VERIFY_DETECTION] | Verify-Detection  | If a movement was detected the module shall check if the movement is still present after a short amount of time. (50ms <= t <= 1 sek). It is possible that there occur false positives. E.g. temperature variations. | DEFINED |
| [REQ_MOVEMENT_03_SENSOR_INDEPENDENCE] | Independend from sensor | The movement-detection mudle must not depend on a specific sensor-model. The movement-detection module shall not be modified if the sensor-model has changed. | DEFINED |
| [REQ_MOVEMENT_04_USE_SYSTEM_MSG_BUS] | Use system msg-bus | In case a movement was detected and verified a signal shall be send via the system message-bus. The information in that message is formated as a JSON string. The Fields of that string are defined by the movment-detection module. The data may include the following information. Timestamp, Location | DEFINED |

****Status***: the following states apply on the status field
- **DEFINED** - The requirement has been defined only.
- **CONCEPT** - There is a concept available how to realize the requirement
- **IMPLEMENTED** - The requirement has been implemented. There is a test-system available
- **VERIFIED** - The funcitonality of the reuirement has been verified. E.g. there is a unittest available and the feature was tested over a long period on the test-system.

## Solution Strategy
[[TOP]]

This section describes how to realize each requirement.

| ID | Concept | Solution |
|----|-------|-------------|
| [REQ_MOVEMENT_01_DECT_MOVMENT] | The movement-detection sensor is accessed via a interface to check if there was a movement in front of the system | - |
| [REQ_MOVEMENT_02_VERIFY_DETECTION] | The movement-detection module is implemented as a state-machine if there is a movement event in the idle state the state machine will go into a verify state. If there is one more movement event, the movement has been verified and a signal is send | - |
| [REQ_MOVEMENT_03_SENSOR_INDEPENDENCE] | The sensor-HW is accessed via a interface. There is no direction communication between the movement-detection module and the sensor-HW. The interface is defined by the movement-detection module and implemetned by the sensor driver. | - |
| [REQ_MOVEMENT_04_USE_SYSTEM_MSG_BUS] | The [Signal-Slot-Interface] is used to gneereate system-signals | - |

## Structure
[[TOP]]

### Context

![structure_context](../../../modules/movement_detection/uml/img/movement_detection_context.svg )

## Runtime
[[TOP]]

### State-Machine

![runteim_statemachine](../../../modules/movement_detection/uml/img/movement_detection_state_machine.svg )

| State     | Description |
|-----------|-------------|
| SETUP     | Activate the movement-dectection sensor. |
| WAIT      | Wait for a movement by checking the sensor periodically. If a movement was detected a timeout is started on elaving this state |
| VERIFY    | Check once again if the first event can be aproved. Go back to the IDLE state if the timeout triggers. |
| SIGNALING | A movement was detected and verified. A signal is send via the msg-bus system.  |
| PAUSE     | Wait for a time-interval before start checking fo the next movement. The sensor is deactivated in this state  |

## Integration
[[TOP]]

### Makefile

... comming soon ...

## Usage
[[TOP]]

### Initialization

... comming soon ...