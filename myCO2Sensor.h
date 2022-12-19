

byte cmd[9] = {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};  // get gas command
byte cmdCal[9] = {0xFF, 0x01, 0x87, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78};  // calibrate command
char response[9];  // holds the recieved data


unsigned long warmingTimer = 0;
unsigned long warmUpTime = 60000;
unsigned long maxWaitTime = 3000;

void co2Init(){
	warmingTimer = millis();  // initilize warmup timer
}

bool co2IsReady(){
	return (millis() - warmingTimer > warmUpTime);
}

int co2ppmRemainingSeconds(){
	return (int)((warmUpTime - (millis() - warmingTimer)) / 1000);
}
	
int co2GetReading()
{
	while (Serial.available())  // this clears out any garbage in the RX buffer
	{
		int garbage = Serial.read();
	}

	Serial.write(cmd, 9);  // Sent out read command to the sensor
	Serial.flush();  // this pauses the sketch and waits for the TX buffer to send all its data to the sensor

	unsigned long startTime = millis();  // start the timer
	while (!Serial.available())  // this pauses the sketch and waiting for the sensor responce
	{
		if (millis() - startTime > maxWaitTime)  // if the sensor did not respond in time
		{
			return -1;  // return -1 to indicate that the sensor did not respond
		}
		delay(1);
	}

	Serial.readBytes(response, 9);  // once data is avilable, it reads it to a variable
	int responseHigh = (int)response[2];
	int responseLow = (int)response[3];
	return (256 * responseHigh) + responseLow;
}

void co2Calibrate()
{
	Serial.write(cmdCal, 9);
	delay(3000);
}

