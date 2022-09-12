MAJOR TO-DO: I just noticed that frequencies written to output are missing leading zeroes to the right of the decimal place, so
either fix that by hand in a spreadsheet, or wait until I fix the bug. Out of like 300 repeaters, I only had a few to update.

hamcereek v0.1
--------------

hamcreek is going to be an open-ended amateur radio configuration converter, assuming anyone else contributes to it,
because I only have one radio to test and develop for, for the forseeable future. Currently this only does one conversion:
It converts channel definitions from CHIRP CSV format to the CSV import format used by the DJ-MD5XT(G) CPS software.
It would be nice if it would convert in the other direction, but I can't get CPS to read the radio, and that could be
because I'm running it in WINE, which is seldom a "supported" way to use anything. You may nonetheless find it helpful to know
that the Alinco CPS does work for programming the radio from Linux, at least under Debian 11/Bullseye, via WINE.

Current usage:
> hamcreek chirp.csv > Channel.csv

You can then open the Alinco CPS for the device and go to Tool->Import, and specify the resulting Channel.csv as a source
of channel data to import. That import will replace the channel data for the currently loaded codeplug. You can then
flash your codeplug to the radio as usual, via Program->Write to Radio.

You can obtain bulk exports of channel data in CHIRP format from RepeaterBook.com, and probably other places, too.

For reference, the Alinco page for this device is here
(https://www.remtronix.com/land-mobile-radio/digital-radios/dj-md5xt/)

Upcoming features: I'm new to DMR and have never used it before, but when I get it figured out, I will look into
adding support for contacts and so forth.

TODO:

This was originally envisioned as single-purpose, but I don't see why it shouldn't work to add interchange of lots more formats.
The internal format is geared for the DJ-MD5X devices, but some minor changes would make it more general. Specifically,
the power levels are the funky "Small", "Low", etc options which do not generalize well to other devices. Doesn't need
much to begin supporting other data formats, though.

FAQ
----

Q: How do I get Alinco DJ-MD5X CPS working under Linux?

A: Install WINE, and then use "winetricks" to load a bunch of Microsoft components. I'm too lazy to go enumerate them,
	but it's stuff like the Common Controls, OLE, and the MFC runtime. A Visual Basic runtime might have been needed?
	You should be able to run the installer for CPS, and it will work, especially once you have the prerequisites from
	winetricks installed. Make sure the UI is not coming up with controls absent, and that you are not getting obvious
	errors about missing libraries in the console output. You will want to create a registry entry to map COM1 to the
	radio, which comes up on /dev/ttyACM0, for me. If you try to flash the firmware, and it doesn't want to work,
	run the Update program directly, after editing the config file to specify 115200bps. That worked for me.
	
Q: My PTT sticks whenever CTCSS is enabled for an analog channel

A: This was a bizarre error I encountered, and I fixed it by performing a factory reset on the radio. Maybe its state
	got corrupted by some invalid value during testing? Who knows. You can factory-reset the device by turning it
	on while holding in PTT an PF1 (directly below PTT). If that doesn't work, it's probably because you have not
	enabled power-on resets in the codeplug. Go into "Optional Setting" in CPS, and enable power-on resets, then
	flash the device with the resulting codeplug, and now you should be able to factory-reset it during power-up.
	
