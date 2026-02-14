Day 5 Frustration with my own stupidity:
i just spent the last hours debuggin a code that was working and yes the code from before worked from the first try, but i insted of using the correct emulator and flags i was using the wrong ones so it never could boot, i found out because i used an universal boot to 64 and it didnt work so i started checking my script and it had the wrong emulator, the good thing is that next time i know what to look for and im done for today i already have all of the pixels working

Day 5 Implementing everything slowly and loging the data:
so now im not going to put it al at once i first need to put the gdt in the end because both the 32 and 64 will use it, even if its just one bit that the 64 needs it still needs it, and i put it in the end and it still worked, now im going to change the gdt to work in 64 bites ok it still worked and for now its all fine
now im going to exable the PAE (physical address extention) and put it to long mode (64 bits) i enabled it and put another dot (cyan) after the other ones as a test to see if its working
Now i will make the jump to 64 bits, i will delete the freeze in the 32 bit and add a jump to the 64 and put another dot (purple) next to the other ones as a profe that im in, ok it broke but i think its because of the ISR i tried putting it before the gdt and it worked now im going to put the final 64 bit code ok it didnt work it broke im going to do some debuggin to see why, ok i found the line thats givving me an error and its this one "mov cr0, eax
" for some reason that i still dont understand
But for some reason this fells good because untill now it was all going too smoothly having this types of errors just proves that im building it from scrath


Day 5 another triple fault:
So i got one error that was the irs was still in 32 bits but it was loading after the 64 bits so i just changed it to be before the 64 bit code, and when i did that the cpu just died and tryed rebooting over and over again and it still, i dont know what i did but it broke everything im going to refetch the previus version and add things slowly this time to see when it breaks
Ok i fetched the previus version and for now it works, im going to search a  bit more on why the other one failed 


Day 5 Jump to long mode:
From what i understood this is basically the gdt again but insted of jumping to 32bits it jumps to 64, we do this basically like we did in the first gtd, we do it by giving the cpu a "handshake" that is basically a signature that says im prepared to go to 64 bits or another analogy is that its a lock that we need to provide the certain code to unlock the full power mode also i have to change my current gdt to the 64 bits i cant have 2 gdts at the same time

Day 4 testing thhe work:
I did it and i used 4 levels to keep it sanitized and safer and i am going to test it by writing a byte with a certain color in each memory and after that jsusst dysplaying the byte onto the screen and ill use AI for those tests for me not to write to the wrong addresses and now i should have it like this
White (16-bit Boot)
Red (32-bit Boot)
Blue (IDT Interrupt Fired)
Green (Level 4 Map Verified)
Yellow (Level 3 Map Verified)
Purple (Level 1 Hardware Map Verified)
and thats exacly what it gave me so thats all for today and im not sure if i fully understand what paging is because its a strange concept but i think in this devlog i have a good enough understanding of how it works and now the only thing i have left is the long mode and writing a full GDT in 64 bits finnaly also th elong mode i dont know what it is but tomorrow ill search it thats all for today, i know its going slow but if i tried to do this all in one day i could but i would forget to eat and drink so to keep me stable i need to do this by steps

Day 4 Implementation of paging:
as im still in stuck in 512 bytes i can affod to write the full version i just need one that meets the requirements
And for that im going to use the 4 tables for the levells the each with 4KB from Base and i cant just write directly i need to OR it to not break the system and becasue before anything is done  we are going to just zero it out after that to insted of garbadge data it has all zero if we did it after we where just going to erase the entire map

Day 4 Paging, the next step to almost 64 bits:
As usual i have no clue what it is because im learning as i go and this is no easy task, and fully understanding everything before i do something is not what i like to do, i want to see it break because of something i do to have a full undersanding of what i did
so paging its another requirement like the GDT but this time to go to 64 bits, and from what i understood its more to calibrate the memory because 64 bits is a lot so we use a "matrixes" like structure to say its in this row and this colum to not make the assessing of the memory slow, and it has another point that is making sure 1 memory address dosnt have 2 programs that think they are in the same spot and for that it uses "virtual memory addresses" 

Day 4 understanding the next steps:
when i was reashearching i dindnt know why the only thing i needed to do was just jumping bites like going from 16 to 32 and from that to 64 but now i understand its because all of the cpus are made to be backwards compatible so when it wakes up it thinks it is in the past and only at 16 bits, so we need to build the boot to say, hey you need to go to 64 bits, and all of that in 512 bytes  

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