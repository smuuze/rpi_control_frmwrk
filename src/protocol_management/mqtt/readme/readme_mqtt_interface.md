[TOP]: #section "Go to the top of the page"

[Signal-Slot-Interface]: ../../../../readme/readme_signal_slot.md "SW-IRQ based communication system used in the rpi-control firmware"
[Movement-Detection-Drivers]: ../../../driver/movement_detection/readme/readme_movement_detection.md#section "List of drivers to be used by the movement-detection controller"

### Section

Readme | [Changelog](../../../../changelog.md)

### Location
[frmwrk](../../../../README.md) / [protocol](../../readme_protocol.md) / MQTT-INTERFACE

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
[Interface](#interface)\
&nbsp;&nbsp;&nbsp;&nbsp;[Sensor Interface](#sensor-interface)\
&nbsp;&nbsp;&nbsp;&nbsp;[Signals](#signals)\
&nbsp;&nbsp;&nbsp;&nbsp;[Configuration Macros](#configuration-macros)\
[Integration](#integration)\
&nbsp;&nbsp;&nbsp;&nbsp;[Makefile](#makefile)\
[Usage](#usage)\
&nbsp;&nbsp;&nbsp;&nbsp;[Initialization](#initialization)

</details>

<br>

# MOVEMENT-DETECTION

## Brief
[[TOP]]

... comming soon ...

## Features
[[TOP]]

... comming soon ...

## Requirements
[[TOP]]

[REQ_MQTT_INTERFACE_01]: #Requirements "First requirement"

| ID | Title | Description | *Status |
|----|-------|-------------|--------|
| [REQ_MQTT_INTERFACE_01] | Detect Movement | The movement-detection module must detect movement in front of the system.  | CONCEPT |

****Status***: the following states apply on the status field
- **DEFINED** - The requirement has been defined only.
- **CONCEPT** - There is a concept available how to realize the requirement
- **IMPLEMENTED** - The requirement has been implemented. There is a test-system available
- **VERIFIED** - The funcitonality of the reuirement has been verified. E.g. there is a unittest available and the feature was tested over a long period on the test-system.

## Solution Strategy
[[TOP]]

This section describes how to realize each requirement.

| ID | Concept | Solution |
|----|---------|----------|
| [REQ_MQTT_INTERFACE_01] | - | - |

## Structure
[[TOP]]

### Context

... comming soon ...

## Runtime
[[TOP]]

### State-Machine

... comming soon ...

| State              | Description |
|--------------------|-------------|
| Name-Of-State      | Description of the state. |

## Interface
[[TOP]]

### Sensor Interface

See [protocol/mqtt/mqtt_interface.h](../../../protocol/mqtt/mqtt_interface.h) for details.

| Function          | Description                                   |
|-------------------|-----------------------------------------------|
| Name-Of-Function  | Description of function      |

### Signals

| Signal-Name                         | Direction | Arguments | Description |
|-------------------------------------|-----------|-----------|-------------|
| `MOVEMENT_DETECT_SIGNAL`            | SEND      | none      | Is send in case there was a movement detected and verified |
| `MOVEMENT_DETECT_POWER_DOWN_SIGNAL` | RECEIVE   | none      | If this signal is received, the module goes into power down mode |
| `MOVEMENT_DETECT_POWER_UP_SIGNAL`   | RECEIVE   | none      | If this signal is received the module leaves power down mode, if entered previously |

### Configuration Macros

The following values can be defined as a macro. E.g. in your project specific `config.h`\
See [modules/movement_detection/movement_detection_controller.h](../../../modules/movement_detection/movement_detection_controller.h)

| Configuration Macro                                       | Default Value | Description                 |
|-----------------------------------------------------------|---------------|-----------------------------|
| `MOVEMENT_DETECTION_CONTROLLER_SCHEDULE_INTERVAL_MS`      | 100           | Interval at which the controller checks the sensor for detected movement |
| `MOVEMENT_DETECTION_CONTROLLER_WAIT_TO_VERIFY_TIMEOUT_MS` | 500           | Time in milliseconds to wait before verify a movement. |
| `MOVEMENT_DETECTION_CONTROLLER_VERIFY_TIMEOUT_MS`         | 1000          | Time in milliseconds to verify a movement. If this time is up, there is no movement. |
| `MOVEMENT_DETECTION_CONTROLLER_PAUSE_TIME_MS`             | 1000          | Pause interval in milliseconds after a successful movement detection. |

## Integration
[[TOP]]

### Makefile

Add the following statement to your project makefile. Remember to also select a
suitable movement-detection driver, see [Movement-Detection-Drivers].
This will define the macro `MOVEMENT_DETECTION_CONTROLLER_AVAILABLE`

```make
MODULES_CFG += MOVEMENT_DETECTION
```

## Usage
[[TOP]]

### Initialization

Add the following code block to your initialization routine.
Do not forget to include the header file.

```c
#include "modules/movement_detection/movement_detection_controller.h"
```

```c
#ifdef MOVEMENT_DETECTION_CONTROLLER_AVAILABLE
{
    movement_detection_controller_init();
}
#endif
```

The following drivers and modules need to be initialized before.
- System
    - Clock
    - RTC
    - GPIO 
    - Signal-Slot
- Task
