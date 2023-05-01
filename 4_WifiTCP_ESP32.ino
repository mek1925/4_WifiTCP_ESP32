
#include <string>
#include <WiFi.h>
#include <ESP32Ping.h>
#include <EEPROM.h>

#define EEPROM_SIZE 1000

void wifiScan();
void initWiFi(String, String);
void ipPing(String, int);
int stringToInt(String);
// String changeString(String);
// String valueString(String);
// int writeStringToEEPROM(int, const String);
// int readStringFromEEPROM(int, String)

String SSID1, PASSWORD1, ipAddres;
int pingCountTime = 0;
int ssid1Offset = 0;
int password1Offset = 20;
int ipAddresOffset = 40;
int pingCountTimeOffset = 60;
int lastOffsetAddr = 63;
uint16_t port = 8080;
static int c = 0, d = 0;
int print = 3;

void setup()
{
    Serial.begin(115200);
    delay(1000);

    // Set WiFi to station mode and disconnect from an AP if it was previously connected
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    Serial.println("\nTesting EEPROM Library\n");
    if (!EEPROM.begin(EEPROM_SIZE))
    {
        Serial.println("Failed to initialise EEPROM");
        Serial.println("Restarting...");
        delay(1000);
        ESP.restart();
    }
    // Serial.print("Size of EEPROM:-");
    // size_t length = EEPROM.length();
    // Serial.println(length);

    delay(1000);
}

void loop()
{
    // static int scanWifi = 0;

    String temp = "";
    String temp1 = "";
    String temp2 = "";
    static String tempSSID = "";
    static String tempPassword = "";
    static int x = 0, s = 0, p = 0;
    // static int tempAddr = 0;
    static int printData = 0;

    if (printData < 1)
    {
        readEEPROMdata();
        printEEPROMdata();
        printData++;
    }

    if (Serial.available() > 0)
    {

        temp = Serial.readString();
        changeString(temp, &temp1);
        valueString(temp, &temp2);

        if (temp2[0] != '\0')
        {
            if (temp1 == "SSID")
            {
                // shiftWriteEEPROMdata(ssid1Offset, &password1Offset, lastOffsetAddr, temp2);
                // writeStringToEEPROM(ssid1Offset, password1Offset, temp2);
                // readEEPROMdata();
                tempSSID = temp2;
                int len = tempSSID.length();
                if (len >= 19)
                {
                    Serial.println("Enter data is to munch long");
                    Serial.println("We can save only first 19 Entered data... ");
                }
                Serial.print("Entered SSID:-");
                Serial.println(tempSSID);
                // WiFi.disconnect();
                x = 0;
                s = 1;
            }
            if (temp1 == "PASSWORD")
            {
                // shiftWriteEEPROMdata(password1Offset, &ipAddresOffset, lastOffsetAddr, temp2);
                // writeStringToEEPROM(password1Offset, ipAddresOffset, temp2);
                // readEEPROMdata();
                tempPassword = temp2;
                int len = tempPassword.length();
                if (len >= 19)
                {
                    Serial.println("Enter data is to munch long");
                    Serial.println("We can save only first 19 Entered data... ");
                }
                Serial.print("Entered PASSWORD:-");
                Serial.println(tempPassword);
                // WiFi.disconnect();
                x = 0;
                p = 1;
            }
            if (temp1 == "HOST IP")
            {
                // shiftWriteEEPROMdata(ipAddresOffset, &pingCountTimeOffset, lastOffsetAddr, temp2);

                int len = temp2.length();
                if (len >= 19)
                {
                    Serial.println("Enter data is to munch long");
                    Serial.println("We can save only first 19 Entered data... ");
                }

                writeStringToEEPROM(ipAddresOffset, (pingCountTimeOffset - ipAddresOffset), temp2);
                // pingCountTimeOffset = writeStringToEEPROM(ipAddresOffset, temp2);
                // writeIntIntoEEPROM(pingCountTimeOffset, pingCountTime);

                readEEPROMdata();
                Serial.print("NEW host IP:-");
                Serial.println(ipAddres);
                x = 0;
            }
            if (temp1 == "PING TIME")
            {
                int tempNum = stringToInt(temp2);
                // byte len = sizeof(tempNum);
                // Serial.print(len);
                // Serial.println("  byte");
                if (tempNum >= 65535)
                {
                    Serial.println("Enter data is to munch long");
                    Serial.println("We can save maximum upto 65535 number... ");
                }
                writeIntIntoEEPROM(pingCountTimeOffset, (lastOffsetAddr - pingCountTimeOffset), tempNum);
                readEEPROMdata();
                Serial.print("NEW Ping Time duration in Sec is:-");
                Serial.println(pingCountTime);
                x = 0;
            }
            if (temp1 == "PORT")
            {
                port = stringToInt(temp2);
                Serial.print("Entered PORT is:-");
                Serial.println(port);
                x = 0;
            }
        }

        if (temp == "*#REBOOT")
        {
            if (s == 1)
            {
                writeStringToEEPROM(ssid1Offset, (password1Offset - ssid1Offset), tempSSID);
                // password1Offset = writeStringToEEPROM(ssid1Offset, temp2);
                // ipAddresOffset = writeStringToEEPROM(password1Offset, PASSWORD1);
                // pingCountTimeOffset = writeStringToEEPROM(ipAddresOffset, ipAddres);
                // writeIntIntoEEPROM(pingCountTimeOffset, pingCountTime);
                s = 0;
            }
            if (p == 1)
            {
                writeStringToEEPROM(password1Offset, (ipAddresOffset - password1Offset), tempPassword);
                // ipAddresOffset = writeStringToEEPROM(password1Offset, temp2);
                // pingCountTimeOffset = writeStringToEEPROM(ipAddresOffset, ipAddres);
                // writeIntIntoEEPROM(pingCountTimeOffset, pingCountTime);
                p = 0;
            }
            readEEPROMdata();
            Serial.println("WIFI is now REBOOT");
            Serial.print("NEW SSID:-");
            Serial.println(SSID1);
            Serial.print("NEW PASSWORD:-");
            Serial.println(PASSWORD1);
            WiFi.disconnect();
        }
        else if (temp == "CLEAR")
        {
            clearEEPROM();
            x = 0;
        }
        else if (temp == "SCAN")
        {
            wifiScan();
        }
        else if (temp == "READ")
        {
            printEEPROMdata();
        }
        else if (temp == "EEPROM")
        {
            fullReadEEPROM();
        }
    }

    if (WiFi.status() != WL_CONNECTED)
    {
        if (SSID1[0] == '\0')
        {
            while (x < print)
            {
                Serial.println("Saved SSID is Empty");
                delay(2000);
                x++;
            }
        }
        if (PASSWORD1[0] == '\0' && SSID1[0] != '\0')
        {
            while (x < print)
            {
                Serial.println("Saved PASSWORD is Empty");
                initWiFi(SSID1, PASSWORD1);
                delay(2000);
                if (WiFi.status() == WL_CONNECTED)
                {
                    x = 0;
                    goto STOP;
                }
                x++;
            }
        }
    STOP:;
        if (PASSWORD1[0] != '\0' && SSID1[0] != '\0')
        {
            Serial.print("Saved SSID:-");
            Serial.println(SSID1);
            Serial.print("Saved PASSWORD:-");
            Serial.println(PASSWORD1);
            initWiFi(SSID1, PASSWORD1);
        }
    }

    if (WiFi.status() == WL_CONNECTED)
    {
        if (ipAddres[0] == '\0')
        {
            while (x < print)
            {
                Serial.println("Saved HOST IP is Empty");
                delay(2000);
                x++;
            }
        }
        // if (pingCountTime == 0 && ipAddres[0] != '\0')
        // {
        //     while (x < print)
        //     {
        //         Serial.println("Saved PING TIME is Empty");
        //         delay(2000);
        //         x++;
        //     }
        // }

        if (port == 0 && ipAddres[0] != '\0')
        {
            while (x < print)
            {
                Serial.println("Pls Entered port:-");
                delay(2000);
                x++;
            }
        }
        if (ipAddres[0] != '\0' && port != 0)
        {
            // Serial.print("Saved TCP Host IP:-");
            // Serial.println(ipAddres);
            // Serial.print("Entered port:-");
            // Serial.println(port);
            tcpHost(ipAddres, port);
            // Serial.println("");
            // Serial.println("--------------");
            // Serial.println("");
        }

        //  if (ipAddres[0] != '\0' && pingCountTime != 0)
        // {
        //     Serial.print("Saved PING IP:-");
        //     Serial.println(ipAddres);
        //     Serial.print("Saved CountTime in Sec:-");
        //     Serial.println(pingCountTime);
        //     ipPing(ipAddres, pingCountTime);
        //     Serial.println("");
        //     Serial.println("--------------");
        //     Serial.println("");
        // }
    }
}

void wifiScan()
{
    WiFi.disconnect();
    Serial.println("Scan start...");

    // WiFi.scanNetworks will return the number of networks found

    int n = WiFi.scanNetworks();
    Serial.println("Scan done...");
    Serial.println(" ");
    if (n == 0)
    {
        Serial.println("no networks found");
    }
    if (n > 0)
    {
        Serial.println(" ");
        Serial.print("Number of Networks found is:- ");
        Serial.print(n);
        Serial.println(" ");
        for (int i = 0; i < n; ++i)
        {
            // Print SSID and RSSI for each network found
            Serial.print(i + 1);
            Serial.print(": ");
            Serial.print(WiFi.SSID(i));
            Serial.print(" (");
            Serial.print(WiFi.RSSI(i));
            Serial.print(")");
            Serial.print((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*   ");

            if (WiFi.SSID(i) != SSID1)
            {
                Serial.println(".....Not Saved");
            }
            else
            {
                Serial.println(".....Saved");
            }
            delay(10);
        }
    }

    Serial.println("If you want to Scan again type:-  'SCAN' ");
    delay(1000);
}

void initWiFi(String SSID1, String PASSWORD1)
{

    const char *ssid = SSID1.c_str();
    const char *password = PASSWORD1.c_str();
    uint32_t notConnectedCounter = 0;

    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi ..");

    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.println('.');
        delay(1000);
        notConnectedCounter++;

        if (Serial.available() > 0)
        {
            // continue;
            goto SAVE;
        }
        if (notConnectedCounter > 15)
        {
            // Reset board if not connected
            Serial.println("Please Check saved SSID & PASSWORD");
            Serial.println("If you wnat Scan available WIFI..... Enter:-  'SCAN'");
            // Serial.println("Resetting due to Wifi not connecting...");
            Serial.println("");
            Serial.println("--------------");
            Serial.println("");
            notConnectedCounter = 0;
            // ESP.restart();
        }
    }
    // while (WiFi.status() == WL_CONNECTED)
    // {
    //     /* code */
    // }

    Serial.println("Setup done");
    Serial.println("Now Wifi is connected..");
    Serial.print("RRSI:- ");
    Serial.println(WiFi.RSSI());
    Serial.print("Our ESP32 IP: ");
    Serial.println(WiFi.localIP());
SAVE:;
}

void ipPing(String IP, int time)
{
    bool success = Ping.ping(IP.c_str(), 2);
    // bool success = Ping.ping(WiFi.gatewayIP(), 3);

    if (success)
    {
        // Serial.print(WiFi.gatewayIP());
        Serial.print("Ping succesful...");
        Serial.print(Ping.averageTime());
        Serial.println(" ms");
    }

    else
    {
        Serial.println("Ping failed");
        return;
    }

    for (int i = 0; i < time; i++)
    {
        if (Serial.available() > 0)
        {
            continue;
            // goto SAVE;
        }
        delay(1000);
    }
}

void tcpHost(String IP, uint16_t p)
{
    WiFiClient client;

    if (!client.connect(IP.c_str(), p, 3000))
    {
        c = 0;
        while (d < print)
        {
            Serial.println("Connection to server Host failed");
            // delay(100);
            d++;
        }
    }
    else
    {
        d = 0;
        while (c < print)
        {
            Serial.println("Connected to server Host successful!");
            // delay(100);
            c++;
        }
    }

    // client.print("Hello from ESP32!");

    // Serial.println("Disconnecting...");
    // client.stop();

    // delay(5000);
}

int writeStringToEEPROM(int addrOffset, int limit, const String &strToWrite)
{
    int len = strToWrite.length();

    if (len < limit)
    {
        for (int i = addrOffset + 1; i < limit - 2; i++)
        {
            EEPROM.write(i, 0);
        }
        EEPROM.commit();
    }
    if (len >= limit)
    {
        len = limit - 1;
    }
    EEPROM.writeInt(addrOffset, len);
    for (int i = 0; i < len; i++)
    {
        EEPROM.write(addrOffset + 1 + i, strToWrite[i]);
    }

    EEPROM.commit();
    return addrOffset + 1 + len;
}

int readStringFromEEPROM(int addrOffset, String *strToRead)
{
    int newStrLen = EEPROM.read(addrOffset);

    // *strToRead = EEPROM.readString(addrOffset+1);

    if (newStrLen > 0)
    {
        char data[newStrLen + 1];
        for (int i = 0; i < newStrLen; i++)
        {
            data[i] = EEPROM.read(addrOffset + 1 + i);
        }
        data[newStrLen] = '\0';

        *strToRead = String(data);
    }
    else
    {
        *strToRead = "\0";
    }

    return addrOffset + 1 + newStrLen;
}

int writeIntIntoEEPROM(int addrOffset, int limit, int number)
{
    // byte len = sizeof(number);
    // byte len1 = sizeof(limit);

    if (number >= 65535)
    {
        number = 0xFFFF;
    }
    EEPROM.write(addrOffset, number >> 8);
    EEPROM.write(addrOffset + 1, number & 0xFF);
    EEPROM.commit();
    return addrOffset + limit;
}

int readIntFromEEPROM(int addrOffset, int *intToRead)
{
    *intToRead = (EEPROM.read(addrOffset) << 8) + EEPROM.read(addrOffset + 1);
    return addrOffset + 2;
}

int stringToInt(String inputString)
{
    int number = 0;
    for (int i = 0; inputString[i] != '\0'; i++)
    {
        number = number * 10 + (inputString[i] - 48);
    }
    return number;
}

void clearEEPROM()
{
    for (int i = 0; i < EEPROM_SIZE; i++)
    {
        EEPROM.writeInt(i, 0);
    }
    EEPROM.commit();
    readEEPROMdata();

    Serial.println("Clear EEPROM data done!....");
}

void fullReadEEPROM()
{
    Serial.println("");
    Serial.println("**********");
    Serial.println("EEPROM DATA:-");

    char data[EEPROM_SIZE];
    // int n = 0;
    for (int i = 0; i < EEPROM_SIZE; i++)
    {
        data[i] = EEPROM.read(i);

        // if ((65 <= data <= 90) && (97 <= data <= 122))
        // {
        //     Serial.print(char(data));
        // }
        if (i == ssid1Offset || i == password1Offset || i == ipAddresOffset || i == pingCountTimeOffset || i == pingCountTimeOffset + 1)
        {
            Serial.print(int(data[i]));
            Serial.print(",");
            // n++;
        }
        else if ((int(data[i])) == 0)
        {
            Serial.print(",");
            // n++;
        }
        else
        {
            Serial.print(data[i]);
            Serial.print(",");
            // n++;
        }
    }
    Serial.println("");
    // Serial.print("Total data read:-");
    // Serial.println(n);
    Serial.println("**********");
}

void readEEPROMdata()
{
    // password1Offset = readStringFromEEPROM(ssid1Offset, &SSID1);
    // ipAddresOffset = readStringFromEEPROM(password1Offset, &PASSWORD1);
    // pingCountTimeOffset = readStringFromEEPROM(ipAddresOffset, &ipAddres);
    // lastOffsetAddr = readIntFromEEPROM(pingCountTimeOffset, &pingCountTime);

    readStringFromEEPROM(ssid1Offset, &SSID1);
    readStringFromEEPROM(password1Offset, &PASSWORD1);
    readStringFromEEPROM(ipAddresOffset, &ipAddres);
    readIntFromEEPROM(pingCountTimeOffset, &pingCountTime);
}

void printEEPROMdata()
{
    Serial.println("");
    Serial.print("Saved SSID:-");
    Serial.println(SSID1);
    Serial.print("Saved PASSWORD:-");
    Serial.println(PASSWORD1);
    Serial.print("Saved IP:-");
    Serial.println(ipAddres);
    Serial.print("Saved CountTime in Sec:-");
    Serial.println(pingCountTime);
    Serial.println("");
    Serial.println("---------");
    Serial.println("");
    Serial.println("Read saved data....Enter:- 'READ' ");
    Serial.println("Read EEPROM data....Enter:- 'EEPROM' ");
    Serial.println("Clear EEPROM data....Enter:- 'CLEAR' ");
    Serial.println("Reboot your enter WIFI....Enter:- '*#REBOOT' ");
    Serial.println("Scan available WIFI..... Enter:-  'SCAN'");
    Serial.println("Rewrite SSID..... Enter:-  '*SSID:'NEW SSID'#\r' ");
    Serial.println("Rewrite PASSWORD..... Enter:-  '*PASSWORD:'NEW PASSWORD'#\r' ");
    Serial.println("Rewrite HOST IP..... Enter:-  '*HOST IP:'NEW HOST IP'#\r' ");
    Serial.println("Rewrite Port..... Enter:-  '*PORT:'PORT Number'#\r' ");
    // Serial.println("Rewrite PING TIME..... Enter:-  '*PING TIME:'NEW PING TIME'#\r' ");
    Serial.println("");
    Serial.println("---------");
    Serial.println("");
}

void changeString(String temp, String *newString)
{

    char new1[temp.length()];
    int in = 0; // Variable to track whether we are inside the tag
    int out = 0;
    int index = 0;

    for (int i = 0; i < temp.length(); i++)
    {
        if (temp[i] == '*')
        {
            in = 1;
            continue;
        }
        else if (temp[i] == ':')
        {
            in = 0;
            out = 1;
            continue;
        }
        if (in == 1)
        {
            new1[index] = temp[i];
            index++;
        }
    }
    new1[index] = '\0';

    if (out == 1)
    {
        *newString = String(new1);
    }
}

void valueString(String temp, String *newString)
{

    char new1[temp.length()];
    int in = 0;  // Variable to track whether we are inside the tag
    int out = 0; // to track tag complited
    int index = 0;

    for (int i = 0; i < temp.length(); i++)
    {
        if (temp[i] == ':')
        {
            in = 1;
            continue;
        }
        else if (temp[i] == '#')
        {
            in = 0;
            out = 1;
            continue;
        }
        if (in == 1)
        {
            new1[index] = temp[i];
            index++;
        }
    }
    new1[index] = '\0';

    // Remove the trailing spaces from the beginning
    while (new1[0] == ' ')
    {
        // Shift the string to the left
        for (int i = 0; i < strlen(new1); i++)
        {
            new1[i] = new1[i + 1];
        }
    }
    // Remove the trailing spaces from the end
    while (new1[strlen(new1) - 1] == ' ')
    {
        new1[strlen(new1) - 1] = '\0';
    }

    if (out == 1)
    {
        *newString = String(new1);
    }

    // return new;
}
