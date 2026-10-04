/*
 * File:   main.c
 * Author: Admin
 *
 * Created on 28 September, 2026, 8:03 PM
 */
#pragma config OSC = HS      // match your board's crystal
#pragma config WDT = OFF     // watchdog off, otherwise the chip keeps resetting
#pragma config LVP = OFF
#pragma config PBADEN = OFF  // PORTB pins digital (also helps your LEDs)

#include <xc.h>
#include"adc.h"
#include"digital_keypad.h"
#include"ssd_display.h"
#include<string.h>
#include"can.h"
#include"msg_id.h"
#include"clcd.h"
#include"matrix_keypad.h"
static void init_config(void)
{
    init_digital_keypad();
    init_adc();
    //init_clcd();
    init_matrix_keypad();
    init_ssd_control();
    init_can(); // we are making rb3 as i/p so.
    TRISB0 = 0;// maked port0,1 and 6,7 as output for indicater
    TRISB1 = 0;
    TRISB6 = 0;
    TRISB7 = 0;
    RB0 = 0;
    RB1 = 0;
    RB6 = 0;
    RB7 = 0;
    
}

void main(void)
{
    init_config();
    //rpm  start
    static unsigned char ssd[MAX_SSD_CNT];
    static unsigned char digit[] = {ZERO, ONE,TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE};
    
    unsigned int rpm;
    unsigned char key;
    unsigned char flag = {0};      // 0 = off, 1 = left indicator, 2 = right indicator
    unsigned int delay = 0;
    unsigned int rx_msg_id = 0, rx_len = 0;
    unsigned char rx_data[4];
    unsigned char rpm_rx_data[4] = {0};     // last received RPM

    //unsigned char rpm_rx_data[4];// = {ZERO, ZERO, ZERO, ZERO};
   
    //clcd_print("RPM",LINE1(0));
   // RB0 = 1,RB0 = 1,RB6 = 1,RB7 = 1; to check the led
    
   
    while(1)
    {
        rpm = (unsigned long)read_adc(CHANNEL4) * 6000/1023;
        ssd[3] = digit[rpm%10];
        ssd[2] = digit[(rpm/10)%10 ];
        ssd[1] = digit[(rpm/100)%10];
        ssd[0] = digit[(rpm/1000)%10];
        
        //display(ssd);
        
        can_transmit(RPM_MSG_ID, ssd, 4);
        __delay_ms(80);

       
        rx_msg_id = 0;
        can_receive(&rx_msg_id, rx_data, &rx_len);
        if ((rx_msg_id == RPM_MSG_ID) && (rx_len == 4))
        {
            strcpy(rpm_rx_data, rx_data);
        }
        display(rpm_rx_data);
        
        //clcd_print(rx_data,LINE2(0));  // can recive need \0 bcz clcd we not mention the lenghth so by \0 it will identify end
        //rpm end
        
        //indicater start
        key = read_digital_keypad(STATE_CHANGE);
        if(key == SWITCH1)
        {
            flag = 1;// left indicater 6 and 7
            RB0 = 0;
            RB1 = 0;
            delay = 0;
        }
        else if(key == SWITCH2)
        {
            flag = 2;
            RB6 = 0;
            RB7 = 0;
            delay = 0;
        }
        else if(key == SWITCH3)
        {
            flag = 0;
            RB1 = 0,RB0 = 0,RB6 = 0,RB7 = 0;
        }
        can_transmit(INDICATOR_MSG_ID,&flag,1);
        can_receive(&rx_msg_id,rx_data,&rx_len);
        if(rx_data[0] == 1)
        {
            if(delay++ <= 10)
            {
                RB6 = 1;
                RB7 = 1;
                RB0 = 0;
                RB1 = 0;
            }
            else if(delay <= 20)
            {
                RB6 = 0;
                RB7 = 0;
                RB0 = 0;
                RB1 = 0;
            }
            else
                delay = 0;
        }
        else if(rx_data[0] == 2)
        {
            if(delay++ <= 10)
            {
                RB0 = 1;
                RB1 = 1;
                RB6 = 0;
                RB7 = 0;
            }
            else if(delay <= 20)
            {
                RB0 = 0;
                RB1 = 0;
                RB6 = 0;
                RB7 = 0;
            }
            else
                delay = 0; 
        }
        else if(rx_data[0] == 0)
        {
            RB1 = 0,RB0 = 0,RB6 = 0,RB7 = 0;
        }
        // indicater ebd
        
    }
    return;
}
