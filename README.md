# embeddable-chip8
Chip8 emulator which doesn't require the need for any libraries. 


## How to use?

```c
Chip8 chip8 = chip8_create(/* Fetch your ROM somehow. */ ROM);
chip8_setup (
  &chip8,
  /* callback for the rand function. */rand,
  /* callback for the log function.*/log,
  /* callback for the beep function.*/beep
);

chip8_emulate(&chip8);
/* To be able to use keys: */
for (int i=0;i<16;i++){
  if (key[i] == PRESSED) chip8.keys[i] = 1;
  else if (key[i] == RELEASED) chip8.keys[i] = 0;
}


if (chip8.flags.draw) {
// Draw the pixels by using the chip8 framebuffer (chip8.framebuffer)
chip8.flags.draw = 0;
}
```
