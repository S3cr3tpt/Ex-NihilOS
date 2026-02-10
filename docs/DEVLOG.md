Day 1 white pixel done:
So in this day i finnaly made an os that just boots up and gives me a white dot i will keep testing but not add notes because its just fun tests like changing the color and position, this is the first step out of many in this journy

Day 1 the black screen appeared:
Ok now it worked and a black screen appeared that means that the video was initialized and now i just have to write the white pixel by giving it the white value with the offset for it to know its in the graphical part
And after some digging and knowing that im working with 16 bits i cant direclty ask for the 32k but i need some math so i will need some registers and i need to add them first into a register before making it into the segments and i will need to put the ax to 0xA000 the initial part of the video segment and then put it into the es so its in a regiter and after that i need to put the 32k as the offset so i will put in the DI 32k and only after that i will put them thoguer and use the 0x0f to make it white
and i found the wait instruction its hlt so ill put it before the jm $ so it dosnt waste cpu cycles

Day 1My first error:
When i tried the first code i just wrote it and it didnt do nothing i reaserched and i found that it dosnt stay loaded i needed to write the jmp $ to keep it alive in an infinite loop but for me it seems like it will use all of my CPU just running it infinitely, so i am going to search if there is a sleep call that i can make, and also i forgot to add the signature in the end so i need to just fill everything else with zeros and in the very end add the 0xaa55 signature and knoing its 512 bytes i need to write it for 510 and the last 2 bytes are that and writing it after the jmp $ it wont ever run but it will store the signature correctly

Day 1: Creating the first white dot
To create the first white dot i needed to find how to i set it to video mode and to set it to VGA, and i found that in this assembly i need to use AH and set it to 0x00 to set the video mode and AL and set it to 0x13 (the VGA with 256 colors) and after that to call it i need to use int 0x10 but before any of that i need to specify that im iin the 0x7c000 or it will just crash because its impied that im in that position but the assembly dosnt know that so i need to say it by using [org 0x7c00] that just tells it that the ofset is the 0x7c000

Day 1: Creating the Boot file:
Now i know more or less how the startup goes from the bios it checks 512 bytes in the storage and looks for a signature in the last 2 bytes that are 0x55 and 0xAA and if its there then it jumps to 0x7C000 and then my code starts in that memory address, and to put the white dot in the midle of the screen i needed to know how to write it, but its easier thatn i tought, every pixel is stored from 0xA0000–0xBFFFF and the midle of the screen is at the 32000 byte more or less so i just need to put the 0xA0000 plus the 32000 byte ofset
Also I created a scipt int he root directory to make my life easier and just compile everything and run the emulator

Day 1: Booting Sequence Initiation:
For the first day i just wanted something that boots up and shows a white dot on screen for the future booting up sequence;
And for now i know almost nothing on how making an Operating system works but i reaserched and i will now create the file structure first to keep everythig clean

Day 0: Repository Created. The void is open.
Visual Target (The Vision)
**Boot Sequence:**
1. State: Total Void (Black Screen).
2. Event: A single white point appears in center (The Singularity).
3. Action: Point ripples outward, expanding into the Interface.

**The Tiling Interface (TUI) Layout:**
-------------------------------------------------------
|  [ CPU / RAM / BATT ]  |      [ TERMINAL ]      |   [ FILES ]   |
|     (System State)     |      (God Mode)        |   (Context)   |
|                        |                        |               |
-------------------------------------------------------
|                  [ VIRTUAL KEYBOARD ]                   |
|               (Visualizer / Macro Map)                  |
-------------------------------------------------------