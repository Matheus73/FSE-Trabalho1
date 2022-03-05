#ifndef CSV_
#define CSV_

typedef struct data{
    float temp_int;
    float temp_ext;
    float temp_ref;
    float cooler_speed;
    float resistor_power;
} Data;

void csv_append_data(Data data);
int get_data_csv(int * time, float * temp);

#endif
