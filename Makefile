MODULES = planstatech
EXTENSION = planstatech
DATA = planstatech--1.0.sql
DOCS = README.md

PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
