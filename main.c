#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <wiringPi.h>
#include <softPwm.h>

#define RESISTOR 4
#define COOLER 5

#include "inc/csv.h"
#include "inc/esp_protocol.h"
#include "inc/lcd_driver.h"
#include "inc/bme280_driver.h"
#include "inc/temperature_controller.h"
#include "inc/pid.h"

float temp_passada = 25;

void final_config(){
    close_temp();
    lcd_init();
    ClrLcd();
    lcdLoc(LINE1);
    send_int_control(0);
    typeln("FINALIZADO");
    exit(0);
}

void potenciometro_rotina();
void curva_rotina();

void terminal_rotina(float temp_pot){
    int comand = 0;
    float temp_passada;
    while(comand == 0){
        usleep(500000);

        double temp_ext;
        int result = get_temp(&temp_ext);
        result += 0;
        // if(result == -1) return;

        pid_atualiza_referencia(temp_pot);

        float temp_int = get_internal_temp();

        if(temp_int < 0 || temp_int > 100){
            temp_int = temp_passada;
        }
        temp_passada = temp_int;

        float power = pid_controle(temp_int);
        float power_resistor, power_cooler;
        if(power < 0){
            power_cooler = power * -1;
            if(power_cooler < 40) power_cooler = 40;
            power_resistor = 0;
        } else {
            power_cooler = 0;
            power_resistor = power;
        }

        send_int_control(power);
        send_float_control(temp_pot);

        lcd_init();
        ClrLcd();
        lcdLoc(LINE1);
        typeln("TI");
        typeFloat(temp_int);
        typeln(" TE");
        typeFloat(temp_ext);
        lcdLoc(LINE2);
        typeln("TR");
        typeFloat(temp_pot);
        typeln(" TERM.");
        
        printf("TI: %.2f TE: %.2f TR: %.2f\n",temp_int, temp_ext, temp_pot);
        Data data = {temp_int, temp_ext, temp_pot, power_cooler, power_resistor};
        csv_append_data(data);

        
        change_temperature(power);
        comand = get_user_comand();
    }
    if(comand == 2){
        final_config();
    }
    if(comand == 3){
        potenciometro_rotina();
    }
    if(comand == 4){
        curva_rotina();
    }
}
void potenciometro_rotina(){
    int comand = 0;
    while(comand == 0 || comand == 3){
        usleep(500000);
        float temp_pot = get_potenciometro_temp();
        double temp_ext;

        int result = get_temp(&temp_ext);
        result += 0;
        // if(result == -1) return;

        pid_atualiza_referencia(temp_pot);

        float temp_int = get_internal_temp();

        if(temp_int < 0 || temp_int > 100){
            temp_int = temp_passada;
        }
        temp_passada = temp_int;

        float power = pid_controle(temp_int);
        float power_resistor, power_cooler;
        if(power < 0){
            power_cooler = power * -1;
            if(power_cooler < 40) power_cooler = 40;
            power_resistor = 0;
        } else {
            power_cooler = 0;
            power_resistor = power;
        }

        send_int_control(power);

        lcd_init();
        ClrLcd();
        lcdLoc(LINE1);
        typeln("TI");
        typeFloat(temp_int);
        typeln(" TE");
        typeFloat(temp_ext);
        lcdLoc(LINE2);
        typeln("TR");
        typeFloat(temp_pot);
        typeln(" POTEN.");
        
        printf("TI: %.2f TE: %.2f TR: %.2f\n",temp_int, temp_ext, temp_pot);
        Data data = {temp_int, temp_ext, temp_pot, power_cooler, power_resistor};
        csv_append_data(data);

        
        change_temperature(power);
        comand = get_user_comand();
    }
    if(comand == 2){
        final_config();
    }
    if(comand == 4){
        curva_rotina();
    }
}

void curva_rotina(){

    int time[600];
    float temp[600];

    int size = get_data_csv(time, temp);

    int count = 0;
    float atual_temp = temp[0];
    int next_time = time[1];
    int time_count = 1;

    int comand = 0;
    while(comand == 0 || comand == 4){
        usleep(500000);

        if(count == next_time){
            if(time_count == size){
                comand = 2;
            }
            atual_temp = temp[time_count];
            time_count++;
            next_time = time[time_count];
        }
        double temp_ext;
        int result = get_temp(&temp_ext);
        result += 0;

        pid_atualiza_referencia(atual_temp);
        float temp_int = get_internal_temp();

        if(temp_int < 0 || temp_int > 100){
            temp_int = temp_passada;
        }
        temp_passada = temp_int;

        float power = pid_controle(temp_int);

        float power_resistor, power_cooler;
        if(power < 0){
            power_cooler = power * -1;
            if(power_cooler < 40) power_cooler = 40;
            power_resistor = 0;
        } else {
            power_cooler = 0;
            power_resistor = power;
        }

        send_int_control(power);
        send_float_control(atual_temp);

        lcd_init();
        ClrLcd();
        lcdLoc(LINE1);
        typeln("TI");
        typeFloat(temp_int);
        typeln(" TE");
        typeFloat(temp_ext);
        lcdLoc(LINE2);
        typeln("TR");
        typeFloat(atual_temp);
        typeln(" CURVA");
        
        printf("TI: %.2f TE: %.2f TR: %.2f\n",temp_int, temp_ext, atual_temp);
        Data data = {temp_int, temp_ext, atual_temp, power_cooler, power_resistor};
        csv_append_data(data);

        change_temperature(power);


        count++;

        comand = get_user_comand();
    }
    if(comand == 2){
        final_config();
    }
    if(comand == 3){
        potenciometro_rotina();
    }
    if(comand == 4){
        curva_rotina();
    }
}

int main(){
    signal(SIGINT, final_config);
    printf("--------\n");
    printf("| MENU |\n");
    printf("--------\n");
    printf("1. Rasp42\n");
    printf("2. Rasp43\n");
    printf("3. Definir constantes manualmente\n");

    int opt;
    scanf("%d", &opt);

    float kp, ki, kd;
    switch(opt){
        case 1:
            kp = 30.0;
            ki = 0.2;
            kd = 400.0;
            break;
        case 2:
            kp = 20.0;
            ki = 0.1;
            kd = 100.0;
            break;
        case 3:
            printf("kp: ");
            scanf("%f", &kp);

            printf("ki: ");
            scanf("%f", &ki);

            printf("kd: ");
            scanf("%f", &kd);
            break;
        default:
            printf("OPCAO INVALIDA!\n");
            return -1;
    }

    init_bme();
    pid_configura_constantes(kp,ki,kd);
    config_temperature();

    int opt_term = 0;

    printf("GOSTARIA DE USAR O MODO TERMINAL OU O MODO UART?\n");
    printf("1. TERMINAL\n");
    printf("2. UART\n");

    scanf("%d", &opt_term);

    if(opt_term == 1){
        printf("ENTRANDO EM MODO TERMINAL\n");
        printf("Digite a temperatura que deseja: ");
        float temp_ter;
        scanf("%f",&temp_ter);
        terminal_rotina(temp_ter);

    } else if (opt_term == 2){

        printf("ENTRANDO EM MODO UART\n");

        printf("para finaliza o programa digite: CTRL + C\n");
        printf("Ou mande o sinal de desligar via dashboard.\n");

        int count = 0;
        printf("AGUARDANDO COMANDO\n");
        while(count < 240){
            sleep(1);
            count++;
            // printf("COUNT - %d\n", count);

            int comand = get_user_comand();
            switch(comand){
                case 1:
                    break;
                case 2:
                    final_config();
                    break;
                case 3:
                    potenciometro_rotina();
                    break;
                case 4:
                    curva_rotina();
                    break;
            }
        }
    } else {
        printf("ESCOLHA INVALIDA!\n");
    }


    return 0;

}
