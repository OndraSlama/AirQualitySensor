#include <WiFiUdp.h>
#include <NTPClient.h>

//Week Days
String weekDays[7]={"Ne", "Po", "Ut", "St", "Ct", "Pa", "So"};
//Month names
// String months[12]={"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");



void setupDatetime() {
	timeClient.begin();
	timeClient.setTimeOffset(3600);
}

String getFormattedTime(int noSeconds=false) {
	timeClient.update();
	String formattedTime = timeClient.getFormattedTime();
	if (noSeconds) {
		formattedTime = formattedTime.substring(0, 5);
	}
	return formattedTime;
}

String getFormattedDate() {
	// Get date
	String weekDay = weekDays[timeClient.getDay()];
	time_t epochTime = timeClient.getEpochTime();
	struct tm *ptm = gmtime ((time_t *)&epochTime); 
	int monthDay = ptm->tm_mday;
	String monthDayStr = monthDay < 10 ? "0" : "" + String(monthDay);
	int currentMonth = ptm->tm_mon+1;
	String currentMonthStr = currentMonth < 10 ? "0" : "" + String(currentMonth);
	int currentYear = ptm->tm_year+1900;

	//Print complete date:
	String currentDate =weekDay + " " + String(monthDay) + "." + String(currentMonthStr) + ".";

	return currentDate;
}