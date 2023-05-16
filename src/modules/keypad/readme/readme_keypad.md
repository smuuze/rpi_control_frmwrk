[TOP]: #section "Go to the top of the page"

### Section

Readme | [Changelog](../../../../changelog.md)

### Location
[frmwrk](../../../../README.md) / [modules](../../readme_modules.md) / KEYPAD

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
&nbsp;&nbsp;&nbsp;&nbsp;[Signals](#signals)\
&nbsp;&nbsp;&nbsp;&nbsp;[Configuration Macros](#configuration-macros)\
[Integration](#integration)\
&nbsp;&nbsp;&nbsp;&nbsp;[Makefile](#makefile)\
[Usage](#usage)\
&nbsp;&nbsp;&nbsp;&nbsp;[Initialization](#initialization)

</details>

<br>

# LCD

## Brief
[[TOP]]

Interface to handle a keypad.

## Features
[[TOP]]

... comming soon ...

## Requirements
[[TOP]]

[REQ_KEYPAD_01_USE_SYSTEM_MSG_BUS]: #REQ_KEYPAD_01_USE_SYSTEM_MSG_BUS "Requirement that the current system-wide message bus is used for sending signals and information"
[REQ_KEYPAD_02_KEY_STATES]: #REQ_KEYPAD_02_KEY_STATES "Requirement that a key can have one of three states RELEASED / PRESSED / RELEASED"
[REQ_KEYPAD_03_STATE_EVENTS]: #REQ_KEYPAD_03_STATE_EVENTS "Requirement that there is a event generate in case a key has changed its state."
[REQ_KEYPAD_04_KEYPAD_INDEPENDENCE]: #REQ_KEYPAD_04_KEYPAD_INDEPENDENCE "Requirement that keypad controller does not depend on a specific type of keypad-hw"

| ID | Title | Description | *Status |
|----|-------|-------------|---------|
| [REQ_KEYPAD_01_USE_SYSTEM_MSG_BUS] | Use system msg-bus | - | DEFINED |
| [REQ_KEYPAD_02_KEY_STATES] | A key can have have different states | Every key can have one of the following states. A key can always be in only one state at a time.<ul><li>RELEASE - key is not used at this moment</li><li>PRESSED - the key was just pressed, this state is left automatically</li><li>DOWN - The key is hold down by the user.</li></ul> | DEFINED |
| [REQ_KEYPAD_03_STATE_EVENTS] | Generate key state events. | In case the state of a key is changed (see the list below) a event is generated transmitted via the system msg-bus. The events are generated for each key individually. <br><ul><li>RELEASED -> PRESSED</li><li>PRESSED -> DOWN</li><li>DOWN -> RELEASED</li></ul> | DEFINED |
| [REQ_KEYPAD_04_KEYPAD_INDEPENDENCE] | Independence of Keypad-HW | The keypad controller doesn not depends on a specific type of keypad. The HW can be changed without changing the keypad-controller | DEFINED |

****Status***: the following states apply on the status field
- **DEFINED** - The requirement has been defined only.
- **CONCEPT** - There is a concept available how to realize the requirement
- **IMPLEMENTED** - The requirement has been implemented. There is a test-system available
- **VERIFIED** - The funcitonality of the reuirement has been verified. E.g. there is a unittest available and the feature was tested over a long period on the test-system.

## Solution Strategy
[[TOP]]

This section describes how to realize each requirement.

| ID | Concept | Solution |
|----|---------|-------------|
| [REQ_KEYPAD_01_USE_SYSTEM_MSG_BUS] | - | - |

## Structure
[[TOP]]

### Context

![structure_context](../../../modules/movement_detection/uml/img/movement_detection_context.svg )

## Runtime
[[TOP]]

### State-Machine

![runteim_statemachine](../../../modules/movement_detection/uml/img/movement_detection_state_machine.svg )

| State              | Description |
|--------------------|-------------|
| SETUP              | - |

## Interface
[[TOP]]

### Signals

| Signal-Name                         | Direction | Arguments | Description |
|-------------------------------------|-----------|-----------|-------------|
| `MOVEMENT_DETECT_SIGNAL`            | -      | -      | - |

### Configuration Macros

The following values can be defined as a macro. E.g. in your project specific `config.h`\
See [modules/movement_detection/movement_detection_controller.h](../../../modules/movement_detection/movement_detection_controller.h)

| Configuration Macro                                       | Default Value | Description                 |
|-----------------------------------------------------------|---------------|-----------------------------|
| `MOVEMENT_DETECTION_CONTROLLER_SCHEDULE_INTERVAL_MS`      | -           | - |

## Integration
[[TOP]]

### Makefile

The KEYPAD-controller is integrated into a project by the Makefile. Depending on the type of KEYPAD to use
the following line is added to the MODULE configuration. There is only support for a KEYAD with 7 pins. Three COLUMN pins are used as
outputs to set a high level for detection. CUrrently pressed keys are read via the four ROW pins.

```Makefile
USER_INTERFACE_CFG += KEYPAD_3X4
```

## Usage

To use the keypad-controller the header file needs to be included. See below

```C
#include "modules/keypad/keypad_interface.h"
```

### Initialization

Befor the LCD can be used it needs to be initialized and enabled. See below.

```C
keypad_init();
```

### Connect Key Slots

Keys are handled via Signals. If a key is pressed the 'PRESSED' signal of this key is send.
If more than one key is pressed at a time. A seperate signal for every pressed key is send.
To handle a key a slot mus be connected to its pressed-signal. The following tyble gives an
overview about the available signals.

| Signal        | Description                  |
|---------------|------------------------------|
| KEY_0_PRESSED_SIGNAL | Key 0 is currently pressed   |
| KEY_1_PRESSED_SIGNAL | Key 1 is currently pressed   |
| KEY_2_PRESSED_SIGNAL | Key 2 is currently pressed   |
| KEY_3_PRESSED_SIGNAL | Key 3 is currently pressed   |
| KEY_4_PRESSED_SIGNAL | Key 4 is currently pressed   |
| KEY_5_PRESSED_SIGNAL | Key 5 is currently pressed   |
| KEY_6_PRESSED_SIGNAL | Key 6 is currently pressed   |
| KEY_7_PRESSED_SIGNAL | Key 7 is currently pressed   |
| KEY_8_PRESSED_SIGNAL | Key 8 is currently pressed   |
| KEY_9_PRESSED_SIGNAL | Key 9 is currently pressed   |
| KEY_SPECIAL_1_PRESSED_SIGNAL | Special Key 1 is currently pressed   |
| KEY_SPECIAL_2_PRESSED_SIGNAL | Special Key 2 is currently pressed   |

The following example shows how to create a handler for KEY_0.

```C
void MY_KEY_0_PRESSED_SIGNAL_HANDLER(void) {
    // do something
}

...

SIGNAL_SLOT_INTERFACE_CREATE_SLOT(KEY_0_PRESSED_SIGNAL, MY_KES_0_PRESSED_SIGNAL_SLOT, MY_KEY_0_PRESSED_SIGNAL_HANDLER)
```

## Scheduling

## Structure
[[TOP]]

![Structure](../../../modules/lcd/uml/img/rpi_control_frmwrk_diagram_component_lcd_controller.svg )

## Runtime
[[TOP]]

![Runtime](../../../modules/lcd/uml/img/rpi_control_frmwrk_diagram_sequence_lcd_controller.svg )

