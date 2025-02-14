/*
 * Copyright (C) OpenTX
 *
 * Based on code named
 *   th9x - http://code.google.com/p/th9x 
 *   er9x - http://code.google.com/p/er9x
 *   gruvin9x - http://code.google.com/p/gruvin9x
 *
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _BUZZER_DRIVER_H_
#define _BUZZER_DRIVER_H_

#define BUZZER_BUFFER_DURATION  (10)
#define BUZZER_QUEUE_LENGTH (4) // must be power of 2

#define BEEP_MIN_FREQ           (250)
#define BEEP_MAX_FREQ           (14000)
#define BEEP_DEFAULT_FREQ       (2250)
#define BEEP_KEY_UP_FREQ        (BEEP_DEFAULT_FREQ+150)
#define BEEP_KEY_DOWN_FREQ      (BEEP_DEFAULT_FREQ-150)


#if defined(__cplusplus)

struct BuzzerTone {
  uint16_t freq;
  uint16_t duration;
  uint16_t pause;
  uint8_t  flags;
  int8_t   freqIncr;
  BuzzerTone() {};
  BuzzerTone(uint16_t freq, uint16_t duration, uint16_t pause, uint8_t flags, int8_t freqIncr):
    freq(freq),
    duration(duration),
    pause(pause),
    flags(flags),
    freqIncr(freqIncr)
  {};
};

struct BuzzerState {
  uint16_t freq;
  uint16_t duration; // current decremented duration
  uint16_t pause; // current pause, set to 0 after use, reset in repeat handler
  uint8_t repeat; // current decremented repeat
  uint8_t spare; // unused
  BuzzerTone tone;
  BuzzerState() {};
  BuzzerState(uint16_t freq, uint16_t duration, uint16_t pause, uint8_t repeat, BuzzerTone tone):
    freq(freq),
    duration(duration),
    pause(pause),
    repeat(repeat),
    tone(tone)
  {};
};

class BuzzerToneFifo
{
  private:
    volatile uint8_t ridx;
    volatile uint8_t widx;

    uint8_t nextIdx(uint8_t idx) const
    {
      return (idx + 1) & (BUZZER_QUEUE_LENGTH - 1);
    }

  public:
    BuzzerTone tones[BUZZER_QUEUE_LENGTH];

    BuzzerToneFifo() : ridx(0), widx(0), tones() {};

    bool empty() const
    {
      return ridx == widx;
    }

    bool full() const
    {
      return ridx == nextIdx(widx);
    }

    void clear()
    {
      widx = ridx;                      // clean the queue
    }

    const uint8_t get()
    {
      if (!empty()) {
        const uint8_t currentRIdx = ridx;
        ridx = nextIdx(ridx);
        return currentRIdx;
      }
      return 0;
    }

    void push(const BuzzerTone & tone)
    {
      if (!full()) {
        tones[widx] = tone;
        widx = nextIdx(widx);
      }
    }
};

void playTone(uint16_t freq, uint16_t len, uint16_t pause = 0, uint8_t flags = 0, int8_t freqIncr = 0);
void buzzerEvent(unsigned int index);
void buzzerOn();
void buzzerOff();
void buzzerSound(uint8_t duration);
void audioKeyPress();
void audioKeyError();
void audioTrimPress(int value);
void audioTimerCountdown(uint8_t timer, int value);
void buzzerHeartbeat();

#endif // __cplusplus

#endif // _BUZZER_DRIVER_H_