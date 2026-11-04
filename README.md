# Toy Machine

A physics and object simulation game.

## How to play
You can spawn emojis with the emoji picker (press space to open it) and click the emoji you want to spawn, each emoji has it's own attributes and behaviour.

Your primary progression method will be combining emojis, which will give you coins, acheivements will also give you coins and there will be other methods as well.

In order to combine emojis, you need to use the Factory emoji, put in items and it throws out your crafted item, 
you can also throw fast rocks on it to remove the item currently inside the factory.

You're given a few base emojis to start crafting new emojis, as you make new emojis, you can spawn those unlocked emojis via the emoji picker.

Currently there's only a bank emoji as a coin generator which when shaken drops a coin, this is slow and annoying though.

There is a vault system, where all coins get collected and stored, all transactions will occur via the vault

## Building & Running

### Dependencies

Requires CMake, Ninja, SDL2 and SDL2_image.

On Fedora: `sudo dnf install gcc cmake ninja-build SDL2-devel SDL2_image-devel`

On Ubuntu/Debian: `sudo apt install build-essential cmake ninja-build libsdl2-dev libsdl2-image-dev`


### Build & Run
This program can run on windows, I've added what would be needed for CMake to do so, but I haven't tested it yet.

```bash
make run
```

There is currently a developer console implemented in the game, you can access it by pressing F1 in the game window. 

Available commands:
- `help` - more detailed help message with commands
- `spawn` - spawn an emoji at the given coords (e.g. `spawn soccer_ball 50 80`)
- `vault` - view/modify vault balances (e.g. `vault add/spend 100`)
- `speed` - set world simulation speed (e.g. `speed 100`)
- `hitboxes` - enable/disable hitboxes (e.g. `hitboxes on/off`)
- `velocity` - enable/disable velocity vectors (e.g. `velocity on/off`)
- `unlock` - unlock a specific emoji (e.g. `unlock hammer`)
- `unlockall` - unlock all available emojis (e.g. `unlockall`)
- `exit` - exit the game (e.g. `exit`)

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
