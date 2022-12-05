# Converting TTF and OTF fonts to smooth fonts
1. Download Processing IDE from https://processing.org/download
1. Copy truetype font to \data directory
1. Open Create_font.pde in processing
    * Change font name 
    * Change desired output size
1. Copy .vlw files to \data subdirectory under the sketch
1. Upload data to SPIFFS using "ESP32 Sketch data upload" tool
2. Ensure SPIFFS is loaded by encorporating SPIFFS.begin() in the code




The .vlw files must be uploaded to the processors FLASH filing system (SPIFFS, LittleFS or SD card) for use. Alternatively the .vlw files can be converted to C arrays (see "Smooth Font -> FLASH_Array" examples) and stored directly in FLASH as part of the compile process. The array based approach is convenient, provides performance improvements and is suitable where: either use of a filing system is undesirable, or the processor type (e.g. STM32) does not support a FLASH based filing sytem.



//String fontName ="Noway_Regular";  
String fontName ="Noway_Light";  
//String fontName ="Noway_Medium";  
