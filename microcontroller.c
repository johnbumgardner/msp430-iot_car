/*
 * Course -			ECE 306 - Introduction to Embedded Systems
 * Author - 		John Wesley Bumgardner
 * Description -  	This file contains a sample of some of the code I wrote for my embedded systems course.
 * 					Because of the nature of the course, I cannot post all of my code to GitHub. If you're
 *					interested in reviewing more of my code, I'll happily share it with you. Please email me
 *					at jwbumga2@ncsu.edu or john.bumgardner@icloud.com	
 *					The class entailed an embedded systems project which lead to a WiFi enabled car that could
 *					communicate with a TCP/IP client using serial communications. The car could receive WiFi 
 *					commands which the microprocessor (Texas Instrument MSP-430) could carry out. Several schematics
 *					regarding the control module are provided in the repository. These are not mine, but are simply
 *					attached for anyone wishing to carry out the project on their own.
 * Date - 			Fall 2018		
 */	


/* 
 * Sample Port Configuration
 * Included is my port configuration for Port 1 for my MSP430
 * Port1 contains the control for the LED's, the Analog inputs and clocks
 * Necessary macros files are missing
 */

void Init_Port_1(void)
{ 
  /*
  1	0	P1_0_LED1	GP I/O	        OUT	0	0	0	
  1	1	P1_1_LED2	GP I/O	        OUT	0	0	0	
  1	2	V_THUMB	        FUNCTION	A2	1	1	n/a	
  1	3	TEST_PROBE	GP I/O	        OUT	0	0	0	
  1	4	V_DETECT_R	FUNCTION	A4	1	1	n/a	NO
  1	5	V_DETECT_L	FUNCTION	A5	1	1	n/a	NO
  1	6	SD_MOSI	        FUNCTION	UCB0SIMO 	1	0	n/a	
  1	7	SD_MISO	        FUNCTION	UCBOSOMI 	1	0	n/a	
  */

  //configure Port 1
  P1SEL0 = LOW; //GPIO
  P1SEL1 = LOW; //GPIO
  P1DIR = LOW; //Set P1 direction as input
  
  //P1_0
  P1SEL0 &= ~RED_LED; //set as GPIO
  P1SEL1 &= ~RED_LED; //set as GPIO
  P1DIR |= RED_LED; //set as output
  P1OUT |= RED_LED; //set the LED to on
    
  //P1_1
  P1SEL0 &= ~GRN_LED; //set as GPIO
  P1SEL1 &= ~GRN_LED;
  P1DIR |= GRN_LED; //set as output
  P1OUT |= GRN_LED; //set the LED  to on
  
  //P1_2
  P1SEL0 |= V_THUMB; //set up as function with [1,1]
  P1SEL1 |= V_THUMB; 
  
  //P1_3
  P1SEL0 &= ~TESTPROBE; // set as GPIO
  P1SEL1 &= ~TESTPROBE;
  P1DIR |= TESTPROBE; //set as output
  P1OUT &= ~TESTPROBE; //turn off
  
  //P1_4
  P1SEL0 |= V_DETECT_R; //set up as function with [1,1]
  P1SEL1 |= V_DETECT_R;
  
  //P1_5
  P1SEL0 |= V_DETECT_L; //set up as function with [1,1]
  P1SEL1 |= V_DETECT_L;
  
  //P1_6
  P1SEL0 &= ~SD_UCB0SIMO; //set up as a function with [0,1]
  P1SEL1 |= SD_UCB0SIMO;
  
  //P1_7
  P1SEL0 &= ~SD_UCB0SOMI; //set up as a function with [0,1]
  P1SEL1 |= SD_UCB0SOMI;
}


/*
 * Sample serial communcation interrupt
 * Interrupt for UCA3
 * UCA3 is used to communicate with the IoT module
 * Ability to read from the IoT module is important
 * I didn't transmit anything from my board to the IoT module
 * IoT Module information: Telit/Gainspan GS2101MIP Wi-Fi module
 */


#pragma vector=USCI_A3_VECTOR 
__interrupt void USCI_A3_ISR(void)
{   
  char data_in; // variable to store received data
  switch(__even_in_range(UCA3IV, RANGE)) //determine which interrupt to use
  {     
  case NO_INT:                                     // Vector 0 - no interrupt       
    break;     
  case READ_INT:                                     // Vector 2 - to read    
    data_in = UCA3RXBUF; // read from the Rx buffer
    temp = usb_rx_ring_wr++; // increment the ring counter
    IOT_Char_Rx[usb_rx_ring_wr++] = data_in; // place the received value into an input buffer
    if(usb_rx_ring_wr >= sizeof(IOT_Char_Rx)) // reset when size is exceeded
    {
      usb_rx_ring_wr = CLEAR;
    }
    UCA0TXBUF = data_in; //send it to UCA0 which links back to the USB
  case TRANSMIT_INT:  // Vector 4 – Transmit
  break; 
  default: break;     
  } 
}

/*
 * Sample Wheel motion control
 * function to make car go forward
 * works with PWM to help control the speed
 */
void iot_forward()
{
    RIGHT_FORWARD_SPEED = MAX_RIGHT - SPEED_OFFSET;
    LEFT_FORWARD_SPEED = MAX_LEFT;
    RIGHT_REVERSE_SPEED = CLEAR;
    LEFT_REVERSE_SPEED = CLEAR;
}

/*
 * Sample function with interacting with ADC
 * Function controls the car and allows it to intercept the black line
 * When it gets to the black line, the car will stop.
 */
void intercept_black_line_project_9(void)
{
  forward_slow(); // move slowly
  if(ADC_Right_Detector < NOT_BLACK_COLOR && ADC_Left_Detector < NOT_BLACK_COLOR) //while still on white
  {
    RIGHT_FORWARD_SPEED = MAX_RIGHT;
    LEFT_FORWARD_SPEED = MAX_LEFT;
  }
  else //once black is detected
  {
    stop_motion();
    state_for_auto = FOLLOW_AUTO;
  } 
}