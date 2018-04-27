#include "IntervalTimer.h"
#define ANALOGMAX 512

//This is a class for handling all EMS channel details
class EMSchannel {
  private:
    //Set control pins
    int ems_enable;
    int ems_current;

    //Create parameter variables
    int freq;
    int pulse_width;
    int amp;

    //Store whether signal high, low, or neutral
    int pwm_state;

    //Redundant - lets the channel know which channel it is.
    int channel_ID;

    //Is the stimulation currently running?
    bool stim_active;

  public:
    //Set up stimulation channels with pins
    void init_channel(int _en, int _cur, int _chan_ID);

    //Start stim, with stim parameters
    bool startStimulation(int _freq, int _pwm, int _amp);

    //Stop stim, reset parameters
    void stopStimulation();

    //Just update the stimulation parameters, without triggering stimulation
    bool setStimulationParameters(int _freq, int _pwm, int _amp);

    //Access functions for stimulation variables
    int get_pwm();
    int get_freq();
    int get_amp();

    //Handle the pulse shape per channel at each pulse tick
    int stimulation_step();

    //Access function for stimulation_active flag
    bool active();

    //Check whether change is sufficient to update values (filter for some noise)
    bool check_change(int _freq, int _pwm, int _amp, int _threshold);
};
