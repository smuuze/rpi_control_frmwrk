[TOP]: #section "Go to the top of the page"

### Section

Readme | [Changelog](changelog.md)

### Location

frmwrk /

# RPi Control Framework

## Brief

The RPi-Control Framework contains common modules, drivers and other implementations
that are used by the RPi-Control projects. The framework provides a basic operating
system to run different task like components and a Signal-Slot mechanism to transfer
data from one module to another.

## Features

## Usage



## Main Structure

![Main_Structure](uml/img/rpi_control_frmwrk_diagram_pacakge_main_structure.svg)

| Component                                     | Description |
|-----------------------------------------------|-------------|
| Application                                   |             |
| App-Task                                      |             |
| Command-Handling                              |             |
| Driver                                        |             |
| [kernel](readme/readme_kernel.md#section)             | Contains the basic operating system and all of it's sub-components           |
| [modules](src/modules/readme_modules.md#section)  | Sub-Modules realizing special functions, e.g. LCD-Controller, Logger, File-Controller  |
| Protocol                                      |             |
