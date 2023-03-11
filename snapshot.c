#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() 
{
    int get_speed= speed();
    int get_temperature =temperature();

        if (get_speed > 50 && get_temperature > 80)
        {
            // capture snapshot
             printf("snapshot taken\n");
             printf("speed=%d temperature=%d",get_speed,get_temperature);
        }
        else if (get_speed<50 && get_temperature <80)
        {
         printf("Error: could not create snapshot\n");
        }
    return 0;
}

int speed()
{
    srand(time(NULL));
    int speed_value=rand()%100+6;
    return speed_value;
}

int temperature()
{
    srand(time(NULL));
    int temperature_value=rand()%110;
    return temperature_value;
}
