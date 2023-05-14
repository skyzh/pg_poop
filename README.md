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
