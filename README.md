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

## Utilities

`./add_emoji.sh` can be used to quickly download emojis:

```bash
# Downloads Black Sun with Rays and saves as `assets/emojis/sun.svg`
./add_emoji.sh ☀️ sun
```
