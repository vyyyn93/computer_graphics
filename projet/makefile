# Makefile pour compiler ex3.cpp avec GLShader.cpp et tiny_obj_loader.cc

# Compilateur
CXX = g++

# Options de compilation
CXXFLAGS = -Wall

# Bibliothèques nécessaires
LIBS = -lopengl32 -lglfw3 -lglew32

# Liste des fichiers source
SOURCES = src/ex1.cpp src/common/GLShader.cpp src/tiny_obj_loader.cc

# Nom de l'exécutable
TARGET = ex1

# Règle de construction de l'exécutable
$(TARGET): $(SOURCES)
	$(CXX) $(SOURCES) -o $(TARGET) $(LIBS)

# Règle pour nettoyer les fichiers temporaires
clean:
	rm -f $(TARGET)
