#include <stdio.h>
#include <string.h>

#include "esp_protocol.h"
#include "modbus.h"

#define GET_DATA 0x23
#define SEND_DATA 0x16

float get_internal_temp(){
    modbus_config(GET_DATA, 0xc1);
    modbus_send(NULL, 0);
    unsigned char message[255];

    int size = modbus_receive(message, 254);
    size += 0;
    float temp = *(float *) &message[3];

    modbus_close();
    return temp;
}

float get_potenciometro_temp(){

    modbus_config(GET_DATA, 0xc2);
    modbus_send(NULL, 0);
    unsigned char message[255];

    int size = modbus_receive(message, 254);
    size += 0;
    float temp = *(float *) &message[3];

    modbus_close();
    return temp;
}

int get_user_comand(){

    modbus_config(GET_DATA, 0xc3);
    modbus_send(NULL, 0);
    unsigned char message[255];

    int size = modbus_receive(message, 254);
    size += 0;
    if(message[3] == 0x01){
        printf("COMANDO RECEBIDO: LIGAR\n");
        modbus_close();

        send_system_status(1);
        return 1;
    }
    else if(message[3] == 0x02){
        printf("COMANDO RECEBIDO: DESLIGAR\n");
        modbus_close();
        send_system_status(0);
        return 2;
    }
    else if(message[3] == 0x03){
        printf("COMANDO RECEBIDO: POTENCIOMETRO\n");
        modbus_close();
        send_system_mode(0);
        return 3;
    }
    else if(message[3] == 0x04){
        printf("COMANDO RECEBIDO: CURVA\n");
        modbus_close();
        send_system_mode(1);
        return 4;
    }
    return 0;
}

void send_int_control(int control_signal){

    modbus_config(SEND_DATA, 0xd1);
    unsigned char message[255];
    memcpy(message, &control_signal, sizeof(int));

    modbus_send(message, 4);
    modbus_close();
}

void send_float_control(float control_signal){

    modbus_config(SEND_DATA, 0xd2);
    unsigned char message[255];
    memcpy(message, &control_signal, sizeof(float));

    modbus_send(message, 4);
    modbus_close();
}

void send_system_status(int control_signal){

    modbus_config(SEND_DATA, 0xd3);
    unsigned char message[255];
    memcpy(message, &control_signal, sizeof(int));

    modbus_send(message, 1);
    int size = modbus_receive(message, 254);
    if(size == -1){
        printf("CRC RECEBIDO INVALIDO\n");
    }

    modbus_close();
}

void send_system_mode(int control_signal){

    modbus_config(SEND_DATA, 0xd4);
    unsigned char message[255];
    memcpy(message, &control_signal, sizeof(int));

    modbus_send(message, 1);
    modbus_close();
}
