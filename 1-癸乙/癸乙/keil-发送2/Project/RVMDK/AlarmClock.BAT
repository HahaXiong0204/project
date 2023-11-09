SET PATH=C:\Keil_v5\ARM\ARMCC\Bin;C:\Windows\system32;C:\Windows;C:\Windows\System32\Wbem;C:\Windows\System32\WindowsPowerShell\v1.0\;C:\Windows\System32\OpenSSH\;C:\Program Files (x86)\Altium Designer 6\System;C:\Program Files (x86)\Microchip\MPLAB C32 Suite\bin;C:\Program Files (x86)\Microchip\xc8\v1.41\bin;C:\Program Files (x86)\Microsoft SQL Server\90\Tools\binn\;C:\Program Files (x86)\Windows Kits\8.1\Windows Performance Toolkit\;C:\Program Files\Microsoft SQL Server\110\Tools\Binn\;%C:\mysql-8.0.26-winx64%\bin;%C:\mysql-8.0.26-winx64%;C:\Program Files\PuTTY\;C:\Users\Administrator\AppData\Local\Microsoft\WindowsApps;
SET CPU_TYPE=STM32F103C6
SET CPU_VENDOR=STMicroelectronics
SET UV2_TARGET=AlarmClock
SET CPU_CLOCK=0x00B71B00
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\obj\main.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\obj\1602.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\obj\d18b20.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\obj\core_cm3.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\obj\system_stm32f10x.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmAsm" --Via ".\obj\startup_stm32f10x_cl._ia"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\obj\misc.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\obj\stm32f10x_gpio.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\obj\stm32f10x_rcc.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\obj\stm32f10x_tim.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\obj\stm32f10x_flash.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\obj\stm32f10x_adc.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\obj\stm32f10x_usart.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmLink" --Via ".\Obj\stm32.lnp"
"C:\Keil_v5\ARM\ARMCC\Bin\fromelf.exe" ".\Obj\stm32.axf" --i32combined --output ".\Obj\stm32.hex"
