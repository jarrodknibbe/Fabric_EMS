#include "Arduino.h"
#include "EMSchannel.h"

void EMSchannel::init_channel(int _enable_pin, int _cur_shape_pin, int _chan_ID)
{
  ems_enable = _enable_pin;
  ems_current = _cur_shape_pin;

  channel_ID = _chan_ID;

  pinMode(ems_enable, OUTPUT);
  pinMode(ems_current, OUTPUT);

  pwm_state = 0;

  stim_active = false;
}

bool EMSchannel::startStimulation(int _freq, int _pwm, int _amp)
{
  stim_active = true;
  if (_freq != -1){
    freq = _freq;
    pulse_width = _pwm;
    amp = _amp;
    return true;
  }

  return false;
}

void EMSchannel::stopStimulation()
{
  stim_active = false;
}

int EMSchannel::get_pwm()
{
  return pulse_width;
}

int EMSchannel::get_freq()
{
  return freq;
}

int EMSchannel::get_amp()
{
  return amp;
}

int EMSchannel::stimulation_step()
{
  //At first step, write the pulse shape HIGH
  if (pwm_state == 0)
  {
    digitalWrite(ems_enable, HIGH);
    analogWrite(ems_current, (ANALOGMAX/2) + amp);
  }
  else if (pwm_state == 1) //Then write the pulse shape low
  {
    analogWrite(ems_current, (ANALOGMAX/2) - amp);
  }
  else //Then disable the EMS, (so we don't care what shape the pulse is at this point)
  {
    digitalWrite(ems_enable, LOW);
  }

  //Increment the pulse shape counter
  pwm_state++;

  //Reset when all states complete
  if (pwm_state == 3)
  {
    pwm_state = 0;
    return 1;
  }

  return 0;
}

bool EMSchannel::setStimulationParameters(int _freq, int _pwm, int _amp)
{
  freq = _freq;
  pulse_width = _pwm;
  amp = _amp;

  return true;
}

bool EMSchannel::active()
{
  return stim_active;
}
