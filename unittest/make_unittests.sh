#!/bin/bash


start_time=$(date +%s)
now=$(date +"%m_%d_%Y")
time=$(date +"%H:%M:%S")
log_file=../log_file.txt

printf "========================================================\n\n" #> $log_file
printf "Starting UNITTESTS - Time: %s\n\n" $time #>> $log_file

source_dir_list=(
    cfg_file_parser
    cli_argument
    cli_executer
    cli_handler_gpio
    cli_remote_control
    gpio_driver_atmega1284p
    ir_remote_jvc
    ir_remote_led_lights
    ir_remote_samsung
    ir_remote_sony
    json_parser
    log_interface
    math_module
    mqtt_interface
    msg_executer
    parse_trace_object
    power_management
    print_trace_object
    qeue_interface
    read_trace_object
    rpi_protocol_client
    rpi_protocol_host
    signal_slot_interface
    #pca9670_driver_test
    #rpi_protocol_handler_test
    #power_management_test
    #gpio_driver_test
    #timer
    #timer_test
)

for i in "${!source_dir_list[@]}"; do

        source_dir=${source_dir_list[$i]}
    time=$(date +"%H:%M:%S")

        printf "\n\n%s\n" "---------------------------------------------------------------" #>> $log_file
        printf " - TIME: $time\n" #>> $log_file
        printf " - SOURCE: $source_dir\n" #>> $log_file

        # check if source is available
        if ls | grep $source_dir > /dev/null; then
                printf "%s\n" " - AVAILABLE" #>> $log_file
        else
                printf "%s\n" " - NOT AVAILABLE !!!" #>> $log_file
                continue
        fi

        # check if src-dir is empty to prevent rsync from delete data within backup-dir
        if [ -z "$(ls -A $source_dir)" ]; then
                printf "%s\n" "- EMPTY !!" #>> $log_file
                continue
        else
                printf "%s\n" " - NOT EMPTY" #>> $log_file
        fi

        printf "%s\n" " - STARTING UNITTEST:" #>> $log_file

    cd $source_dir
    if make clean all run; then
        printf "%s\n" "- SUCCESS";
    else
        printf "%s\n" "- FAILED !!!";
        exit 1;
    fi

    printf "%s\n" " - CLEANING UP"
    make clean

    printf "\n\n"
    cd ..
done
