#ifndef __d12_CONFIG_H
#define __d12_CONFIG_H

#ifndef d12_CONFIG_USE_GPIO_PIN
  #define d12_CONFIG_USE_GPIO_PIN   (1)
    /*!< 1: use GPIO pin; 0: use PWM pin */
#endif

#ifndef d12_CONFIG_IS_LOW_ACTIVE
  #define d12_CONFIG_IS_LOW_ACTIVE   (1)
    /*!< 1: LED is low active (cathode on port side), 0: LED is HIGH active (anode on port side) */
#endif

#endif /* __d12_CONFIG_H */
