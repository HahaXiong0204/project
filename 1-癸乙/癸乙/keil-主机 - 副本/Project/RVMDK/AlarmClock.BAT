SET PATH=D:\software\Keil5\ARM\ARMCC\Bin;F:\mingw64\bin;C:\Program Files (x86)\Common Files\Oracle\Java\javapath;D:\software\VMware\bin\;C:\Program Files\Common Files\Oracle\Java\javapath;D:\software\Qt\5.14.2\mingw73_32\bin;C:\WINDOWS\system32;C:\WINDOWS;C:\WINDOWS\System32\Wbem;C:\WINDOWS\System32\WindowsPowerShell\v1.0\;C:\WINDOWS\System32\OpenSSH\;C:\Program Files\Microsoft SQL Server\150\Tools\Binn\;C:\Program Files\Microsoft SQL Server\Client SDK\ODBC\170\Tools\Binn\;C:\Program Files\MySQL\MySQL Server 8.0\bin;D:\software\微信web开发者工具\dll;C:\Str;wberry\c\bin;C:\Strawberry\perl\site\bin;C:\Strawberry\perl\bin;D:\software\CMake\bin;D:\software\OpenSSH\bin;D:\software\Microsoft VS Code\arm\gcc-arm-none-eabi-10.3-2021.10-win32\gcc-arm-none-eabi-10.3-2021.10\arm-none-eabi\bin;D:\software\Microsoft VS Code\arm\openocd-20211118\OpenOCD-20211118-0.11.0\bin;D:\software\vs_studio\VC\Tools\MSVC\14.33.31629\bin\Hostx64\x86;D:\software\SQLite;D:\software\Git\cmd;C:\Program Files\Docker\Docker\resources\bin;D:\software\xshell\;C:\Program Files (x86)\STMicroelectronics\STM32 ST-LINK Utility\ST-LINK Utility;D:\software\python 3.10\Scripts\;D:\software\python 3.10\;C:\Users\xq\AppData\Local\Microsoft\WindowsApps;D:\software\Microsoft VS Code\bin;D:\software\LLVM\bin;C:\Users\xq\AppData\Local\GitHubDesktop\bin;D:\software\Cpolar_amd\
SET ARMCC5_ASMOPT=--diag_suppress=9931
SET ARMCC5_CCOPT=--diag_suppress=9931
SET ARMCC5_LINKOPT=--diag_suppress=9931
SET CPU_TYPE=STM32F103C6
SET CPU_VENDOR=STMicroelectronics
SET UV2_TARGET=AlarmClock
SET CPU_CLOCK=0x00B71B00
"D:\software\Keil5\ARM\ARMCC\Bin\ArmCC" --Via ".\obj\main.__i"
"D:\software\Keil5\ARM\ARMCC\Bin\ArmCC" --Via ".\obj\1602.__i"
"D:\software\Keil5\ARM\ARMCC\Bin\ArmCC" --Via ".\obj\core_cm3.__i"
"D:\software\Keil5\ARM\ARMCC\Bin\ArmCC" --Via ".\obj\system_stm32f10x.__i"
"D:\software\Keil5\ARM\ARMCC\Bin\ArmAsm" --Via ".\obj\startup_stm32f10x_cl._ia"
"D:\software\Keil5\ARM\ARMCC\Bin\ArmCC" --Via ".\obj\misc.__i"
"D:\software\Keil5\ARM\ARMCC\Bin\ArmCC" --Via ".\obj\stm32f10x_gpio.__i"
"D:\software\Keil5\ARM\ARMCC\Bin\ArmCC" --Via ".\obj\stm32f10x_rcc.__i"
"D:\software\Keil5\ARM\ARMCC\Bin\ArmCC" --Via ".\obj\stm32f10x_tim.__i"
"D:\software\Keil5\ARM\ARMCC\Bin\ArmCC" --Via ".\obj\stm32f10x_flash.__i"
"D:\software\Keil5\ARM\ARMCC\Bin\ArmCC" --Via ".\obj\stm32f10x_adc.__i"
"D:\software\Keil5\ARM\ARMCC\Bin\ArmCC" --Via ".\obj\stm32f10x_usart.__i"
"D:\software\Keil5\ARM\ARMCC\Bin\ArmCC" --Via ".\obj\esp8266.__i"
"D:\software\Keil5\ARM\ARMCC\Bin\ArmCC" --Via ".\obj\mqttkit.__i"
"D:\software\Keil5\ARM\ARMCC\Bin\ArmCC" --Via ".\obj\onenet.__i"
"D:\software\Keil5\ARM\ARMCC\Bin\ArmCC" --Via ".\obj\delay.__i"
"D:\software\Keil5\ARM\ARMCC\Bin\ArmCC" --Via ".\obj\sys.__i"
"D:\software\Keil5\ARM\ARMCC\Bin\ArmCC" --Via ".\obj\usart.__i"
"D:\software\Keil5\ARM\ARMCC\Bin\ArmLink" --Via ".\Obj\stm32.lnp"
"D:\software\Keil5\ARM\ARMCC\Bin\fromelf.exe" ".\Obj\stm32.axf" --i32combined --output ".\Obj\stm32.hex"
