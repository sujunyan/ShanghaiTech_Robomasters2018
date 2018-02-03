SET PATH=F:\keil\ARM\ARMCC\Bin;C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v9.0\bin;C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v9.0\libnvvp;;C:\ProgramData\Oracle\Java\javapath;D:\软件\python\Scripts\;D;\软件\python\;C;\Program Files (x86)\Intel\iCLS Client\;C:\Program Files\Intel\iCLS Client\;C:\Program Files (x86)\Lenovo\FusionEngine;C:\Windows\system32;C:\Windows;C:\Windows\System32\Wbem;C:\Windows\System32\WindowsPowerShell\v1.0\;C:\Program Files (x86)\NVIDIA Corporation\PhysX\Common;C:\Program Files (x86)\Intel\Intel(R) Management Engine Components\DAL;C:\Program Files\Intel\Intel(R) Management Engine Components\DAL;C:\Program Files (x86)\Intel\Intel(R) Management Engine Components\IPT;C:\Program Files\Intel\Intel(R) Management Engine Components\IPT;E:\Dev-Cpp\MinGW64\bin;C:\Windows.old\Users\lenovo\AppData\Local\Programs\Python\Python35;C:\Program Files\Calibre2\;D:\软件\laTex\MiKTeX\miktex\bin\x64\;\Program Files (x86)\Windows Kits\8.1\Windows Performance Toolkit\;C:\Program Files\Git\cmd;C:\Program Files\nodejs\;C:\Program Files\Intel\WiFi\bin\;C:\Program Files\Common Files\Intel\WirelessCommon\;J:\matlab\runtime\win64;J:\matlab\bin;C:\Program Files\PuTTY\;C:\WINDOWS\system32;C:\WINDOWS;C:\WINDOWS\System32\Wbem;C:\WINDOWS\System32\WindowsPowerShell\v1.0\;C:\Users\lenovo\.cargo\bin;C:\Users\lenovo\AppData\Local\Microsoft\WindowsApps;C:\Users\lenovo\AppData\Roaming\npm;D:\软件\python;J:\Download\curl-7.54.0-win64-mingw\curl-7.54.0-win64-mingw\bin
SET CPU_TYPE=STM32F427IIHx
SET CPU_VENDOR=STMicroelectronics
SET UV2_TARGET=RMv3testDemo
SET CPU_CLOCK=0x0A037A00
"F:\keil\ARM\ARMCC\Bin\ArmAsm" --Via ".\rmv3testdemo\startup_stm32f427xx._ia"
"F:\keil\ARM\ARMCC\Bin\ArmCC" --Via ".\rmv3testdemo\stm32f4xx_hal_pcd.__i"
"F:\keil\ARM\ARMCC\Bin\ArmCC" --Via ".\rmv3testdemo\stm32f4xx_hal_rcc.__i"
"F:\keil\ARM\ARMCC\Bin\ArmCC" --Via ".\rmv3testdemo\stm32f4xx_hal_uart.__i"
"F:\keil\ARM\ARMCC\Bin\ArmCC" --Via ".\rmv3testdemo\stm32f4xx_hal_can.__i"
"F:\keil\ARM\ARMCC\Bin\ArmCC" --Via ".\rmv3testdemo\stm32f4xx_hal_spi.__i"
"F:\keil\ARM\ARMCC\Bin\ArmCC" --Via ".\rmv3testdemo\stm32f4xx_hal_dma.__i"
"F:\keil\ARM\ARMCC\Bin\ArmCC" --Via ".\rmv3testdemo\stm32f4xx_hal_pwr_ex.__i"
"F:\keil\ARM\ARMCC\Bin\ArmCC" --Via ".\rmv3testdemo\stm32f4xx_hal_tim.__i"
"F:\keil\ARM\ARMCC\Bin\ArmCC" --Via ".\rmv3testdemo\stm32f4xx_hal_flash_ex.__i"
"F:\keil\ARM\ARMCC\Bin\ArmLink" --Via ".\RMv3testDemo\RMv3testDemo.lnp"
"F:\keil\ARM\ARMCC\Bin\fromelf.exe" ".\RMv3testDemo\RMv3testDemo.axf" --i32combined --output ".\RMv3testDemo\RMv3testDemo.hex"
fromelf.exe  --bin -o ./bin/test.bin .\RMv3testDemo\RMv3testDemo.axf
