
#include "Arduino.h"
#include "EMSboard.h"


void EMSboard::init()
{
  channels[0].init_channel(24, A22, 0);
  channels[1].init_channel(39, A21, 1);

  channels[0].setStimulationParameters(80,120,3);
  channels[1].setStimulationParameters(60,160,3);

  switchState = getSwitchState();

  for (int i = 0; i < 3; i++)
  {
    pinMode(LEDs[i], OUTPUT);
    pinMode(switches[i], INPUT_PULLUP);
    pinMode(pots[i], INPUT);
  }

  analogWriteResolution(9);
}

void EMSboard::stimulate(int _channel, int _freq, int _pwm, int _amp)
{
  if (_freq != -1)
  {
    channels[_channel].startStimulation(_freq, _pwm, _amp);
  }
}

void EMSboard::stopStimulation(int _channel)
{
  channels[_channel].stopStimulation();
}

int EMSboard::getSwitchState()
{
  int state = 0;
  for (int i = 0; i < 3; i++)
  {
    if (digitalRead(switches[i]) == HIGH){
      state += (1 << i);
    }
  }
  return state;
}

void EMSboard::useManualEMSControl(bool _use)
{
  manualEMSControl = _use;
}

void EMSboard::setParameters(int channel, int _freq, int _pwm, int _amp)
{
  channels[channel].setStimulationParameters(_freq, _pwm, _amp);
}

bool EMSboard::step()
{
  useManualEMSControl(bitRead(getSwitchState(), 0));
  
  if (manualEMSControl)
  {
    setLEDBrightness(bitRead(getSwitchState(), 1), (int)analogRead(pots[2])/4);
    
    if (bitRead(getSwitchState(), 2) != bitRead(switchState,2))
    {
      switchLED(2);
      int _freq = (int)(analogRead(pots[0])/4);
      int _pwm = (int)(analogRead(pots[1])/4);
      int _amp = (int)(analogRead(pots[2])/4);
      Serial.print(_freq);
      Serial.print(" ");
      Serial.print(_pwm);
      Serial.print(" ");
      Serial.println(_amp);
      setParameters(bitRead(getSwitchState(), 1), _freq, _pwm, _amp);
      switchState = getSwitchState();
      return true;
    }
  }

  return false;
}

int EMSboard::pulse_step(int _chan)
{
  return channels[_chan].stimulation_step();
}

void EMSboard::switchLED(int _index)
{
  digitalWrite(LEDs[_index], !digitalRead(LEDs[_index]));
}

void EMSboard::setLEDBrightness(int _index, int value)
{
  analogWrite(LEDs[_index], value);
}

int EMSboard::getChannelpwm(int _chan)
{
  return channels[_chan].get_pwm();
}

int EMSboard::getChannelfreq(int _chan)
{
  return channels[_chan].get_freq();
}

int EMSboard::getChannelamp(int _chan)
{
  return channels[_chan].get_amp();
}

