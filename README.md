# schlossinator

The Schlossinator is a microcontroller-based doorlock using an Arduino, a servo motor, and a 4x4 membrane keypad.
4 leds give visual feedback and represent the states.

* 🔴 lock state:  in the lock state the door becomes locked and its represenred by a red glowing led.
* 🟢 unlock state:  in the unlock state the door becomes unlocked and its represenred by a green glowing led.
* 🔵 reset state: during reset state you are asked to enter a new password and confirm by pressing D. The reset state is represented by a blue glowing led. You open the reset state by pressing the button C.
* 🟡 check password state: before you are able to enter 🔴 lock state, 🟢 unlock state or 🔵 reset state the system will change into the check password state to verify the user. If the user gets the password wrong the yellow led will blink and the system will change into the previous state.

STL files used for this prject from Marcio Jose Soares https://www.thingiverse.com/thing:5831395

## 🛠️ Required Hardware

-  4 * 470 Ω resistors 

## 🔌 Wiring Diagram

## 🚀 Installation

Have fun assembling :D
