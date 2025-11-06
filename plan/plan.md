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
- ✅ Core library (astrocore) with comprehensive astronomical calculations
- ✅ All major house systems (Equal, Placidus, Koch, Campanus, Regiomontanus, Porphyry, PorphyryNeo, Whole Sign, Topocentric, Meridian, Morinus, EqualMid, Alcabitius)
- ✅ GUI with wxWidgets: chart wheel, planet positions, aspect lines/grid
- ✅ German localization throughout the interface
- ✅ Complete menu system (File, Chart, View, Help) with handlers
- ✅ Date/time/timezone and location input dialogs
- ✅ Aspect detection with applying/separating classification
- ✅ Declination parallels and contra-parallels (core + CLI)
- ✅ Aspect pattern detection (Grand Trine, T-Square)
- ✅ Zodiac sign ring with Unicode symbols (♈..♓)
- ✅ Chart parameter controls and view toggles
- ✅ PNG export functionality
- ✅ CLI interface with comprehensive commands
- ✅ Extensive unit test coverage

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
- [x] Planetary stations (exact retrograde/direct points)
- [x] Lunar phases
- [x] Eclipse calculations
- [x] Planetary nodes and apogee/perigee
 
 Notes (DE):
- Implementiert: Knoten (mittlerer Mondknoten) und Apogäum (Lilith/mean apogee).
- Implementiert: Sonnen-/Mondfinsternisse (Heuristik um Syzygien; Klassifikation via ekliptische Mondbreite).
- Hinweis: Heuristische Implementierung; Verfeinerungen (Wirkungsbereich, Kontaktzeiten) möglich. Ephemeriden werden aus astroeph.dat gelesen.

#### 1.5 Aspect Calculations
- [x] Basic aspect detection (conjunction, opposition, trine, etc.)
- [x] Aspect orb handling
- [x] Aspect pattern detection (T-square, Grand Trine) (core + tests + CLI)
- [x] Applying/separating aspect classification (core + tests + CLI output)
- [x] Parallels and contra-parallels (core + CLI + tests)

Implementation plan for aspect calculations:

- __[API/Data model]__ Extend aspect result structure to include:
  - `applying`/`separating` flag, `exact_jd` (optional), `orb_deg`, involved bodies.
  - Distinct `AspectType` including parallels/contra-parallels.
- __[Applying/Separating]__ Implemented in `detect_aspect_with_speeds()` (core). Tests added in `tests/test_core.cpp` (ApplyingSeparatingClassification). CLI now surfaces applying/separating via `aspect` command optional speeds. GUI styling to follow.
- __[Parallels/Contra-parallels]__ Implemented as `detect_declination_aspect()` (core) with CLI command `declaspect`. Tests added in `tests/test_core.cpp` (Parallels/ContraParallels + Zero/Boundary cases). Next: integrate configurable `orbDecl` into CLI flags and GUI toggles/visuals.
- __[Pattern detection]__ Build higher-order patterns from detected aspects:
  - Implemented: T-square (two squares + an opposition), Grand Trine (three trines).
  - Use tolerance consistent with per-aspect orbs; avoid duplicate pattern reporting.
  - CLI `patterns` command lists detected patterns and indices.
- __[Configuration]__ Centralize orbs per aspect type and declination orb in config accessible from CLI/GUI.
 - __[Testing]__
  - (Done) Unit tests for applying/separating using synthetic ephemeris-like scenarios.
  - (Done) Tests for parallels/contra-parallels including zero/edge-orb cases.
  - (Done) Pattern detection tests on synthetic configurations (Grand Trine, T-square).
 - __[Integration]__
  - CLI: `aspect` prints applying/separating when speeds supplied; `declaspect` available; `patterns` lists detected patterns.
  - GUI: style applying vs separating (line style), toggle parallels; highlight patterns (TODO).

### 2. GUI Implementation

#### 2.1 Chart Display
- [x] Basic wheel rendering
- [x] Planet glyphs and positioning
- [x] House cusps and divisions (Equal, Placidus)
- [x] Aspect lines (configurable orb)
- [x] Visibility toggles: axes, labels, planets, aspect lines
- [x] Asc/MC labels
- [ ] Applying vs separating aspect styling (line style/color)
- [ ] Parallels/contra-parallels: overlay rendering and View toggle
- [ ] Highlight aspect patterns (T-square, Grand Trine)
- [ ] GUI support for additional house systems (Koch, Campanus, Regiomontanus, Porphyry, PorphyryNeo, Whole, Topocentric, Meridian, Morinus, EqualMid, Alcabitius)
- [ ] Planet set options: include Nodes and Chiron
- [x] Zodiac sign ring and glyphs (always visible, Unicode symbols)
- [ ] Customizable chart styles and colors
- [ ] Multiple chart types (natal, transit, synastry)
- [ ] Professional astrological font integration
- [ ] Enhanced planet glyph rendering (currently ASCII placeholders)

#### 2.2 Data Input and Management
- [x] Date/time input with timezone (German localized dialogs)
- [x] Location input (longitude/latitude) with validation
- [ ] Atlas integration for city lookup
- [ ] Chart data persistence (save/load)
- [ ] Chart comparison tools
- [ ] Legacy data file parsing (astronam.dat, astroger.dat, astroini.dat)

#### 2.3 Analysis Tools
- [x] Aspect grid display (separate window)
- [x] Aspect pattern detection and CLI output
- [x] Applying/separating aspect classification (core + CLI)
- [x] Declination parallels/contra-parallels (core + CLI)
- [ ] Planet and house position tables
- [ ] Aspect listing with interpretations
- [ ] Visual highlighting of aspect patterns in GUI
- [ ] Parallels/contra-parallels GUI display and toggle
- [ ] Transit forecasting
- [ ] Progressions and directions

### 3. Command Line Interface

#### 3.1 Basic Functionality
- [x] Chart calculation from command line
- [x] Aspect detection with speeds (`aspect` command)
- [x] Declination aspects (`declaspect` command)
- [x] Pattern detection (`patterns` command)
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
- [x] Unit tests for core calculations (comprehensive coverage)
- [x] Aspect detection and pattern recognition tests
- [x] House system validation across multiple latitudes
- [x] Applying/separating and declination aspect tests
- [ ] Integration tests
- [ ] Accuracy validation against standard ephemerides
- [ ] Performance benchmarking
- [ ] GUI automated testing

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

**High Priority:**
- [ ] Optimize chart drawing for smooth rendering (current bottleneck)
- [ ] Cache frequently used calculations (ephemeris lookups)
- [ ] Optimize aspect line rendering performance
- [ ] Memory usage optimization for large datasets

**Medium Priority:**
- [ ] Optimize planet calculations for speed
- [ ] Implement lazy loading for GUI components
- [ ] Background calculation threading
- [ ] Chart export optimization

### 6. Testing and Validation

- [x] Unit tests for core calculations (comprehensive)
- [x] House system accuracy tests
- [x] Aspect detection validation
- [ ] Validation against published ephemerides (Swiss Ephemeris comparison)
- [ ] GUI automated testing framework
- [ ] Cross-platform testing (Linux/Windows/macOS)
- [ ] Performance regression testing
- [ ] User acceptance testing

## Module-by-Module Implementation Sequence

1. **Core Astronomy Module** ✅ **COMPLETED**
   - ✅ Complete planet position calculations
   - ✅ Implement all house systems (12 systems implemented)
   - ✅ Complete aspect calculations with applying/separating
   - ✅ Declination aspects and pattern detection

2. **Chart Drawing Module** 🔄 **IN PROGRESS**
   - ✅ Basic chart wheel rendering
   - ✅ Add zodiac sign glyphs (Unicode symbols)
   - ✅ Implement aspect grid (separate window)
   - 🔄 Enhance planet glyph rendering (professional fonts needed)
   - 🔄 Visual styling for applying/separating aspects
   - 🔄 Pattern highlighting in GUI

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
