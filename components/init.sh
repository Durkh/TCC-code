a='idf_component_register(	SRCS "Adafruit_Sensor.cpp"
			INCLUDE_DIRS "."
			PRIV_REQUIRES arduino-esp32)'

b='idf_component_register(SRCS "Adafruit_BusIO_Register.cpp"
							"Adafruit_GenericDevice.cpp"
							"Adafruit_I2CDevice.cpp"
							"Adafruit_SPIDevice.cpp"
						INCLUDE_DIRS "."
						PRIV_REQUIRES arduino-esp32)'

c='idf_component_register(	SRCS "Adafruit_AHTX0.cpp"
						INCLUDE_DIRS "."
						PRIV_REQUIRES arduino-esp32 busyIO adafruit_sensor)'

d='idf_component_register( SRCS "src/ScioSense_ENS160.cpp"
                        INCLUDE_DIRS "src"
                        REQUIRES arduino-esp32)'


echo $a>adafruit_sensor/CMakeLists.txt
echo $b>busyIO/CMakeLists.txt
echo $c>AHT/CMakeLists.txt
echo $d>ENS/CMakeLists.txt
