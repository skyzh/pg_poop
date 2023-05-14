# pg_poop

This Postgres extension will rewrite all VARCHAR/TEXT columns with poop emojis. Only tested with pg15.

The extension was originally created as a demo plugin of [pgextmgrext][pgextmgrext], which is an extension manager (as
an extension) that extends Postgres hooks to make it easier to develop Postgres extensions. The version using pgext can
be found [here][pgext_pg_poop].

[pgextmgrext]: https://github.com/cmu-db/pgextmgrext/
[pgext_pg_poop]: https://github.com/cmu-db/pgextmgrext/tree/master/pgext_pg_poop

## Install

```shell
git clone https://github.com/skyzh/pg_poop && cd pg_poop
make PG_CONFIG={PATH TO pg_config} USE_PGXS=1 install
```

## Usage

```
pgextmgr=# select '1';
 ?column?
----------
 1
(1 row)

pgextmgr=# LOAD 'pg_poop';
LOAD
pgextmgr=# select '1';
 ?column?
----------
 💩
(1 row)
```

## How does it work?

pg_poop will use the `ExecutorRun` hook in Postgres and replace the output destination with pg_poop's custom destination
pipe. In the pipe, it will find all varchar/text columns, get the size of the datum, and replace it with poop emojis of
the same length.

One alternative way to do pg_poop is to hook the planner and wrap all varchar/text output column with a function that
replaces all chars with emoji, which is how it worked in PostgreSQL Anonymizer extension.
