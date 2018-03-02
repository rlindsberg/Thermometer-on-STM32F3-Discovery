#ifndef __DISPLAY_H
#define __DISPLAY_H

void initDisplay(void);
void sendCommandToSPI(uint8_t commandToBeSent[]);
void selectRow(uint8_t rowNr);
void sendDataToDisplay(uint8_t asciiData);
void sendCharToDisplay(char charBuffer[]);



#endif // DISPLAY_H_
