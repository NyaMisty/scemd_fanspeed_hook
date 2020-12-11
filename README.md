scemd_fanspeed_hook
=============

hooks scemd's ioctl function, check if it's fanspeed controlling code, then redirecting it to our own fan speed control logic.

The code contains speed control code for ITE IT8728F chip, from the below credits you can find the IT8772E

## Usage:

1. Compile: ```make```

2. On Synology, move the `/usr/syno/bin/scemd` to `/usr/syno/bin/scemd.orig`

3. Copy the compiled `scemd` and `scemd_hijack.so` to `/usr/syno/bin`

4. (Optional) tweak your settings in `/usr/syno/etc/scemd.xml` and `/usr/syno/etc.defaults/scemd.xml`

## Credits:

- [terramaster-fancontrol](https://github.com/ahmedmagdiosman/terramaster-fancontrol): offering a brilliant idea to change the fanspeed
- [ldpreloadhook](https://github.com/poliva/ldpreloadhook): great template code for ioctl hook!

## For developers

- I found the magic ioctl request code by reverse engineering the `scemd`, `libsynoscemd.so.1` and `libhwcontrol.so.1`. The `ScemFanSpeedAdjuster` function in `libsynoscemd.so.1` will do the call to `SYNOFanDebugPrint` in `libhwcontrol.so.1`, then you'll find everything looking at `SYNOFanDebugPrint`'s cross references :)
- You can enable verbose debug log by `echo 1 > /tmp/m2tmp.txt`