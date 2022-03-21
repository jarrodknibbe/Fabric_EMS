#include "EMSboard.h"

//Create the board object
//The board provides interfacing to all functionality
//and includes the two EMS channels
EMSboard board;

//Matt
int pinOut = A17; //Output pin of LED flash
int pinIn = A16; //Input pin checking if button is pressed
int timeStart; //Init of start of reaction time variable
int timeEnd; //Init of end of reaction time variable
int reactionTime = 0; //Init reaction time as 0

//Intervaltimers are responsible for scheduling all stimulation
//They have to sit here unfortunately due to programming challenges
IntervalTimer freqTimer1, freqTimer2, pwmTimer1, pwmTimer2;

//In total the Teensy offers 4 interval timers, we use them all.
//There are two frequency timers, that trigger the below two functions
//for example, the below functions might run at 80Hz and 60Hz, respectively,
//to trigger stimulation.
void frequency_action_channel1()
{
  pwmTimer1.priority(15);
  pwmTimer1.begin(pwm_action_channel1, board.getChannelpwm(0));
}

void frequency_action_channel2()
{
  pwmTimer2.priority(17);
  pwmTimer2.begin(pwm_action_channel2, board.getChannelpwm(1));
}

//The frequency functions, when they are triggered, then start pulse width clocks
//These clocks control the shape of the stimulation. At the moment,
//they push the signal high, then low, then off. Creating a biphasic square wave. 
//You could experiment with signal shape, but changing the response of the
//pulse step function.
void pwm_action_channel1()
{
  int result = board.pulse_step(0);

  if (result == 1) pwmTimer1.end();
}

void pwm_action_channel2()
{
  int result = board.pulse_step(1);

  if (result == 1) pwmTimer2.end();
}

//This function sets up the frequency triggers to run at the desired frequency
//This function also handles any update to frequency. 
void create_timers()
{
  Serial.println("Create timers");
  freqTimer1.end();
  freqTimer2.end();
  freqTimer1.priority(10);
  freqTimer2.priority(11);

  freqTimer1.begin(frequency_action_channel1, (1. / board.getChannelfreq(0)) * 1000000.);
  freqTimer2.begin(frequency_action_channel2, (1. / board.getChannelfreq(1)) * 1000000.);
}

//Set up the board, and the timers, and start some very light stimulation.
//(You probably couldn't feel it.)
void setup() {
  //Serial.begin(9600);
  board.init();
  //create_timers();

  board.stimulate(0,80,120,3);
  board.stimulate(1,60,140,3);

  pinMode(pinOut, OUTPUT); //Set LED pin as output
  pinMode(pinIn, INPUT); //Set button pin as input
}

//This function runs all the time.
//At the moment, it looks for updates to the blue potentiometers on the board
//and the red switches. If it finds switch updates, it calls create_timers
//to update the timers. 
void loop() {
 if (board.step()) create_timers(); 
 // Check whether any control input has been given
 // Use this to programmatically update the stimulation parameters
 // Format of updates: 'nc, a, f, p' (e.g. 1c 5 100 50 updates channel 1 to amp 5, freq 100 and pwm 50)
 if (Serial.available()){
    int channelVal = Serial.parseInt();
    int labelChan = Serial.read();
    int commanda = Serial.parseInt();
    int commandf = Serial.parseInt();
    int commandp = Serial.parseInt();

    if (labelChan == 'c') board.setParameters(channelVal, commandf, commandp, commanda);

    if (labelChan == 't'){
        board.setParameters(channelVal, commandf, commandp, commanda);
        create_timers();
    }
        
        
        
        // for (int i = 0; i <= 30; i++) {
        //  delay(4000); // Wait 3 seconds before flashing the LED (this should be changed to a random time between 1-5 seconds)
        digitalWrite(pinOut,HIGH); //After x time turn on LED
        timeStart = millis(); //Start the timer as soon as LED turns on

        delay(50); //EMS delay (theorhetically should be 205ms)
        board.stimulate(channelVal,-1,commandp,commanda);
        delay(10); // extend this pulse time.
        board.stopStimulation(channelVal);
  
        while (digitalRead(pinIn) == LOW) {
    
        }
  
        timeEnd = millis(); //'stop' the timer as soon as button is pressed
        digitalWrite(pinOut,LOW); //turn of LED
        reactionTime = timeEnd - timeStart; //record time difference to find reaction time

        Serial.print(reactionTime/1000.0, 3); //print reaction time to computer
        Serial.print(" s, This is reaction time in seconds");
        Serial.println(" ");
        timeStart = 0; 
        timeEnd = 0; //reset timer
        
       
    }
  
  delay(20);
}
