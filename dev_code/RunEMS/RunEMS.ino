#include "EMSboard.h"

//Create the board object
//The board provides interfacing to all functionality
//and includes the two EMS channels
EMSboard board;

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
  Serial.print("---> Created New Timers (Channel 1: ");
  Serial.print((1. /  (256 - board.getChannelfreq(0))) * 1000000.);
  Serial.print(", Channel 2: ");
  Serial.print((1. / (256 - board.getChannelfreq(1))) * 1000000.);
  Serial.print(")");
  freqTimer1.end();
  freqTimer2.end();
  freqTimer1.priority(10);
  freqTimer2.priority(11);

  freqTimer1.begin(frequency_action_channel1, (1. / (256 - board.getChannelfreq(0))) * 1000000.); //inverted the values here again, so that the frequency increases when turned clockwise 
  freqTimer2.begin(frequency_action_channel2, (1. / (256 - board.getChannelfreq(1))) * 1000000.); //and decreases when turned counter clockwise (+1 to prevent infinity
}

//Set up the board, and the timers, and start some very light stimulation.
//(You probably couldn't feel it.)
void setup() {
  Serial.begin(9600);
  board.init();
  create_timers();

  board.stimulate(0, 80, 120, 3);
  board.stimulate(1, 60, 140, 3);

  pinMode(A12, INPUT);



}

//This function runs all the time.
//At the moment, it looks for updates to the blue potentiometers on the board
//and the red switches. If it finds switch updates, it calls create_timers
//to update the timers.
void loop() {
  if (board.step()) create_timers();

  //Very basic implementation idea for sensor triggering - purely as a guide
  //Read a value from an analog sensor
  int val = analogRead(A12);

  //if the sensor value is between some thresholds
  if (val < 400 && val > 200)
  {
    //switch the LED on the board to help debugging
    board.switchLED(2);

    //start stimulating with some parameters
    board.stimulate(0, 80, 140, 45);

    //delay to allow some stimulation to happen
    delay(100);
  }
  else
  {
    //Stop stimulation if sensor value outside some thresholds
    board.stopStimulation(0);
  }

  delay(20);

  // Check whether any control input has been given
  // Use this to programmatically update the stimulation parameters
  // Format of updates: 'nc, a, f, p' (e.g. 1c 5 100 50 updates channel 1 to amp 5, freq 100 and pwm 50)
  if (Serial.available()) {
    int channelVal = Serial.parseInt();
    int labelChan = Serial.read();
    int commanda = Serial.parseInt();
    int commandf = Serial.parseInt();
    int commandp = Serial.parseInt();
    Serial.println("Values received");
    if (labelChan == 'c') board.setParameters(channelVal, commandf, commandp, commanda);
  }
}
