COMP = g++-9.3.0
#put all your object files here
OBJS = main.o  NaiveAlgorithm.o Simulation.o WeightBalanceCalculator.o Port.o Utils.o Container.o  ErrorHandling.o AlgorithmValidator.o AlgorithmRegistrar.o AlgorithmRegistration.o

#The executabel filename DON'T CHANGE
EXEC = simulator
CPP_COMP_FLAG = -std=c++2a -Wall -Wextra -Werror -pedantic-errors -DNDEBUG
CPP_LINK_FLAG = -ldl -export-dynamic -lstdc++fs #(-rdynamic)

$(EXEC): $(OBJS)
	$(COMP) $(OBJS) $(CPP_LINK_FLAG) -o $@
$(OBJS):
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp -o $@
#a rule for building a simple c++ source file

clean:
	rm -f $(OBJS) $(EXEC)


