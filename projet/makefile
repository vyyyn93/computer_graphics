# Makefile pour compiler ex3.cpp avec GLShader.cpp et tiny_obj_loader.cc

# Compilateur
CXX = g++

# Options de compilation
CXXFLAGS = -Wall

# Bibliothèques nécessaires
LIBS = -lopengl32 -lglfw3 -lglew32

# Liste des fichiers source
SOURCES1 = src/scene1.cpp src/common/GLShader.cpp src/tiny_obj_loader.cc
SOURCES2 = src/scene2.cpp src/common/GLShader.cpp src/tiny_obj_loader.cc

# Nom de l'exécutable
TARGET1 = scene1
TARGET2 = scene2

all: $(TARGET1) $(TARGET2)

# Règle de construction de l'exécutable
$(TARGET1): $(SOURCES1)
	$(CXX) $(SOURCES1) -o $(TARGET1) $(LIBS)

$(TARGET2): $(SOURCES2)
	$(CXX) $(SOURCES2) -o $(TARGET2) $(LIBS)

# Règle pour nettoyer les fichiers temporaires
clean:
	rm -f scene*
