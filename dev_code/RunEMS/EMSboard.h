#include "EMSchannel.h"
#include "Arduino.h"

class EMSboard {
  private:
    //Create two EMS channel objects for our board
    EMSchannel channels[2];

    //Set up all other functionality for the board, including the pins
    int LEDs[3] = {2,3,4};
    int switches[3]= {6,7,8};
    int pots[3]= {A9, A8, A7};

    //*************
    //If you wish to use other pins for things, you could define them here
    //For example, making other analog or digital pins available for things
    //*************

    //Store the switch state
    //The switch state is stored as an int, constructed from its 3 bits of information
    //Read back from the switch state by looking at individual bits with bitRead(n, bit_n)
    int switchState = 0;

    //Flag for whether the manual EMS controls are available through the potis
    bool manualEMSControl = false;

  public:
    //Initialise the board
    void init();

    //Set stimulation parameters and stimulate (use _freq = -1 to not change parameters)
    void stimulate(int _channel, int _freq, int _pwm, int _amp);

    //Just set the stimulation parameters, but don't stim
    void setParameters(int _channel, int _freq, int _pwm, int _amp);

    //Stop stimulation
    void stopStimulation(int _channel);

    //Get state of 3pin switch
    int getSwitchState();

    //Turn off/on potentiometers for setting EMS parameters
    void useManualEMSControl(bool _use);

    //Switch an on-board LED
    void switchLED(int _index);
    //Set the brightness of an on-board LED
    void setLEDBrightness(int _index, int value);

    //The board's 'loop' function, call in standard 'loop'
    bool step();

    //Handle the pulse width steps for stimulation
    //Effectively just calls each channels pulse step function
    int pulse_step(int _chan);

    //Access channels stimulation parameters
    int getChannelpwm(int _chan);
    int getChannelfreq(int _chan);
    int getChannelamp(int _chan);
};


