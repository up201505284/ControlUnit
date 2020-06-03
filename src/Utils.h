#ifndef _UTILS_H_
#define _UTILS_H_
#include <stdint.h>

void        InitPorts               (void               );
void        InitAdc                 (void               );
uint16_t    ReadAdc                 (void               );
void        SetupPwm                (uint8_t _dutyCycle );
void        SetupExternalInterrupts (uint8_t _state     );
void        SetupSpi                (void               );
void        DisablePwm              (void               );
//uint8_t     Round                   (float _number      );
//float       CalculateTunning        (float _pulseRate   );
#endif //  _UTILS_H_