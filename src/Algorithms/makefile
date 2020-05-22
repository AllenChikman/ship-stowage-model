COMP = g++-9.3.0

FIRST_ALGO_SO = _317624278_a.so
SECOND_ALGO_SO = _317624278_b.so

SHARED_OBJS = $(FIRST_ALGO_SO) $(SECOND_ALGO_SO)

FIRST_ALGO = _317624278_a.o
SECOND_ALGO = _317624278_b.o

ALGORITHM_OBJS = $\
NaiveAlgorithm.o


COMMON_OBJS = $\
../Common/Port.o $\
../Common/Utils.o $\
../Common/Container.o $\
../Common/AlgorithmValidator.o

INTERFACE_OBJS = $\
../Interfaces/WeightBalanceCalculator.o $\

OBJS = $(ALGORITHM_OBJS) $(COMMON_OBJS) $(INTERFACE_OBJS)

CPP_COMP_FLAG = -std=c++2a -Wall -Wextra -Werror -pedantic-errors -DNDEBUG -fvisibility=hidden -fvisibility-inlines-hidden -fPIC
CPP_LINK_FLAG = -shared

all: $(SHARED_OBJS)

$(FIRST_ALGO_SO): $(OBJS) $(FIRST_ALGO)
	$(COMP) $(CPP_LINK_FLAG) -o $@ $^

$(SECOND_ALGO_SO): $(OBJS) $(SECOND_ALGO)
	$(COMP) $(CPP_LINK_FLAG) -o $@ $^
	
$(OBJS) $(FIRST_ALGO) $(SECOND_ALGO):
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp -o $@
	
clean:
	rm -f $(OBJS) $(SHARED_OBJS)