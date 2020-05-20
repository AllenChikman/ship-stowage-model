COMP = g++-9.3.0

MAIN = $\
main.o

ALGORITHMS = $\
./Algorithms/NaiveAlgorithm.o

SIMULATOR = $\
./Simulator/Simulation.o $\
./Simulator/AlgorithmRegistrar.o $\

INTERFACES = $\
./Interfaces/WeightBalanceCalculator.o $\
./Interfaces/AlgorithmRegistration.o

COMMON = $\
./Common/Port.o $\
./Common/Utils.o $\
./Common/Container.o $\
./Common/ErrorHandling.o $\
./Common/AlgorithmValidator.o $\

OBJS = $(MAIN) $(ALGORITHMS) $(SIMULATOR) $(INTERFACES) $(COMMON)

EXEC = joinedExe
CPP_COMP_FLAG = -std=c++2a -Wall -Wextra -Werror -pedantic-errors -DNDEBUG
CPP_LINK_FLAG = -ldl -export-dynamic -lstdc++fs #(-rdynamic)

$(EXEC): $(OBJS)
	$(COMP) $(OBJS) $(CPP_LINK_FLAG) -o $@
$(OBJS):
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp -o $@

clean:
	rm -f $(OBJS) $(EXEC)


