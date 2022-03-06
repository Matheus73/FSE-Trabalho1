#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "modbus.h"
#include "crc16.h"
#include "uart_file.h"

// Definindo variáveis estáticas
static unsigned char device_cod = 0x01;
static unsigned char matricula[] = {6, 9, 7, 0};
#define SLEEP_TIME 100
#define TIMEOUT 5

// Criando variáveis globais
unsigned char func_cod = 0x00;
unsigned char func_sub_cod = 0x00;

void modbus_config(unsigned char cod, unsigned char sub_cod){
    int status = -1;
    while(status == -1){
        status = uart_config();
        if(status == -1){
            modbus_close();
            usleep(300000);
        }
    }

    func_cod = cod;
    func_sub_cod = sub_cod;
}

void modbus_close(){
    uart_close();
}

int modbus_receive(unsigned char *message, unsigned char size){
    int len = 0;

    for(int i = 0; i < TIMEOUT; i++){
        usleep(SLEEP_TIME * 1000);
        len = uart_read(message, size);
        if(len != 0) break;
    }

    if(len <= 0) return len;

    unsigned short crc;
    int len_crc = len - sizeof(unsigned short);
    unsigned short exp_crc = calcula_CRC(message, len_crc);
    memcpy(&crc, &message[len_crc], sizeof(unsigned short));

    if(crc != exp_crc)
        return -1;
    return len;
}

int modbus_send(unsigned char *message, unsigned char size){
    unsigned char *buffer = malloc(255);

    int count = 0;
    memcpy(&buffer[count++], &device_cod, sizeof(unsigned char));
    memcpy(&buffer[count++], &func_cod, sizeof(unsigned char));
    memcpy(&buffer[count++], &func_sub_cod, sizeof(unsigned char));

    memcpy(&buffer[count], matricula, sizeof(matricula));
    count += sizeof(matricula);

    memcpy(&buffer[count], message, size);
    count += size;

    unsigned short crc = calcula_CRC(buffer, count);

    memcpy(&buffer[count], &crc, sizeof(unsigned short));
    count += sizeof(unsigned short);

    int final_size = uart_write(buffer,count);

    free(buffer);

    return final_size;
}

