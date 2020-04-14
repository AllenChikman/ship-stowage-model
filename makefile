COMP = g++-8.3.0
#put all your object files here
OBJS = main.o StowageAlgorithm.o Ship.o Simulation.o WeightBalanceCalculator.o Port.o Utils.o Container.o
#The executabel filename DON'T CHANGE
EXEC = ex1
COMMON = src/Common/
SIMULATOR = src/Simulator/
ALGO = src/StowageAlgorithm/
INCLUDE = src/Common/include/
CPP_COMP_FLAG = -std=c++2a -Wall -Wextra -Werror -pedantic-errors -DNDEBUG -I./src
CPP_LINK_FLAG = -lstdc++fs

$(OBJS):
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp
$(EXEC): $(OBJS)
	$(COMP) $(OBJS) $(CPP_LINK_FLAG) -o $@
#a rule for building a simple c++ source file

clean:
	rm -f $(OBJS) $(EXEC)