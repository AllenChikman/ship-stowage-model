COMP = g++-8.3.0
#put all your object files here
OBJS = main.o  NaiveAlgorithm.o  AbstractAlgorithm.o Ship.o Simulation.o WeightBalanceCalculator.o Port.o Utils.o Container.o  ErrorHandling.o AlgorithmValidator.o AlgorithmRegistration.o AlgorithmRegistrar.o

#The executabel filename DON'T CHANGE
EXEC = ex1
CPP_COMP_FLAG = -std=c++2a -Wall -Wextra -Werror -pedantic-errors -DNDEBUG -I./src
CPP_LINK_FLAG = -lstdc++fs

$(EXEC): $(OBJS)
	$(COMP) $(OBJS) $(CPP_LINK_FLAG) -o $@
$(OBJS):
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp
#a rule for building a simple c++ source file

clean:
	rm -f $(OBJS) $(EXEC)




