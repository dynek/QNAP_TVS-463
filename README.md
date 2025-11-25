# QNAP_TVS-463

### Summary
This is a summary of quite a few days of work (I'm not allowing myself to work 100% on geeky projects, family first 😁) and because it took quite a bit of back-and-forth, there may be errors. If you ever find this useful, feel free to let me know if there's anything that didn't work for you via issues, PR or discussions.

### Goal
**Operating System**  
Initially I wanted to repurpose this *old* piece of hardware with a more recent operating system that better fits my requirements. To be honest, I already have another beefy NAS running TrueNAS Scale (25.10 at the time of writing this), and I wanted to run the same operating system on the QNAP as a backup target.

**Hardware**  
Then later on, as always with "pet projects" I fell in a rabbit hole, and spent way more time than I initially expected. Not only was it a challenge to get TrueNAS working smoothly, but I also realized I couldn't just call it a day and make no use of the NAS buttons, LEDs, and ... LCD!  
This kept me awake quite a few evenings ... erm ... nights until I reached a satisfying point. I was able to find multiple forum posts, GitHub pages, and whatnot about interacting with the LCD but none of the solutions worked for me; it was quite tricky to get working.

### TrueNAS
This is an operating system I have been using for the past couple of months in my new NAS, and I simply love it compared to what I was used to with QNAP. I do not critize QTS, it has been running flawlessly for quite a few years, but compared to TrueNAS, QTS feels more like a hobbyist system. And to my taste, on the default installation QNAP is trying to accomodate too many services and softwares. Also I needed something more responsive but that's not fair, as their choice was to use a USB 2.0? DOM. Seriously, QNAP?

**Components**  
* I kept the same 4 hard drives: Wester Digital WD1003FBYX 1Tb each.
* There'a PCIe connector next to the power supply, I purchased an [Axagon PCEM2-N](https://www.galaxus.ch/fr/s1/product/axagon-pcem2-n-controleur-memoire-16647150) (about 10.- CHF). Let me be nice, here's an [expansion slot bracket](https://www.thingiverse.com/thing:7212481) I designed for this card.
* I installed a [Transcend PCIE SSD 110S 256GB](https://www.galaxus.ch/fr/s1/product/transcend-pcie-ssd-110s-256gb-256-go-m2-2280-ssd-10088992), but I went for the cheapest model I could find (about 25.- CHF)
* During my experimentation, I managed to destroy the original Apacer DOM, which I eventually replaced with a generic USB stick and a home-made connector. It’s simply a 9-pin USB header, using only the top four pins, probably in the wrong order (+ | D- | D+ | GND). This is still required to chainload the OS on the NVMe, the BIOS is too old to enumerate such devices.
* I also took the opportunity (because I had one around) to change the stock fan with a BeQuiet something.

**Installation**  
Of course you will need a monitor (HDMI) and a keyboard; network may also be a good idea at a later stage.  
I'm a heavy user of [Ventoy](https://ventoy.net/en/index.html); I simply downloaded [TrueNAS Scale](https://www.truenas.com/download-truenas-community-edition/), booted Ventoy and installed TrueNAS on the NVMe.
To enter the BIOS, one would simply hit the DELETE key once prompted by the AMI BIOS. Reach the boot options and make sure legacy is selected with only the Ventoy USB stick in the devices.

⚠️ BTW, do not take this as an advise, do it at your own risk ⚠️ I [updated the BIOS](https://forum.qnap.net.pl/download/tvs-x63.270/) for no reason. I wanted to find out if a newer version could boot from NVMe, maybe it’ll work I thought... by some miracle, and it did not 😅 If you still want to do so, still at your own risk, I used FreeDOS if that helps.

Now the USB stick: I used [rEFInd](https://www.rodsbooks.com/refind/), some good documentation could be found on [Arch Linux's Wiki](https://wiki.archlinux.org/title/REFInd).

Voila what it looks like (I used gpart if memory serves me right, fdisk will also do the trick):
```
Disk /dev/sde: 1.86 GiB, 1992294400 bytes, 3891200 sectors
Disk model: UDisk
Units: sectors of 1 * 512 = 512 bytes
Sector size (logical/physical): 512 bytes / 512 bytes
I/O size (minimum/optimal): 512 bytes / 512 bytes
Disklabel type: gpt
Disk identifier: 02152992-33DF-4747-9E04-F7AB6A20B0BB

Device     Start     End Sectors  Size Type
/dev/sde1   2048 1048575 1046528  511M Microsoft basic data
```

Then something along these lines:
* `refind-install --usedefault /dev/sdXY --alldrivers`
* Mount the device and remove unnecessary drivers (none x64 directories and efi boot files)
* Download [clover](https://github.com/CloverHackyColor/CloverBootloader), extract its content and copy `NvmExpressDxe.efi` to `drivers_x64` (mentioned on rEFInd's [own documentation](https://www.rodsbooks.com/refind/drivers.html))
* Create a directory named `TrueNAS` containing `grubx64.efi` (I copied it from another TrueNAS's installation) and `grub.cfg` containing:
```
search --label "boot-pool" --set=root
configfile /grub/@/grub.cfg
```
* Plug the stick in any USB port on the NAS
* Reach BIOS again, change boot option from legacy to UEFI, no drive selected
* There should be a single option (thanks to `grubx64.efi` above), hit enter and the system should boot
* If not some debugging will be needed, good luck 🍀🤞🏼😉

If it helps, here's the directory structure on the stick:
```
./EFI
./EFI/BOOT
./EFI/BOOT/drivers_x64
./EFI/BOOT/icons
./EFI/BOOT/icons/licenses
./EFI/BOOT/keys
./EFI/BOOT/vars
./EFI/TrueNAS
```

Once in the system, you can add a `modprobe f71882fg` as pre-init script in init/shutdown scripts, that provides `sensors` with way more information about the different voltages, fan(s) speed, temperatures and power.

**Hardware**  
Now, what about the front LEDs (Status, USB & drives), buttons (rear reset, front usb copy) and the LCD screen.

I couldn't realistically link every forum post or github page/gist I went through, but [only one](https://gist.github.com/zopieux/0b38fe1c3cd49039c98d5612ca84a045) really helped. All of them though, just made me realize that what works for everyone, unfortunately didn’t work for me.  
To date, I still haven't found a single page explaining any of the below. I may be wrong but I did my homework, using Google and ChatGPT to no avail. The latter though was my best friend during the investigation, nothing would have been possible without the back-and-forth done during a few days.  
⚠️ That said don't forget, AI may be wrong, so double-check anything that you will copy/paste and execute on your device, check reliable sources. Also YMMV depending on the model, it worked for me.

Without further ado: Fintek F71869A Super I/O  
Every single LED, button pushed and LCD interaction is driven or goes through this chip.  
To be honest this part goes beyond my area of expertise and that's where ChatGPT shined.  

I haven't found much information about LEDs and buttons on the web, however LCD interaction has been documented a few times. But as said earlier, it never worked for me as describe. I cracked open the NAS, hooked my oscilloscope on that port onto which the LCD is connected (saying COM2 LCM), ran a few commands against `/dev/ttyS1` to no avail — not a single waveform, square wave, nothing...

I’ll spare you the full thought process, and the time it took to determine that the Fintek chip was guilty all the time. QTS initializes everything through their own kernel modules, HAL binaries whatever, but nothing is documented, as far as I could see. So I took a mix of `/etc/model.conf` information on QTS, c code to dump LDN's and registers, and compared everything between QTS and TrueNAS. A loooooooooong whiiiiiiile later, I think I found what needs to be configured to get this working.

In this repository you will find:
* `initialize_f71869.c` - directly accesses the Fintek chip using legacy I/O port commands. It enters the Super I/O’s configuration mode, selects the logical device number (LDN 0x06) probably corresponding to the GPIO / System I/O block at base 0x0A00, and configures two registers (CR2A and CR30) that control GPIO-related functions such as buttons, LEDs, and COM2-related signals (ttyS1).
* `buttons.c` - reads button states by directly accessing runtime GPIO registers. For each button, it reads a specific register (e.g., I92, IE2) and extracts a single bit that represents that button’s state.
* `leds.c` - writes to the runtime GPIO registers to control LEDs bit-by-bit. Given an LED name, it looks up the corresponding I/O register (e.g. I91, I81, IE1) and bit position, reads that register via the SYSIO index/data ports (0xA05/0xA06), sets or clears the selected bit according to the 0/1 argument, and writes the modified value back—turning that specific LED on or off.
* `A125_User_Manuel_revise_04.pdf` - describing how LCD works and how to interact with it. Basically using `stty` and `printf` as described in pages such as https://github.com/elvisimprsntr/QnapFreeLCD or https://github.com/stephenhouser/QnapLCD-Menu

All of them were compiled with `gcc -static -O2 -o <NAME> <NAME>.c`

Regarding the buzzer, there's an [old repository](https://github.com/johnath/beep) providing a tool allowing to interact with it.
