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
- Basic chart drawing with house cusps (Equal, Placidus, Koch, Porphyry, PorphyryNeo, Whole Sign, Campanus)
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
- [x] Sidereal time calculation
- [x] Delta-T calculation for historical dates

#### 1.2 Coordinate Systems
- [x] Ecliptic to equatorial conversion
- [x] Coordinate transformations
- [x] Horizontal coordinate system (altitude/azimuth)
- [x] Galactic coordinates

#### 1.3 House Systems
- [x] Equal house system
- [x] Placidus house system
- [ ] Implement additional house systems:
  - [x] Koch (`legacy/astro.h: vKoch`)
  - [x] Campanus (`legacy/astro.h: vCampanus`)
  - [x] Regiomontanus (`legacy/astro.h: vRegiomontanus`)
  - [x] Porphyry (`legacy/astro.h: vPorphyry`, `vPorphyryNeo`)
  - [x] Whole Sign (`legacy/astro.h: vWhole`)
  - [x] Topocentric (`legacy/astro.h: vTopocentric`)
  - [x] Meridian / Axial (`legacy/astro.h: vMeridian`)
  - [x] Morinus (`legacy/astro.h: vMorinus`)
  - [x] Equal (MC) / Equal Mid (`legacy/astro.h: vEqualMid`)
  - [x] Alcabitius (`legacy/astro.h: vAlcabitius`)

Implementation plan for house systems:

- __[API design]__ Add a `HouseSystem` enum and an `astrocore` API to compute 12 cusp longitudes and angles for a given date/time/location and system. Return structured data with MC/ASC and validity flags (for high-latitude failures).
- __[Algorithm sources]__ Port or reimplement algorithms following the legacy signatures listed above in `legacy/astrouni.h` as references. Validate mathematics against known references for each system.
- __[Edge cases]__ Handle high latitudes and dates where Placidus/Koch methods fail. Provide fallbacks (e.g., switch to Porphyry or report N/A) and surface warnings in the GUI status bar.
- __[Normalization]__ Ensure cusps are normalized to [0, 360) and monotonically increasing per system rules, accounting for intercepted signs and wrap-around.
- __[Precision]__ Use double precision with robust trigonometric handling; clamp numeric domains to avoid NaNs at extreme declinations and refractions.
- __[Testing]__ Add unit tests covering multiple latitudes (0°, 45°, 60°, 66.5°, 70°), both hemispheres, and random dates. Include regression fixtures derived from the legacy implementation for each system.
- __[GUI integration]__ Extend the existing house system selector to list all supported systems and refresh the chart upon change. If computation invalid, display a non-fatal notice and keep previous valid cusps highlighted.
- __[CLI integration]__ Add a `--house-system` option accepting all enum values; print cusps as a table for verification.
- __[Documentation]__ Document formulas, references, and limitations per system in Developer Docs, with links to tests.

#### 1.4 Planetary Calculations
- [x] Basic planetary positions (longitude/latitude)
- [x] Planetary speeds and retrogrades
- [ ] Planetary stations (exact retrograde/direct points)
- [ ] Lunar phases and eclipse calculations
- [ ] Planetary nodes and apogee/perigee

#### 1.5 Aspect Calculations
- [x] Basic aspect detection (conjunction, opposition, trine, etc.)
- [x] Aspect orb handling
- [ ] Aspect pattern detection (T-square, Grand Trine, etc.)
- [ ] Applying/separating aspect classification
- [ ] Parallels and contra-parallels

### 2. GUI Implementation

#### 2.1 Chart Display
- [x] Basic wheel rendering
- [x] Planet glyphs and positioning
- [x] House cusps and divisions
- [x] Aspect lines
- [ ] Customizable chart styles and colors
- [ ] Multiple chart types (natal, transit, synastry)

#### 2.2 Data Input and Management
- [x] Date/time input with timezone
- [x] Location input (longitude/latitude)
- [ ] Atlas integration for city lookup
- [ ] Chart data persistence (save/load)
- [ ] Chart comparison tools

#### 2.3 Analysis Tools
- [x] Aspect grid display
- [ ] Planet and house position tables
- [ ] Aspect listing with interpretations
- [ ] Transit forecasting
- [ ] Progressions and directions

### 3. Command Line Interface

#### 3.1 Basic Functionality
- [x] Chart calculation from command line
- [ ] Text-based chart output
- [ ] Batch processing of charts
- [ ] Ephemeris generation

#### 3.2 Advanced Features
- [ ] Scriptable chart analysis
- [ ] Data export (CSV, JSON)
- [ ] Integration with other tools

### 4. Documentation and Testing

#### 4.1 Developer Documentation
- [ ] API documentation
- [ ] Algorithm explanations
- [ ] Code examples

#### 4.2 User Documentation
- [ ] User manual
- [ ] Tutorial guides
- [ ] Reference materials

#### 4.3 Testing
- [ ] Unit tests for core calculations
- [ ] Integration tests
- [ ] Accuracy validation against standard ephemerides
- [ ] Performance benchmarking

### 5. Future Enhancements

#### 5.1 Advanced Techniques
- [ ] Harmonic charts
- [ ] Arabic parts
- [ ] Fixed stars
- [ ] Asteroids and minor bodies

#### 5.2 Predictive Tools
- [ ] Solar returns
- [ ] Lunar returns
- [ ] Solar arcs
- [ ] Primary directions

#### 5.3 Additional Bodies
- [ ] Pluto position calculation
- [ ] Chiron and asteroid calculations
- [ ] Node calculations
- [ ] Lilith and hypothetical points
- [ ] Transneptunian objects

#### 5.4 User Interface Enhancements
- [ ] Advanced chart styling options
- [ ] Multi-chart workspace
- [ ] Customizable report generation
- [ ] Mobile device support
- [ ] Font selection
- [ ] Glyph style selection

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
