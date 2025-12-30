#pragma once
#include "Types.h"

ServoCal readCalibration(ServoType type);

void calibrateSetup();
void calibrateLoop();
