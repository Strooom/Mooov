// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author : Pascal Roobrouck - @strooom                                  ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once

#include <Arduino.h>
#include "event.h"

// -------------------------------
// raw GPIO - hardware abstraction
// -------------------------------
// Note : one object instance represents all needed I/Os
// 6 limit switches + 6 buttons

class inputs {
  public:
    inputs();
    bool get(uint32_t index) const;
    void sample();

#if defined(__MK64FX512__) || defined(__MK66FX1M0__)        // Teensy 3.5 || Teensy 3.6
    static constexpr uint8_t nmbrInputs{12};                // how many inputs does this particular HW have
#else
    static constexpr uint8_t nmbrInputs{12};
#endif

#ifndef UnitTesting
  private:        // commented out during unit testing
#endif
    uint32_t theInputs{0};
    static constexpr uint32_t inputMask{0x00'00'0F'FF};        // this mask allows to invert individual bits in case of active low signals, io active high
};

// ---------------------------------------------------
// debounced input - mapped to IO and generates events
// ---------------------------------------------------
// Note : one instance per I/O

class debouncedInput {
  public:
    debouncedInput(inputs &theInputs, uint32_t index, Event onOpen, Event onClose);        // constructor
    bool getState() const;                                                                 // get current state of the input
    Event getEvent();                                                                      // get press or release event

#ifndef UnitTesting
  private:        // commented out during unit testing
#endif
    inputs &theInputs;           // reference to the HW inputs
    const uint32_t index;        // index into one of the bits of (HW) 'inputs' object
    const Event onOpen;          // event to generate when this input opens
    const Event onClose;         // event to generate when this input closes

    static constexpr uint8_t debounceMaxCount = 4;            // sets the upper boundary for debounceCounter : 4 * 5ms = 20ms
    uint8_t debounceCounter                   = 0;            // counts up (input high) or down (input low) until it hits boundaries 0 or maxCount
    bool currentState                         = false;        // after debouncing : true : button is pressed, false : button is not pressed
    bool previousState                        = false;        // remembers previous state, to detect flanks
};
