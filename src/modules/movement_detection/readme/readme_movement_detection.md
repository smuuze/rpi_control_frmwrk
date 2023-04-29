
### Location
[frmwrk](../../../../README.md) / [modules](../../readme_modules.md) / MOVEMENT-DETECTION

<br>
<br>

### Content

<details>
<summary> Click to open</summary>

[Brief](#brief)\
[Features](#features)\
[Solution Strategy](#solution-strategy)\
[Structure](#structure)\
[Runtime](#runtime)\
[Integration](#integration)\
&nbsp;&nbsp;&nbsp;&nbsp;[Makefile](#makefile)\
[Usage](#usage)\
&nbsp;&nbsp;&nbsp;&nbsp;[Initialization](#initialization)

</details>

<br>
<br>

# MOVEMENT-DETECTION

## Brief
[[TOP](#location)]

The movement-detection module can recognize moving objects in front of the system.
E.g. via a PIR sensor. If an object was recognized a signal is generated and send via the systems messages bus.

## Features
[[TOP](#location)]

... comming soon ...

## Requirements
[[TOP](#location)]

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
[[TOP](#location)]

This section describes how to realize each requirement.

| ID | Concept | Solution |
|----|-------|-------------|
| [REQ_MOVEMENT_01_DECT_MOVMENT] | - | - |
| [REQ_MOVEMENT_02_VERIFY_DETECTION] | - | - |
| [REQ_MOVEMENT_03_SENSOR_INDEPENDENCE] | - | - |
| [REQ_MOVEMENT_04_USE_SYSTEM_MSG_BUS] | - | - |

## Structure
[[TOP](#location)]

... comming soon ...

## Runtime
[[TOP](#location)]

... comming soon ...

## Integration
[[TOP](#location)]

### Makefile

... comming soon ...

## Usage
[[TOP](#features)]

### Initialization

... comming soon ...