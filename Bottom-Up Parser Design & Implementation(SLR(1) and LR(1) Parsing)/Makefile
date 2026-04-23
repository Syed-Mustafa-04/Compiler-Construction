CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra
TARGET = slr_parser
SRCDIR = src
OBJDIR = obj
SOURCES = $(SRCDIR)/main.cpp $(SRCDIR)/grammar.cpp $(SRCDIR)/items.cpp $(SRCDIR)/slr_parser.cpp $(SRCDIR)/lr1_parser.cpp $(SRCDIR)/stack.cpp $(SRCDIR)/tree.cpp
OBJECTS = $(OBJDIR)/main.o $(OBJDIR)/grammar.o $(OBJDIR)/items.o $(OBJDIR)/slr_parser.o $(OBJDIR)/lr1_parser.o $(OBJDIR)/stack.o $(OBJDIR)/tree.o

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJDIR) $(TARGET)

.PHONY: clean