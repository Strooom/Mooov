// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once

#ifndef WIN32
#include <Arduino.h>
#else
#include <stdint.h>
#endif
#include "gcodeparseresult.h"        // required as gCodeParseResult is passed as a parameter to some memberfunctions
#include "general.h"                 // required for definition of MotionSpeedProfile

class motionTrajectory {
  public:
    void set(const gCodeParserResult& theParseResult);

    //private:
    float startPosition[nmbrAxis]{};        // mm

    // Linear moves
    float delta[nmbrAxis]{};        // mm

    // Helical moves
    axis arcAxis0{axis::nmbrAxis};
    axis arcAxis1{axis::nmbrAxis};
    float arcCenter0{};        // mm
    float arcCenter1{};        // mm
    float startAngle{};        // rad
    float deltaAngle{};        // rad
    float radius{};            // mm

    // Common to all moves
    float length{};                               // mm
    float deltaRealTime[nmbrAxis]{};              // mm
    float directionUnitVector[nmbrAxis]{};        // dimensionless

    uint32_t toString(char* output, const motionType type) const;
};
