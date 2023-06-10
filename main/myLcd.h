#pragma once
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
void lcdInit();
void lcdPrint(String text, int line=0, int col=0, bool clear=false, bool serial=true);
void lcdClear();


void lcdInit() {
	lcd.init();
	lcd.backlight();
	lcd.clear();
	lcdPrint("Loading...");
}

void lcdPrint(String text, int line, int col, bool clearLine, bool serial) {
	if (clearLine) {
		lcdPrint("                ", line, 0, false, serial);
	}

	lcd.setCursor(col, line);
	lcd.print(text);

	// if (serial) {
	// 	Serial.println(text);
	// }
}

void lcdClear() {
	lcd.clear();
}


