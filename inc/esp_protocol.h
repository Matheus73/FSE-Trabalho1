#ifndef ESP_PROTOCOL_
#define ESP_PROTOCOL_

float get_internal_temp();
float get_potenciometro_temp();
int get_user_comand();
void send_int_control(int power);
void send_float_control(float control_signal);
void send_system_mode(int control_signal);
void send_system_status(int control_signal);

#endif
