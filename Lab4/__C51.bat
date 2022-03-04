@echo off
::This file was created automatically by CrossIDE to compile with C51.
D:
cd "\Academics\ELEC 291\Lab4\"
"D:\Academics\Programs\CrossIDE\CrossIDE\Call51\Bin\c51.exe" --use-stdout  "D:\Academics\ELEC 291\Lab4\adc_spi.c"
if not exist hex2mif.exe goto done
if exist adc_spi.ihx hex2mif adc_spi.ihx
if exist adc_spi.hex hex2mif adc_spi.hex
:done
echo done
echo Crosside_Action Set_Hex_File D:\Academics\ELEC 291\Lab4\adc_spi.hex
