TEST_SRC = $(wildcard test/test_*.cc)
TEST = $(patsubst test/test_%.cc, test/test_%, $(TEST_SRC))
$(info "$(CFLAGS)")
test/% : test/%.cc
	$(CXX) $(CFLAGS) -MM -MT test/$* $< >test/$*.d
	$(CXX) $(CFLAGS) -o $@ $(filter %.cc, $^)
