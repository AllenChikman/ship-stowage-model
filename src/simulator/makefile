COMP = g++-9.3.0

SIMULATOR_OBJS = $\
main.o $\
Simulation.o $\
AlgorithmRegistrar.o


COMMON_OBJS = $\
../common/Port.o $\
../common/Utils.o $\
../common/Container.o $\
../common/AlgorithmValidator.o

INTERFACE_OBJS = $\
../interfaces/WeightBalanceCalculator.o $\
../interfaces/AlgorithmRegistration.o $\

OBJS = $(SIMULATOR_OBJS) $(COMMON_OBJS) $(INTERFACE_OBJS)



EXEC = simulator
CPP_COMP_FLAG = -std=c++2a -Wall -Wextra -Werror -pedantic-errors -DNDEBUG
CPP_LINK_FLAG = -ldl -export-dynamic #(-rdynamic)

$(EXEC): $(OBJS)
	$(COMP) $(OBJS) $(CPP_LINK_FLAG) -o $@
$(OBJS):
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp -o $@

clean:
	rm -f $(OBJS) $(EXEC)
