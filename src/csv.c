#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "csv.h"

#define LOG_FILE "TRAB1_LOG.csv"
#define CURVA_FILE "curva_reflow.csv"

void csv_append_data(Data data){
    FILE *fd = fopen(LOG_FILE, "a");

    time_t time_now;
    struct tm *info;
    time(&time_now);
    info = localtime(&time_now);

    fprintf(fd, "%02d,%02d,%02d,", info->tm_mday, info->tm_mon, info->tm_year%100 + 2000);
    fprintf(fd, "%02d:%02d:%02d,", info->tm_hour, info->tm_min, info->tm_sec);
    fprintf(fd, "%02.2lf,%02.2lf,%02.2lf", data.temp_int, data.temp_ext, data.temp_ref);
    fprintf(fd, "%02.2lf,%02.2lf\n", data.cooler_speed, data.resistor_power);
}

int get_data_csv(int * time, float * temp){
    FILE *fd = fopen(CURVA_FILE, "r");
    char row[1000];
    char * token;
    int count = 0;

    while(feof(fd) != 1){
        fgets(row, 1000, fd);
        token = strtok(row, ",");

        int count_token = 0;
        while(token != NULL){
            if(count_token == 0){
                time[count] = atoi(token);
            } else if (count_token == 1){
                temp[count] = atof(token);
            }
            token = strtok(NULL, ",");
            count_token++;
        }
        count++;
    }
    fclose(fd);
    return count;

}
// int main(){
//
//     int time[15];
//     float temp[15];
//
//     int size = get_data_csv(time, temp);
//
//     for(int i=0; i<size;i++){
//         printf("%d,%f\n",time[i], temp[i]);
//     }
//
//     return 0;
// }
