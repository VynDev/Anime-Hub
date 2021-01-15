**In development!**  
  
Keep your anime lists on your computer with this desktop software.

*Keep in mind this is currently a personnal project for learning purpose (C++/Qt), while I really want to finish it and do something nice, it might not be useful for everyone, see the goals of the project below*

## Goals of the project

- Having your anime lists on your computer, thus you can access it without a connection and doesn't depend on a third party service.
- Search animes online (through API like Anilist), that provide information (titles, cover image, synopsis, genres, etc.) about them.
- Manage multiple lists (watched, planned, etc... You can create the lists you want).
- Add animes to your lists directly from a search, thus you don't have to add the title, synopsis, image or whatever yourself.

### Optional features

- Share your list with your friends with a simple P2P transfer, you'll have a copy of their list on your computer
- Cache image and useful data for offline use.

## Presentation

![presentation](https://github.com/VynDev/Anime-Hub/blob/main/images/presentation.gif)
![presentation styles](https://github.com/VynDev/Anime-Hub/blob/main/images/presentation_styles.gif)

## Build & Run

### Requirements

- git
- g++ (developed with g++ (GCC) 10.2.0 on Arch Linux)
- cmake
- premake5 (used by dependencies)
- make
- Qt6

### Steps (Linux)

In the project folder:
```
git submodule update --init --recursive
cmake
make
```

## License

GPLv3 (see the file "LICENSE" at the root)