#ifndef TELNET_H
#define TELNET_H
	#include <Arduino.h>
	
	class adri_telnet {
	private:
		
	public:
		adri_telnet();
		static void telnet_setup();
		static void telnet_loop();
		static void telnet_put(String output_string);
		static boolean telnet_get(String & input_string);		
	};

#endif // TELNET_H
