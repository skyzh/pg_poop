MODULES = pg_poop
EXTENSION = pg_poop
DATA = pg_poop--0.0.1.sql
OBJS = pg_poop.o

PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
