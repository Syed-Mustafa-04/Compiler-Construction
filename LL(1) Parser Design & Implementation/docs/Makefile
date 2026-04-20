CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic -g
LDFLAGS  =
TARGET   = parser.exe
SRCDIR   = src
SRCS     = $(SRCDIR)/main.cpp \
           $(SRCDIR)/grammar.cpp \
           $(SRCDIR)/first_follow.cpp \
           $(SRCDIR)/parser.cpp \
           $(SRCDIR)/stack.cpp \
           $(SRCDIR)/tree.cpp \
           $(SRCDIR)/error_handler.cpp
OBJS     = $(SRCS:.cpp=.o)

# Default target
.PHONY: all
all: dirs $(TARGET)

# Create required directories if absent
.PHONY: dirs
dirs:
	@mkdir -p input output docs 2>/dev/null || true

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^
	@echo "Build successful  →  ./$(TARGET)"

$(SRCDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

.PHONY: run1 run2 run3
run1: all
	./$(TARGET) input/grammar1.txt

run2: all
	./$(TARGET) input/grammar2.txt

run3: all
	./$(TARGET) input/grammar3.txt

.PHONY: memcheck
memcheck: all
	valgrind --leak-check=full --error-exitcode=1 ./$(TARGET) input/grammar1.txt

.PHONY: clean
clean:
	rm -f $(OBJS) $(TARGET)
	@echo "Cleaned."

.PHONY: distclean
distclean: clean
	rm -f output/*.txt