#!/bin/bash
# Clean Script für Linux/macOS
# Entfernt alle Build-Artefakte

echo "Entferne Build-Artefakte..."

rm -rf build/
rm -rf cmake-build-*/
rm -f *.o
rm -f *.a
rm -f core/*.o
rm -f core/src/*.o

echo "✓ Bereinigung abgeschlossen"
