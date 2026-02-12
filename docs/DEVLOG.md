Day 3 testing what i made:
first i forgot to pop all of the registers and that whould break the code, but seccond after i wrote everthing i tought was right, i got the botting up sequence going crazy just giving the same error over and over again... i just found the but as i was writing, when i did cntr+s to save it didnt read trhe cntr button and added an "s" to the return part in the handler so it broke everythingand now its working and im getting the blue dot when i give the divide by zero interrupt
Im going to explain what happen, because i found it funny so i forced and devide by zero interrupt and as i had a typo it got another interrupt (interrupt 6) but as i was just testing the IDT, i just had the handler for the zivide by zero and ass i didnt have it it double faulted, and as i dont have anything to handle that i got my first triple fault by just having a typo in the return of a function

Day 3 how am i implementing the IDT:
First of all i just understood what it is i have to reashearch more to find how to implementt it
Ok i need even before that to write an interrupt service routine, it makes sense because i was confused the IDT is just saying oh the handler is here but i didnt have it i needed to write the ISR, at first im going to write a simple one that just catches everything that makes another blue dot in the screen because a full ISR has 2048 bytes and right now im workign with 512 because its jut the boot sector and the way im going to do it is im going to manually give an error the division by zero and write the blue dot onto the screen

Day 3 the IDT:
Once i got to the 32 bit i conntinued not knowing whats the next step because im doing this project step by step each day i want to learn something and implement it, and after some reasearch on how to get to 64 bit, i found that even before i turn the 64 bit on i need and IDT, and what is that, i have no clue im going to search it now
OK i now know the name of it and it is interrupt description table and form the name im guessing its like the gdt but insted of segmenting the memory and saying what the "laws" are it just says what the cpu must do in each interrupt
And i have to write the handler for 3 different interrupts like exeption, that it internal issues like the cpu trying to divide by zero, hardware interrupts, that is Keyboard pess what do i do, oh go to the memory adress of the keyboard driver and softwrare interrupts, its the programs asking for things, if the linux askes 0x80 (Linus way to open a file) i need to switch to kerner mode

Day 2 32bit test:
The test i am going to do is just a red dot created with the 32 bit OS and for that i need to flush the CPU by jummping into the 0x08 that is just the far right of the 32 bit, if it dosnt work then i know i did something wrong
Ok and i got an error i tryed to jump back to the freeze on the 16 bit, because i didnt realise that when i jumped to the init_32bit i flushed everything and that is why i needed to put that jump 0x08 right after the initialization of the 32 bits because it cant be working on both at the same time so now i have a white dot in the midle made in 16 bit and right next to it i have a red dot made in 32 bit and the 0x08 is a value on the gdt table thatt makes it the offset of the 0x08 that is the specific size defined in the gdt
Also i didnt realise that i had one mistake in the begining that was i was using a 16 bit register it worked because the assembler knew that i wanted the 32 bit and i could just leave it there but i wanted to change it because i am using the 32 bit from now on (it was the mov eax 0xA000) and i want to keep the code clean and not really on the arbitrary rules of the assembler because i am the one that is making the rules in this os and if i left it there i was adding 1 byte to and cpu cycles just to handle a small number into a small register for no reason

Day 2 gdt error:
As i was expecting my gdt didnt work at the first try but after some digging i saw that i had a few linses that are important because i wrote the gdt but the cpu dosnt really know where in memory it is so i need to add the lines to make sure that the CPU knows where it is when using the lgdt (Load Global Description Table) and it needs to have the a header so the it knows where in memory it is after this i think everything should work and i will test it 

Day 2 making the building block for 32 bit OS:
Ok i have been researching while I was away from my pc, and now i wanted to change to 64bit but from what i learned i cant just jump from 16-64 because there are requirements, first i needed to go to 32 bit by making a gdt table, it took me a while to get what it was but my description is that the gdt is the 2 particles that created the universe in a sense that there is one byte in that table that makes it change from 16 to 32 bit, and from what i understood i need to flush everything in the cpu in the next intruction because it cant have instructions in 16 bit and 32 bit at the same time, and the values are some static values like the firssst 8 bytes are just 0 to catche null pointer error, and then i set the limit for the adress size, and then we start the addresss segment at 0 and then i set the base midle also to 0 because the 16 bit cpu had only 24 address lines and thats why we need  to put it split, then we need to put the static bytes for the access that is just sayyng if the semgent is falid the priveledge if its data or code and not a segment if its code or data the direction, so its only one exact level (previlege) level can execute it if it writable or readable and if iitts accessed, then the finaly byte and here is where the "magic" is happening its where i change it to be 32 bitess, also i had an AI comment the lines so if i ever foguet what is there i know by just reading the code, its easier than writing overcomplicated comments there that i will forguet what i was trying to say in a week

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