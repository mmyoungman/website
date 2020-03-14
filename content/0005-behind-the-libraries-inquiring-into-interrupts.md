Inquiring into Interrupts -- Behind the Libraries Part Four
=

21st October 2018
-

In the previous article, while we managed to use timer1, we also failed to accurately measure time with it. This is a problem because we want our replacement for the delay function to be accurate to a millisecond like the default Arduino delay function. To acheive greater accuracy, we are going to use interrupts.

### What are Interrupts?

Interrupts stop the Arduino doing whatever it is doing so it can perform a higher priority task. Once that task is completed, the Arduino resumes doing whatever it was doing before. So if your program does "a = b + c;" and then "b = c + d;", but an interrupt occurs between the execution of these two lines of code, "a = b + c;" will run, then the interrupt will trigger and execute whatever commands it is set to run, and only once that has finished will "b = c + d;" run.

As a more general example, you might create an interrupt that is triggered by a motion sensor. Once triggered, it might take a picture from a camera. If we didn't have interrupts, our program would instead have to constantly ask and reask the motion sensor whether it has detected something. This is expensive in both time and power. With interrupts, the motion sensor lets us know when it detects something, meaning we can do other tasks while we wait or even sleep to save energy. 

It also means we can respond quicker to the motion sensor -- which is the important feature for us when producing our alternative delay function, because we want a delay of a precise period of time. We know how to measure time using one of the Arduino's timers, but now we want to respond quickly when the timer indicates that that amount of time has passed, so that we don't add extra time and make our delay function imprecise.

Firstly, we'll tackle how to respond quickly to the timer. Previously, we constantly polled the value of TCNT1 to check whether it had gone beyond a certain value.:

> if(TCNT1 > 16000) {
>
> &nbsp;&nbsp;// Do stuff
> 
> &nbsp;&nbsp;TCNT1 = 0; // reset timer1
>
> }

Now we want timer1 to do this checking for us. One way we can avoid constantly polling TCNT1 is have timer1 trigger an interrupt every time timer1 overflows:

> void setup() {
>
> &nbsp;&nbsp;Serial.begin(19200);
>
> &nbsp;&nbsp;// Setting waveform generation mode 0
>
> &nbsp;&nbsp;TCCR1A &= 0b11111100;
>
> &nbsp;&nbsp;TCCR1B &= 0b11100111;
>
> &nbsp;&nbsp;// Setting 1024 prescaler
>
> &nbsp;&nbsp;TCCR1B &= 0b11111101;
>
> &nbsp;&nbsp;TCCR1B |= 0b00000101;
>
> &nbsp;&nbsp;// Set TIMER1\_OVF interrupt on
>
> &nbsp;&nbsp;TIMSK1 |= 0b00000001;
>
> &nbsp;&nbsp;volatile uint16\_t count = 0;
>
> }
> 
> void loop() {
> &nbsp;&nbsp;Serial.print("Count: ");
>
> &nbsp;&nbsp;Serial.println(count);
> 
> }
> 
> ISR(TIMER1\_OVF\_vect) {
>
> &nbsp;&nbsp;count++;
>
> }

There are few new things here to explain. 

> TIMSK1 |= 0b00000001; // Set TIMER1\_OVF interrupt on

TIMSK1 is the register that allows us to turn interrupts on or off that are associated with timer1. In this particular program, we set the bit that turns timer1's overflow interrupt on. Every time TCNT1 ticks from 65535 to 0, this interrupt is triggered.

> volatile uint16\_t count = 0;

The volatile keyword tells the compiler to not assume that count doesn't change. This can be necessary because interrupts can change variables in ways that the compiler isn't expecting. When compiling our code into something that the ATmega328p understands, the compiler may make the assumption that the count variable doesn't change, since loop() doesn't call any other functions that affect count. 

Having compilers make such an assumptions can be good, as it often makes the final program run more efficiently. When using interrupts, however, this assumption can be incorrect.

> ISR(TIMER1\_OVF\_vect) {

ISR stands for interrupt service routine. This is where we put the code we want to run when the associated interrupt is called. TIMER1\_OVF\_vect, as its name suggests, refers to timer1's overflow interrupt.

### But What's Happening on the Hardware?

While the above code makes the Arduino Uno do what we want, it unfortunately hides some of the details of how interrupts work on the underlying hardware. On the ATmega328p, there are 26 interrupts in total -- they are listed on p82-3 of the datasheet. Each individual interrupt has a specific piece of memory associated with it called an interrupt vector. Any program command stored there will be executed when its associated interrupt is triggered. Since the vector is small, it usually holds a command to jump to a larger set of commands stored elsewhere -- those commands actually do whatever it is we want to do when that interrupt is triggered.

You can see how much of this complexity is hidden in our previous code. I believe the compiler automatically puts a command in the TIMER1\_OVF interrupt vector that jumps to the commands inside our interrupt service routine. (We can actually check this, but this isn't something I want to explore until later in this series.)

### Global Interrupts Enable/Disable

One important consideration when using interrupts is whether they could introduce bugs into your program if they occurred at the wrong time. For example, let's say within loop() we have the code "uint16\_t previousCount = TCNT1;". While that is a single line of code, as previously discussed, TCNT1 is actually stored on two 8-bit registers, TCNT1H and TCNT1L. This means that this single line of code actually requires two commands on the hardware. Essentially, the hardware must do "uint8\_t prevCountHigh = TCNT1H;" and "uint8\_t prevCountLow = TCNT1L;". If an interrupt fired between these two commands _and_ the interrupt changed TCNT1, then we might end up with one half of previousCount being the old value of TCNT1, and the other half being the new value.

For this reason, the ATmega328p offers the option to temporarily disable all interrupts. This can be done by changing the 7th bit in the ATmega328p's Status Register (SREG). We can clear or set this bit with "cli();" (clear global interrupt flag) and "sei();" (set global interrupt flag). To fix the above problem, we would call disable interrupts with "cli();", then run "uint16\_t previousCount = TCNT1;", and then enable interrupts again with "sei();"

### Precisely Counting Time

For our delay function, we want it to accept an argument in milliseconds and then pause for precisely that amount of time. With our previous program, timer1's overflow interrupt was triggered 16000000 / 1024 / 65536 = 0.24 times per seconds or once every 4.17 seconds. This isn't useful for our delay function, since there is no easy way to measure milliseconds. For convenience, what we want is for an interrupt to trigger every millisecond, which will make it much easier for us to write our own delay function.

Thankfully, timer1 has another "waveform generation mode" we can use called CTC or clear timer on compare. If we set this mode, instead of it counting up to the maximum value of 65535, we can instead specify the value it counts up to. Also, as the name of the mode suggests, the ATmega328p will automatically set the timer to zero when it hits that value, which helps keep the timing as accurate as possible, as we don't have to spend time doing it ourselves.

Given that the Arduino's crystal ocsillator runs at 16MHz, for convenience we'll have the interrupt trigger on the value 16000. This means that if we don't use a prescaler, the timer will count to 16000 in precisely 1 millisecond. To set timer1 so it only counts up to 16000, we need to set OCR1A to 16000. OCR1A stands for Output Compare Registers 1 A. Similar to TCNT1, OCR1A is a 16-bit value which is stored in two 8-bit registers, OCR1AH and OCR1AL.

Since timer1 no longer overflows back to zero, the timer1 overflow interrupt no longer works for us. So instead we'll use the timer1 compare match A interrupt. This interrupt triggers when the value of TCNT1 matches the value of OCR1A.

> void setup() {
>
> &nbsp;&nbsp;pinMode(13, OUTPUT);
>
> &nbsp;&nbsp;
>
> &nbsp;&nbsp;// mode 4, CTC (Clear Timer on Compare)
>
> &nbsp;&nbsp;TCCR1A &= 0b11111100; 
>
> &nbsp;&nbsp;TCCR1B &= 0b11100111;
>
> &nbsp;&nbsp;TCCR1B |= 0b00001000;
> 
> &nbsp;&nbsp;TCCR1B &= 0b11111001;
>
> &nbsp;&nbsp;TCCR1B |= 0b00000001; // no prescaler
> 
> &nbsp;&nbsp;OCR1A = 16000; // COMPA interrupt called every 16000000/16000 = 0.01 seconds
> 
> &nbsp;&nbsp;//SET STATUS REGISTER SO GLOBAL INTERRUPTS ARE ENABLED
>
> &nbsp;&nbsp;//EVEN THOUGH IT IS NOT NECESSARY
> 
> &nbsp;&nbsp;TIMSK1 |= 0b00000010; // Set COMPA interrupt on
> }
> 
> void loop() {
>
> &nbsp;&nbsp;digitalWrite(13, HIGH);
>
> &nbsp;&nbsp;delay2(1000);
>
> &nbsp;&nbsp;digitalWrite(13, LOW);
>
> &nbsp;&nbsp;delay2(1000);
> }
> 
> volatile uint16\_t count;
> 
> ISR(TIMER1\_COMPA\_vect) {
>
> &nbsp;&nbsp;count++;
>
> }
> 
> void delay2(uint16\_t milliseconds) {
>
> &nbsp;&nbsp;count = 0;
>
> &nbsp;&nbsp;TCNT1 = 0;
>
> &nbsp;&nbsp;while(count < milliseconds);
>
> }

While we used the volatile keyword in the previous program as good practice, in this particular program the keyword is necessary. If you remove the keyword, the compiler looks at the delay2 function, assumes that checking "count < milliseconds" is pointless and can be skipped, and so it will get stuck in delay2's while loop forever.

And there you have it. We have made our own imaginatively-named replacement for the delay function. Next time, we'll look into how to do serial communication without using the Arduino's Serial library.