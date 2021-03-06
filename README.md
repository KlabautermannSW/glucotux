# Important safety information

**This program is experimental software, not developed or supported by Bayer. It might damage your meter or render it unreliable. Use this software at your own risk. You have been warned!**

# Glucotux
This project is done because I never found an application to read data from a Bayer Contour USB Next &reg; glucose meter running on a current Linux supporting a GUI. Most stuff I found was more or less experimental or done not using my favorite programming language : C.

So in mid 2012 I decided to do it by myself.

More information in German language can be found on my webpage [www.GlucoTux.de](https://www.glucotux.de/)

## gluctotux
This application is not implemented yet!

It should be a GUI to the work that is developed with glucotux-cli.

## glucotux-cli
This is the playground to test the code for handling a connection to a Bayer Contour USB Next &reg; glucose meter and to read out and display the data in a human readable way.

glucotux-cli uses the "Data Transfer Mode" of "ASTM Standard E 1394-91". So it reads out all available data records.

The output file holds all result records each in one line. The data is blank separated and read like this example.
```
201707210848  181 mg/dL  N   Glucose  R    17
```
| **value** | **meaning** | **comment** |
| - | - | - |
|  201706141312 | time stamp | YYYYMMDDhhmm |
| 154 | result | see explanation below the table |
| mg/dL | unit | |
| B | user mark | 'B' before meal, 'A' after meal, 'F' fasting, 'N' none (for Glucose only), 'O' for Carb and Insulin |
| Glucose | result type | 'Glucose', 'Carb', 'Insulin' |
| R | record type | 'H' header, 'P' not used, 'R' clinical result |
| 23 | record number | starts with '1' and may run up to '2000' |

### result ###
- Glucose
  - number of units given with 'unit'
- Carb
  - number of units given by unit code (see below)
- Insulin
  - tenth of units (international 'U', german 'IE' or 'I.E.')

### unit ###
**Unit texts ginen in German !**
- Glucose
  - 'mmg/dl'
  - 'mmol'
- Carb
  - '1' : Gramm
  - '2' : BE
  - '3' : KE
- Insulin
  - '1' : schnell wirksames Insulin
  - '2' : langsam wirksames Insulin
  - '3' : Mischinsulin

## Environment
- Bayer Contour USB Next &reg;
- Xubuntu 17.10 with kernel 4.13.0-36
- 64 bit x86 PC
- GNU Make 4.1
- gcc version 7.2.0
- libgtk-3-dev
- atom 1.24.0
- Coding style
  - intending with four spaces / no tabs!
  - single line comments beginning with '//' (C++ style)

## Create
In this direcetory simply call
```
$ make
```
To remove all objects and binaries call
```
$ make clean
```
If you don't the want buid in debugging code you should comment out the first `CCFLAGS` line in `Makefile` an use the second one :
```
#CFLAGS = -I $(DINC) -Wall -O3 -DVERSION=\"$(VERSION)\" -D_DEBUG_
CFLAGS = -I $(DINC) -Wall -O3 -DVERSION=\"$(VERSION)\"
```

## Add Your Glucometer to the System
To prevent the need of "sudo" to get access to your glucometer you should add it to the linux system and enable access to it.

Attach your glocometer to your computer. Then open a terminal and run "lsusb". You will see one of the lines identifying your glocometer.
```
$ lsusb
Bus 001 Device 005: ID 1a79:7410 Bayer Health Care LLC Contour Next
Bus 001 Device 006: ID 1a79:7800 Bayer Health Care LLC
Bus 001 Device 007: ID 1a79:6002 Bayer Health Care LLC Contour
```
**Next do with superuser rights:**

Create a new group:
```
# groupadd --system glucotux
```
Then create a new udev rule replacing the product id (7410 in this sample) with the id of your glucometer. That's the number behind the colon in the ID you found with "lsusb":
```
# echo 'ACTION=="add", KERNEL=="hiddev*", ATTRS{idVendor}=="1a79", ATTRS{idProduct}=="7410", GROUP="gluctux", MODE="0660"' > /etc/udev/rules.d/30-glucometer.rules
```

Use your preferred user administration tool and add the new group to your user.

Then you have to relogin at your computer.

## Run
```
$ bin/glucotux-cli -h

"Glucotux CLI version V0.03", (c) Uwe Jantzen (Klabautermann-Software) Apr 16 2018

Usage:
        bin/glucotux-cli [options] [-o <filename>]
Options:
        -o <filename> file to put the data in,
                      if not set data is printed to screen
        -v            enable verbose mode
        -d            enable debug mode
        -h            show this help then stop without doing anything more
```
To get connection to a Bayer Contour USB Next &reg; device you must run the application with superuser permissions.

It is required that you run the application FIRST and then attach the Bayer Contour USB Next &reg; device to a USB port.
```
$ bin/glucotux-cli -o 180307.dat

"Glucotux CLI version V0.03", (c) Uwe Jantzen (Klabautermann-Software) Apr 16 2018

2000
Glucotux CLI finished
```
If you do not  as this you will get the following error message :
```
$ bin/glucotux-cli -o 180307.dat

"Glucotux CLI version V0.03", (c) Uwe Jantzen (Klabautermann-Software) Apr 16 2018

-
Communication can't be established if Contour device is just attached!
Please remove the Contour device and wait some seconds.
Then FIRST start the program and SECOND attach the Contour device.
```
# What's Planned
Topics that I have on my to do list you may [find here](ToDo.md).

# Credits

This code is not forked but depends largely on the work of Steve Sloan
(CodeMonkeySteve) [contour-glucose](https://github.com/CodeMonkeySteve/contour-glucose) and the fork of it done by pietrasagh [contour-glucose](https://github.com/pietrasagh/contour-glucose/tree/contour_plus_one).
