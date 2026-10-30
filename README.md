# Toy Machine

A physics and object simulation game.

## Building & Running

### Dependencies

Requires CMake, Ninja, SDL2 and SDL2_image.

On Fedora: `sudo dnf install gcc cmake ninja-build SDL2-devel SDL2_image-devel`

On Ubuntu/Debian: `sudo apt install build-essential cmake ninja-build libsdl2-dev libsdl2-image-dev`

### Build & Run

```bash
make run
```

There is currently a developer console implemented in the game, you can access it by pressing F1 in the game window.  
Available commands:
- `help` - help message with commands and currently implemented emojis
- `spawn` - spawn an emoji at the given coords (e.g. `spawn soccer_ball 50 80`)
- `speed` - set world simulation speed (e.g. `speed 100`)
- `hitboxes` - enable/disable hitboxes (e.g. `hitboxes on/off`)

You can drag the emojis around but they do not do anything for now.

## Contributing
If you are Jeremiah or Simon, you should read this.  

PLEASE FUCKING USE BRANCHES I BEG YOU HOLY FUCKING SHIT IT IS NOT THAT HARD

There are currently branches for GUI, Developer Console and the implementation for the mouse emoji.  
I recommend creating a new branch for physics if you're working on that, or world, depends on what you're working on.
PR your changes in after a feature change/fix/create but honestly it doesn't matter if it compiles

Create a new branch for emoji implementations, one for the base emoji behaviour and then new branches for emojis with custom behaviour, like the fan, or the computer mouse (which I am already working on), I recommend creating a PR once the implementation is complete, but you can PR it in as long as the project compiles.

For things like documentation changes and updating scripts, you can directly push to master 

## Utilities

`./add_emoji.sh` can be used to quickly download emojis:

```bash
# Downloads Black Sun with Rays and saves as `assets/emojis/sun.svg`
./add_emoji.sh ☀️ sun
```
