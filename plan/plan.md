# Astrology wxWidgets GUI Implementation Plan

## Overview

This document outlines the comprehensive plan for implementing a full-featured astrology application using wxWidgets GUI framework. The implementation will port all functionality from the legacy astrology codebase while providing a modern, user-friendly interface.

## Core Architecture

The application follows a layered architecture:

1. **Core Library (astrocore)**: C++ library providing astronomical calculations
2. **GUI Layer (apps/gui)**: wxWidgets-based interface for chart display and user interaction
3. **CLI Layer (apps/cli)**: Command-line interface for scripting and batch processing

## Current Status

The following components have been implemented:
- Basic chart drawing with house cusps (Equal and Placidus systems)
- GUI menus (File, Chart, View, Help) with handlers
- Date/time/timezone input dialog
- Location input dialog
- Aspect detection and visualization
- Chart parameter controls (house system, aspect orb)
- View toggles (axes, labels)
- PNG export

## Feature Implementation Plan

### 1. Core Astronomy Calculations

#### 1.1 Time and Calendar Functions
- [x] Julian Day calculation from local civil time
- [x] Timezone handling
- [ ] Sidereal time calculation
- [ ] Delta-T calculation for historical dates

#### 1.2 Coordinate Systems
- [x] Ecliptic to equatorial conversion
- [x] Coordinate transformations
- [ ] Horizontal coordinate system (altitude/azimuth)
- [ ] Galactic coordinates

#### 1.3 House Systems
- [x] Equal house system
- [x] Placidus house system
- [ ] Implement additional house systems:
  - [ ] Koch
  - [ ] Campanus
  - [ ] Regiomontanus
  - [ ] Porphyry
  - [ ] Whole Sign

#### 1.4 Planet Calculations
- [ ] Sun position calculation
- [ ] Moon position calculation
- [ ] Mercury position calculation
- [ ] Venus position calculation
- [ ] Mars position calculation
- [ ] Jupiter position calculation
- [ ] Saturn position calculation
- [ ] Uranus position calculation
- [ ] Neptune position calculation
- [ ] Pluto position calculation
- [ ] Chiron and asteroid calculations
- [ ] Node calculations

#### 1.5 Aspect Calculations
- [x] Basic aspect detection (conjunction, opposition, trine, square, sextile)
- [ ] Extended aspects (quincunx, semi-square, etc.)
- [ ] Aspect pattern detection (grand trine, T-square, etc.)
- [ ] Aspect strength/applying/separating logic

### 2. GUI Implementation

#### 2.1 Chart Display
- [x] Basic wheel drawing with house cusps
- [x] Ascendant and Midheaven display
- [ ] Planet glyphs rendering
- [ ] Zodiac sign glyphs
- [ ] House numbering
- [ ] Aspect grid display
- [ ] Aspect lines with appropriate styling
- [ ] Chart title and information display

#### 2.2 User Interface
- [x] Main menu structure
- [x] Date/time input dialog
- [x] Location input dialog
- [ ] Chart type selection (natal, transit, synastry, etc.)
- [ ] Chart style customization
- [ ] Color scheme selection
- [ ] Font selection
- [ ] Glyph style selection

#### 2.3 Chart Types
- [ ] Natal chart
- [ ] Transit chart
- [ ] Synastry chart
- [ ] Composite chart
- [ ] Solar return
- [ ] Lunar return
- [ ] Progressed chart

#### 2.4 Data Management
- [ ] Chart data saving/loading
- [ ] Location database
- [ ] Timezone database
- [ ] User preferences persistence
- [ ] Recent files list

### 3. File Handling

#### 3.1 Native Format
- [ ] Define native chart file format
- [ ] Save chart data
- [ ] Load chart data
- [ ] Chart data validation

#### 3.2 Legacy Data Files
- [ ] Parse astroeph.dat (ephemeris data)
- [ ] Parse astronam.dat (astronomical names)
- [ ] Parse astroger.dat (German translations)
- [ ] Parse astroini.dat (initialization data)

#### 3.3 Export Formats
- [x] PNG image export
- [ ] PDF export
- [ ] SVG export
- [ ] Text report export

### 4. Advanced Features

#### 4.1 Interpretations
- [ ] Basic planet in sign interpretations
- [ ] Planet in house interpretations
- [ ] Aspect interpretations
- [ ] Chart pattern interpretations

#### 4.2 Calculations
- [ ] Midpoints
- [ ] Arabic parts
- [ ] Fixed stars
- [ ] Harmonics
- [ ] Declinations and parallels

#### 4.3 Predictive Techniques
- [ ] Transits
- [ ] Progressions
- [ ] Directions
- [ ] Solar arcs
- [ ] Returns

### 5. Performance and Optimization

- [ ] Optimize planet calculations for speed
- [ ] Cache frequently used calculations
- [ ] Optimize chart drawing for smooth rendering
- [ ] Memory usage optimization

### 6. Testing and Validation

- [ ] Unit tests for core calculations
- [ ] Validation against published ephemerides
- [ ] GUI testing
- [ ] Cross-platform testing

## Module-by-Module Implementation Sequence

1. **Core Astronomy Module**
   - Complete planet position calculations
   - Implement all house systems
   - Complete aspect calculations

2. **Chart Drawing Module**
   - Implement planet glyph rendering
   - Add zodiac sign glyphs
   - Implement aspect grid

3. **File I/O Module**
   - Implement .dat file parsing
   - Create chart data persistence

4. **Chart Types Module**
   - Implement different chart types
   - Add chart type selection UI

5. **Advanced Features Module**
   - Add interpretations
   - Implement predictive techniques

6. **UI Enhancements**
   - Add customization options
   - Implement preferences dialog
   - Add keyboard shortcuts

## Dependencies

- wxWidgets 3.2.8
- C++20 standard
- CMake build system
- astrocore library (internal)

## Conclusion

This implementation plan provides a roadmap for developing a complete astrology application with wxWidgets. The modular approach allows for incremental development and testing, with each component building on previously implemented functionality.
