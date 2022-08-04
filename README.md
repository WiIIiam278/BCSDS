# BCSDS
[![Discord](https://img.shields.io/discord/818135932103557162?color=7289da&logo=discord)](https://discord.gg/tVYhJfyDWG)

Better Call Saul for the Nintendo DS.

## Playing
For the best experience, copy `bcsds.nds` to a flash cart of your choice and run it on hardware.

You can emulate it using [No$gba](http://problemkaputt.de/gba.htm), DeSMuME or MelonDS (the latter of which you must enable DLDI mode in the config).

Touch the screen to start the game (once you've had enough of the title screen). To play, you need to move the randomly shuffled spliced numbers into the four slots to match Mesa Verde's address&dagger; - 1216 (one after Magna Carta)! Keep sound on for the best experience.

&dagger; *Doesn't Jimmy switch the numbers to 1261 in ths show?* → I think if you double check you'll see that 1216 is correct!!!

## Compiling
Install devkitpro and libnds (On Windows, I reccommend doing this through Msys rather than through the graphical installer). Simply run `make` in the directory to build.

## License
Please see `COPYRIGHTS.md`. The rest of BCSDS, including the main source, is licensed under Apache-2.0.
