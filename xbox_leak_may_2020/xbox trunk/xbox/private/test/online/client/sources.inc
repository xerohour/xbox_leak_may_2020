TEST_INCLUDES=$(BASEDIR)\private\test\inc
TEST_ONLINE_INCLUDES=$(BASEDIR)\private\test\online\client\inc
TEST_ONLINE_LIBS=$(BASEDIR)\private\test\online\client\lib\i386

INCLUDES=\
	$(INCLUDES); \
	$(TEST_INCLUDES); \
	$(TEST_ONLINE_INCLUDES);
