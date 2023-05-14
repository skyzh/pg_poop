#include "postgres.h"

#include "access/detoast.h"
#include "executor/executor.h"
#include "fmgr.h"
#include "optimizer/planner.h"
#include "tcop/dest.h"
#include "utils/lsyscache.h"
#include "utils/palloc.h"

PG_MODULE_MAGIC;

static ExecutorRun_hook_type prev_ExecutorRun_hook = NULL;

static int depth = 0;

struct _PoopReceiver {
  DestReceiver self;
  DestReceiver *recv;
};

typedef struct _PoopReceiver PoopReceiver;

static bool poopReceiveSlot(TupleTableSlot *slot, DestReceiver *self) {
  PoopReceiver *poop = (PoopReceiver *)self;

  TupleDesc typeinfo = slot->tts_tupleDescriptor;
  int natts = typeinfo->natts;
  bytea **bb = palloc_array(bytea *, natts);

  for (int i = 0; i < natts; ++i) {
    bb[i] = NULL;
    Oid ty = TupleDescAttr(typeinfo, i)->atttypid;
    if (ty != VARCHAROID && ty != TEXTOID) {
      continue;
    }
    bool isnull;
    Datum attr = slot_getattr(slot, i + 1, &isnull);
    int64 vallen = toast_raw_datum_size(attr) - VARHDRSZ;

    const char *poop_emoji = "\360\237\222\251";
    const int poop_emoji_len = 4;
    int64 bytea_len = vallen * poop_emoji_len + VARHDRSZ;
    bytea *b = (bytea *)palloc(bytea_len);
    SET_VARSIZE(b, bytea_len);
    for (int j = 0; j < vallen; j += 1) {
      memcpy(VARDATA(b) + j * poop_emoji_len, poop_emoji, poop_emoji_len);
    }
    bb[i] = b;

    slot->tts_values[i] = PointerGetDatum(b);
  }

  bool res = poop->recv->receiveSlot(slot, poop->recv);

  for (int i = 0; i < natts; ++i) {
    if (bb[i]) {
      pfree(bb[i]);
    }
  }

  pfree(bb);

  return res;
}

static void poopStartup(DestReceiver *self, int operation, TupleDesc typeinfo) {
  PoopReceiver *poop = (PoopReceiver *)self;
  poop->recv->rStartup(poop->recv, operation, typeinfo);
}

static void poopDestroy(DestReceiver *self) {
  PoopReceiver *poop = (PoopReceiver *)self;
  poop->recv->rDestroy(poop->recv);
}

static void poopShutdown(DestReceiver *self) {
  PoopReceiver *poop = (PoopReceiver *)self;
  poop->recv->rShutdown(poop->recv);
}

static PoopReceiver *make_poop_receiver(DestReceiver *recv) {
  PoopReceiver *r = palloc(sizeof(PoopReceiver));
  r->recv = recv;
  r->self.rStartup = poopStartup;
  r->self.rDestroy = poopDestroy;
  r->self.rShutdown = poopShutdown;
  r->self.receiveSlot = poopReceiveSlot;
  r->self.mydest = recv->mydest;
  return r;
}

static void pg_poop_executor_run_hook(QueryDesc *queryDesc,
                                      ScanDirection direction, uint64 count,
                                      bool execute_once) {
  depth += 1;

  if (queryDesc->dest != NULL && depth == 1) {
    MemoryContext oldcontext;
    oldcontext = MemoryContextSwitchTo(queryDesc->estate->es_query_cxt);
    PoopReceiver *dest = make_poop_receiver(queryDesc->dest);
    queryDesc->dest = (DestReceiver *)dest;
    MemoryContextSwitchTo(oldcontext);
  }

  if (prev_ExecutorRun_hook) {
    prev_ExecutorRun_hook(queryDesc, direction, count, execute_once);
  } else {
    standard_ExecutorRun(queryDesc, direction, count, execute_once);
  }
  depth -= 1;
}

void _PG_init(void) {
  prev_ExecutorRun_hook = ExecutorRun_hook;
  ExecutorRun_hook = pg_poop_executor_run_hook;
}
