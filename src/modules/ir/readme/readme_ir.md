
### Location
[frmwrk](../../../../README.md) / [modules](../../readme_modules.md) / IR

# IR

## Brief
[[TOP](#location)]



## Integration
[[TOP](#location)]

### Makefile


```Makefile
USER_INTERFACE_CFG += LCD_16X2
```

## Usage

To use the LCD-controller the header file needs to be included. See below

```C
#include "modules/ir/ir_protocol_task.h"
```

### Initialization

Befor the LCD can be used it needs to be initialized and enabled. See below.

```C
ir_protocol_init();
```

## Add Protocol

## Structure
[[TOP](#location)]

### IR Protocol

![Structure](../../../modules/ir/uml/img/rpi_control_frmwrk_diagram_component_ir_protocol.svg )

### Command Handling

![Structure](../../../modules/ir/uml/img/rpi_control_frmwrk_diagrm_component_ir_protocol_cmd_handler.svg )

## Runtime
[[TOP](#location)]

![Runtime](../../../modules/ir/uml/img/rpi_control_frmwrk_uml_runtime_sequence_ir_protocol_interface.svg )