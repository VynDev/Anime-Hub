# Changelog

Versions starting with a 0 are considered as "In development"

## [0.3.0] - 2020-01-[?]

This version aim to provide a better support for AniList and a more stable software.

### Added

- AniList Import (with just an username).
- "About" window (Author, version, license, creadits, etc.).
- Removing anime from a list from the context menu (right click).
- Deleting list.
- Error messages (simple atm).

### Changed

- Anime preview UI has now better formatting

### Fixed

- Design pattern
- Memory leaks

## [0.2.0] - 2020-01-13

### Added
- Dark theme
- Settings: Paths, cache, images fetching, theme

## [0.1.0] - 2020-01-10

### Added
- Right click context menu on animes search results to add them directly to the list of your choice.
- "Episodes", "Status", "Genres" and "Date" fields to the anime preview widget.
- Automatic save/load lists into json format.

### Fixed
- Unicode characters are now displayed correctly.
- "\r\n" are now properly removed from description.

## [0.0.1] - 2020-01-05

Full setup of Qt

### Added
- Create lists (UI & Back-end).
- Fetch animes from Anilist and display title & synopsis (UI & Back-end).
- Fetch animes image for searchs and lists.